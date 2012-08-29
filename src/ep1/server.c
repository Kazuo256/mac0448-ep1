
#define _GNU_SOURCE
#include "ep1/server.h"

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#define EP1_LINESIZE    1024
#define EP1_URISIZE     256
#define EP1_VERSIONSIZE 16
#define EP1_FORMATSIZE  16
#define EP1_METHODSIZE  8

typedef struct {
  char method[EP1_METHODSIZE+1];
  char uri[EP1_URISIZE+1];
  char version[EP1_VERSIONSIZE+1];
} request_line;

static void parse_reqline (const char* data, request_line* reqline) {
  char  line[EP1_LINESIZE];
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

typedef struct {
  FILE  *file;
  char  format[EP1_FORMATSIZE];
} response_file;

static void get_format (const char* uri, char* format) {
  size_t length = strlen(uri);
  if (strcmp(uri+length-5, ".html") == 0)
    strcpy(format, "text/html");
  else if (strcmp(uri+length-4, ".png") == 0)
    strcpy(format, "image/png");
  else
    strcpy(format, "text/html");
}

static void get_file (const char* uri, response_file* resp) {
  char  page[EP1_LINESIZE+1];
  page[0] = '\0';
  strcpy(page, "./www");
  strncat(page, uri, EP1_LINESIZE-5);
  page[EP1_LINESIZE] = '\0';
  puts(page);
  resp->file = fopen(page, "rb"); /* TODO: open binary? */
  get_format(uri, resp->format);
}

static const char *notfoundpacket = 
"HTTP/1.1 404 Not Found\n"
"Date: Sun, 31 Jul 2011 18:45:56 GMT\n"
"Server: ep1\n"
"Content-Length: %d\n"
"Keep-Alive: timeout=15, max=100\n"
"Connection: Keep-Alive\n"
"Content-Type: text/html; charset=iso-8859-1\n\n";

static const char *notfoundhtml =
"<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">\n"
"<html><head>\n"
"<title>404 Not Found</title>\n"
"</head><body>\n"
"<h1>Not Found</h1>\n"
"<p>The requested URL %s was not found on this server.</p>\n"
"</body></html>\n";

static void handle_notfound (const char* failed_uri, EP1_SERVER_data* data) {
  /* Buffer que guarda o código html gerado */
  char  line[EP1_LINESIZE+1];
  int   n;
  /* Inicializa estrutura de dados */
  data->type = EP1_DATATYPE_MEM;
  data->mem.size = 0;
  bzero(data->mem.content, EP1_PACKETSIZE);
  /* Gera código html para NOTFOUND */
  n = sprintf(line, notfoundhtml, failed_uri);
  if (n < 0) perror("html generation failed\n");
  /* Monta o pacote de resposta */
  data->mem.size = sprintf(data->mem.content, notfoundpacket, n);
  strcat(data->mem.content, line);
  data->mem.size += n;
}

static const char *okpacket =
"HTTP/1.1 200 OK\n"
"Date: Sun, 31 Jul 2011 18:41:04 GMT\n"
"Server: ep1\n"
"Last-Modified: Sun, 31 Jul 2011 17:26:08 GMT\n"
"Accept-Ranges: bytes\n"
"Content-Length: %d\n"
"Keep-Alive: timeout=15, max=100\n"
"Connection: Keep-Alive\n"
"Content-Type: %s\n\n";

static void handle_ok (response_file *response, EP1_SERVER_data* data) {
  /* Obtém tamanho do arquivo */
  long    file_size;
  size_t  check;
  fseek(response->file, 0, SEEK_END);
  file_size = ftell(response->file);
  fseek(response->file, 0, SEEK_SET);
  /* Inicializa estrutura de dados */
  data->type = EP1_DATATYPE_IO;
  data->stream.file_size = file_size;
  data->stream.file_data = (char*)malloc(file_size+1);
  check =
    fread(data->stream.file_data, sizeof(char), file_size, response->file);
  if (check != (size_t)file_size)
    puts("<<<<<<<<<< DANGER >>>>>>>>>>>>>");
  data->stream.file_data[file_size] = '\0';
  bzero(data->stream.header, EP1_HEADERSIZE);
  data->stream.header_size =
    sprintf(data->stream.header, okpacket, file_size, response->format);
  fclose(response->file);
}

void EP1_SERVER_accept (const EP1_NET_packet* req, EP1_SERVER_data* data) {
  /* Guarda a linha de requisição do pacote req */
  request_line reqline;
  /* Possível arquivo html mandado em resposta */
	response_file response;
  /* Lê a linha de requisição do pacote req */
  parse_reqline(req->data, &reqline);
  /* Tenta carregar página HTML (TODO verificar método GET) */
  if (strcmp(reqline.uri, "/") == 0)
    strcat(reqline.uri, "index.html");
  get_file(reqline.uri, &response);
  if (response.file != NULL)
    handle_ok(&response, data);     /* 200 OK */
  else
    handle_notfound(reqline.uri, data); /* 404 NOT FOUND */
}

static int respond_mem (EP1_NET_packet* resp, EP1_SERVER_data* data) {
  if (data->mem.size) {
    /* coloca dados no pacote */
    strncpy(resp->data, data->mem.content, data->mem.size);
    resp->size = data->mem.size;
    /* sinaliza fim da resposta */
    data->mem.size = 0;
    return 1;
  } else return 0;
}

static int respond_io (EP1_NET_packet* resp, EP1_SERVER_data* data) {
  if (data->stream.header_size) {
    /* Monta pacote com cabeçalho */
    strncpy(resp->data, data->stream.header, data->stream.header_size);
    resp->size = data->stream.header_size;
    /* Indica que já enviou o cabeçalho */
    data->stream.header_size = 0;
  } else if (data->stream.file_size > 0) {
    /* Copia os dados do arquivo para o pacote */
    resp->data = (char*)realloc(resp->data, data->stream.file_size+1);
    memcpy(resp->data, data->stream.file_data, data->stream.file_size+1);
    resp->size = (size_t)data->stream.file_size;
    /* Limpa os dados usados */
    data->stream.file_size = 0;
    free(data->stream.file_data);
    data->stream.file_data = NULL;
  } else return 0;
  return 1;
}

typedef int (*response_func) (EP1_NET_packet*, EP1_SERVER_data*);

static response_func responses[2] = {
  respond_mem,
  respond_io
};

int EP1_SERVER_respond (EP1_NET_packet* resp, EP1_SERVER_data* data) {
  return responses[data->type] (resp, data);
}

