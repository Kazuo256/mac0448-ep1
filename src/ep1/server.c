
#define _GNU_SOURCE
#include "ep1/server.h"

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#define EP1_LINESIZE    1024
#define EP1_HEADERSIZE  1024
#define EP1_URISIZE     512
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
  char page[EP1_URISIZE+1];
  page[0] = '\0';
  strcpy(page, "./www");
  strncat(page, uri, EP1_URISIZE-5);
  page[EP1_URISIZE] = '\0';
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

static void handle_notfound (const char* failed_uri, EP1_NET_packet* resp) {
  /* Buffer que guarda o código html gerado */
  char  buffer[EP1_HEADERSIZE+1];
  int   n;
  /* Gera código html para NOTFOUND */
  n = sprintf(buffer, notfoundhtml, failed_uri);
  if (n < 0) perror("html generation failed\n");
  /* Monta o pacote de resposta */
  resp->size = sprintf(resp->data, notfoundpacket, n);
  strcat(resp->data, buffer);
  resp->size += n;
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

static void handle_ok (response_file *response, EP1_NET_packet* resp) {
  long    file_size;
  size_t  check;
  char    buffer[EP1_HEADERSIZE];
  /* Obtém tamanho do arquivo */
  fseek(response->file, 0, SEEK_END);
  file_size = ftell(response->file);
  fseek(response->file, 0, SEEK_SET);
  /* Gera cabeçalho */
  resp->size = 
    sprintf(buffer, okpacket, file_size, response->format);
  /* Gera pacote */
  resp->data = (char*)realloc(resp->data, resp->size+file_size+1);
  strcpy(resp->data, buffer);
  check =
    fread(resp->data+resp->size, sizeof(char), file_size, response->file);
  if (check != (size_t)file_size)
    puts("<<<<<<<<<< DANGER >>>>>>>>>>>>>");
  resp->size += file_size;
  resp->data[resp->size] = '\0';
  fclose(response->file);
}

void EP1_SERVER_respond (const EP1_NET_packet* req, EP1_NET_packet* resp) {
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
    handle_ok(&response, resp);         /* 200 OK */
  else
    handle_notfound(reqline.uri, resp); /* 404 NOT FOUND */
}

