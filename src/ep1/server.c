
#define _GNU_SOURCE
#include "ep1/server.h"

#include <stdio.h>
#include <ctype.h>
#include <string.h>

#define EP1_LINESIZE    256
#define EP1_URISIZE     256
#define EP1_VERSIONSIZE 16
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

FILE* get_page (const char* uri) {
  char  page[EP1_LINESIZE+1];
  page[0] = '\0';
  strcpy(page, "./www");
  strncat(page, uri, EP1_LINESIZE-5);
  page[EP1_LINESIZE] = '\0';
  puts(page);
  return fopen(page, "r");
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
static const char *okpacket =
"HTTP/1.1 200 OK\n"
"Date: Sun, 31 Jul 2011 18:41:04 GMT\n"
"Server: ep1\n"
"Last-Modified: Sun, 31 Jul 2011 17:26:08 GMT\n"
"ETag: \"21838f9-1ed-4a960cf6e557a\"\n"
"Accept-Ranges: bytes\n"
"Content-Length: %d\n"
"Keep-Alive: timeout=15, max=100\n"
"Connection: Keep-Alive\n"
"Content-Type: text/html\n\n";

void EP1_SERVER_respond (const EP1_NET_packet* req, EP1_NET_packet* resp) {
  /* Buffer que guarda o código html gerado */
  char line[EP1_LINESIZE];
  /* Guarda o tamanho do código htmp gerado */
  int n;
  /* Guarda a linha de requisição do pacote req */
  request_line reqline;
  /* Possível arquivo html mandado em resposta */
	FILE *response_page;
  /* Lê a linha de requisição do pacote req */
  parse_reqline(req->data, &reqline);
  /* Tenta carregar página HTML (TODO verificar método GET) */
  response_page = get_page(reqline.uri);
  if (0 && response_page != NULL) {
    /* TODO esse código é um cpy-paste. IMPLEMENTAR SOLUÇÃO */
    /* Pega código html */
    n = sprintf(line, notfoundhtml, reqline.uri);
    if (n < 0) perror("html generation failed\n");
    /* Monta o pacote de resposta */
    resp->size = sprintf(resp->data, notfoundpacket, n);
    strcat(resp->data, line);
    resp->size += n;
  } else {
    /* Gera código html para NOTFOUND */
    n = sprintf(line, notfoundhtml, reqline.uri);
    if (n < 0) perror("html generation failed\n");
    /* Monta o pacote de resposta */
    resp->size = sprintf(resp->data, notfoundpacket, n);
    strcat(resp->data, line);
    resp->size += n;
  }
}

