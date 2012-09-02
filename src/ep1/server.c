
#define _GNU_SOURCE
#include "ep1/server.h"

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "ep1/date.h"

/* Vários tamanhos de buffers com finalidades distintas */
#define EP1_LINESIZE    1024
#define EP1_HEADERSIZE  1024
#define EP1_URISIZE     512
#define EP1_DATASIZE    128
#define EP1_VERSIONSIZE 16
#define EP1_FORMATSIZE  16
#define EP1_METHODSIZE  8

/* Estrutura que representa uma linha de requisição de HTTP */
typedef struct {
  char method[EP1_METHODSIZE+1];
  char uri[EP1_URISIZE+1];
  char version[EP1_VERSIONSIZE+1];
} request_line;

/* Lê a linha de requisição do pacote */
static void parse_reqline (const char* data, request_line* reqline) {
  /* Armazena a linha de requisição */
  char  line[EP1_LINESIZE];
  /* Usados para obter os membros da linha de requisição */
  char  *token, *saveptr;
  sscanf(data, "%[^\r\n]", line);
  /* Como cada conexão está em um processo diferente, usamos strtok_r ao
   * invés da versão simples strtok. */
  token = strtok_r(line, " \t", &saveptr);
  strncpy(reqline->method, token, EP1_METHODSIZE);
  token = strtok_r(NULL, " \t", &saveptr);
  strncpy(reqline->uri, token, EP1_URISIZE);
  token = strtok_r(NULL, " \t", &saveptr);
  strncpy(reqline->version, token, EP1_VERSIONSIZE);
}

/* Estrutura que representa um arquivo de resposta a o método GET */
typedef struct {
  FILE  *file;
  char  format[EP1_FORMATSIZE];
  char  path[EP1_URISIZE+1];
} response_file;

/* Obtém o formato de um arquivo requisitado. Tem suporte apenas a textos HTML
 * e imagens PNG, que são os únicos formatos exigidos pelo enunciado. Qualquer
 * outro formato de arquivo será tratado como texto HTML. */
static void get_format (const char* uri, char* format) {
  size_t length = strlen(uri);
  if (strcmp(uri+length-5, ".html") == 0)
    strcpy(format, "text/html");
  else if (strcmp(uri+length-4, ".png") == 0)
    strcpy(format, "image/png");
  else
    strcpy(format, "text/html");
}

/* Obtém um arquivo no servidor a partir de uma URI */
static void get_file (const char* uri, response_file* resp) {
  resp->path[0] = '\0';
  strcpy(resp->path, "./www");
  strncat(resp->path, uri, EP1_URISIZE-5);
  resp->path[EP1_URISIZE] = '\0';
#ifdef EP1_DEBUG
  puts(resp->path);
#endif
  resp->file = fopen(resp->path, "rb");
  get_format(uri, resp->format);
}

/* Estrutura que representa informação obtida através do método POST */
typedef struct {
  size_t contentlength;
  char postfieldone[EP1_DATASIZE];
  char postdataone[EP1_DATASIZE];
  char postfieldtwo[EP1_DATASIZE];
  char postdatatwo[EP1_DATASIZE];
} post_info;

static void get_postinfo (const char* data, post_info* postinfo) {
  /* Enfiar os roles pra pegar as infos aqui, e colocar no postinfo */
  char *saveptr, *token;
  char *savenumber, *number;
  char *saveinfo, *info;
  char line[EP1_PACKETSIZE+1];

  strcpy(line, data);
  /* Encontrando o tamanho do post. */
  token = strtok_r(line, "\r\n", &saveptr);
  while (strncmp(token, "Content-Length:", 15)) 
    token = strtok_r(NULL, "\r\n", &saveptr);
  number = strtok_r(token, " \r", &savenumber);
  number = strtok_r(NULL, " \r", &savenumber);
  postinfo->contentlength = atoi(number);
  /* Encontrando a mensagem do post. */
  strcpy(line, data);
  saveptr = NULL;
  token = strtok_r(line, "\r\n", &saveptr);
  while (strcmp(token, "\r") != 0)
    token = strtok_r(NULL, "\n", &saveptr);
  token = strtok_r(NULL, "\n", &saveptr);
  info = strtok_r(token, "=&\n", &saveinfo);
  strncpy(postinfo->postfieldone, info, EP1_DATASIZE);
  info = strtok_r(NULL, "=&\n", &saveinfo);
  strncpy(postinfo->postdataone, info, EP1_DATASIZE);
  info = strtok_r(NULL, "=&\n", &saveinfo);
  strncpy(postinfo->postfieldtwo, info, EP1_DATASIZE);
  info = strtok_r(NULL, "=&\n", &saveinfo);
  strncpy(postinfo->postdatatwo, info, EP1_DATASIZE);
}

/* Molde para o pacote do tipo 404 NOT FOUND.
 * Precisa de 2 informações:
 *  Data de envio
 *  Tamanho do arquivo HTML enviado */
static const char *notfoundpacket = 
"HTTP/1.1 404 Not Found\n"
"Date: %s\n"
"Server: ep1\n"
"Content-Length: %d\n"
"Keep-Alive: timeout=15, max=100\n"
"Connection: Keep-Alive\n"
"Content-Type: text/html; charset=iso-8859-1\n\n";

/* Molde para o arquivo HTML enviado com pacote de tipo 404 NOT FOUND.
 * Precisa de 1 informação?
 *  URI do arquivo não encontrado */
static const char *notfoundhtml =
"<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">\n"
"<html><head>\n"
"<title>404 Not Found</title>\n"
"</head><body>\n"
"<h1>Not Found</h1>\n"
"<p>The requested URL %s was not found on this server.</p>\n"
"</body></html>\n";

/* Molde para o arquivo HTML enviado em resposta ao método POST.
 * Precisa de 2 informações:
 *  A string enviada no primeiro campo
 *  A string enviada no segundo campo */
static const char *posthtml =
"<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">\n"
"<html><head>\n"
"<title>Conteudo do POST</title>\n"
"</head><body>\n"
"<h1>Post</h1>\n"
"<p>%s = %s</p>\n"
"</b>\n"
"<p>%s = %s</p>\n"
"</body></html>\n";

/* Monta o pacote de resposta 404 NOT FOUND */
static void handle_notfound (const char* failed_uri, EP1_NET_packet* resp) {
  /* Buffer que guarda o código html gerado */
  char      buffer[EP1_HEADERSIZE+1];
  date_buf  date;
  int       n;
  /* Gera código html para NOTFOUND */
  n = sprintf(buffer, notfoundhtml, failed_uri);
  if (n < 0) perror("html generation failed\n");
  /* Monta o pacote de resposta */
  EP1_DATE_current(date);
  resp->size = sprintf(resp->data, notfoundpacket, date, n);
  strcat(resp->data, buffer);
  resp->size += n;
}

/* Molde para o pacote do tipo 200 OK.
 * Precisa de 4 informações:
 *  Data de envio
 *  Data da última modificação do arquivo enviado
 *  Tamanho do arquivo enviado
 *  Formato do arquivo enviado */
static const char *okpacket =
"HTTP/1.1 200 OK\n"
"Date: %s\n"
"Server: ep1\n"
"Last-Modified: %s\n"
"Accept-Ranges: bytes\n"
"Content-Length: %d\n"
"Keep-Alive: timeout=15, max=100\n"
"Connection: Keep-Alive\n"
"Content-Type: %s\n\n";

/* Monta o pacote de resposta 200 OK em resposta a métodos GET */
static void handle_ok (response_file *response, EP1_NET_packet* resp) {
  long      file_size;
  size_t    check;
  char      buffer[EP1_HEADERSIZE];
  date_buf  date, lastmod;
  /* Obtém tamanho do arquivo */
  fseek(response->file, 0, SEEK_END);
  file_size = ftell(response->file);
  fseek(response->file, 0, SEEK_SET);
  /* Gera cabeçalho */
  EP1_DATE_current(date);
  EP1_DATE_lastmodified(response->path, lastmod);
  resp->size = 
    sprintf(buffer, okpacket, date, lastmod, file_size, response->format);
  /* Gera pacote */
  resp->data = (char*)realloc(resp->data, resp->size+file_size+1);
  strcpy(resp->data, buffer);
  check =
    fread(resp->data+resp->size, sizeof(char), file_size, response->file);
  if (check != (size_t)file_size)
    puts("[Algo de muito errado aconteceu]\n");
  resp->size += file_size;
  resp->data[resp->size] = '\0';
  /* Fecha o arquivo. */
  fclose(response->file);
}

/* Monta o pacote de resposta 200 OK em resposta a métodos POST */
static void handle_post (post_info* postinfo, EP1_NET_packet* resp) {
  /* Buffer que guarda o código html gerado */
  char      buffer[EP1_HEADERSIZE+1];
  int       n;
  date_buf  date;
  /* Gera código html para post. */
  n = sprintf(buffer, posthtml, postinfo->postfieldone, 
                postinfo->postdataone, postinfo->postfieldtwo,postinfo->postdatatwo);
  if (n < 0) perror("html generation failed\n");
  /* Monta o pacote de resposta */
  EP1_DATE_current(date);
  resp->size = sprintf(resp->data, okpacket, date, date, n, "text/html");
  strcat(resp->data, buffer);
  resp->size += n;
}

void EP1_SERVER_respond (const EP1_NET_packet* req, EP1_NET_packet* resp) {
  /* Guarda a linha de requisição do pacote req */
  request_line reqline;
  /* Possível arquivo html mandado em resposta */
  response_file response;
  /* Possível informações do POST */
  post_info postinfo;
  /* Lê a linha de requisição do pacote req */
  parse_reqline(req->data, &reqline);
  /* Verifica se é método GET ou POST */
  if (strcmp(reqline.method, "GET") == 0) {
    /* Caso especial / redireciona para /index.html */
    if (strcmp(reqline.uri, "/") == 0)
      strcat(reqline.uri, "index.html");
    /* Tenta carregar página HTML */
    get_file(reqline.uri, &response);
    /* 200 OK */
    if (response.file != NULL)
      handle_ok(&response, resp);
    /* 404 NOT FOUND */
    else
      handle_notfound(reqline.uri, resp);
  } else if (strcmp(reqline.method, "POST") == 0) {
    /* Pega informações enviadas via POST*/
    get_postinfo(req->data, &postinfo);
    /* E monta a resposta */
    handle_post(&postinfo, resp);
  } else printf("[Método %s não suportado]\n", reqline.method);
}

