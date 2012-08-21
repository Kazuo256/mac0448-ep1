
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
  int   i = 0, j;
  sscanf(data, "%[^\r\n]", line);
  /* Como cada conexão está em um processo diferente, usamos strtok_r ao
   * invés da versão simples strtok. */
  token = strtok_r(line, " \t", &saveptr);
  strncpy(reqline->method, token, EP1_METHODSIZE);
  token = strtok_r(NULL, " \t", &saveptr);
  strncpy(reqline->uri, token, EP1_URISIZE);
  token = strtok_r(NULL, " \t", &saveptr);
  strncpy(reqline->version, token, EP1_VERSIONSIZE);
  puts(reqline->uri);
}

static const char *notfoundpacket = 
"HTTP/1.1 404 Not Found\n"
"Date: Sun, 31 Jul 2011 18:45:56 GMT\n"
"Server: ep1\n"
"Content-Length: %u\n"
"Keep-Alive: timeout=15, max=100\n"
"Connection: Keep-Alive\n"
"Content-Type: text/html; charset=iso-8859-1\n"
"\n";
static const char *notfoundhtml =
"<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">\n"
"<html><head>\n"
"<title>404 Not Found</title>\n"
"</head><body>\n"
"<h1>Not Found</h1>\n"
"<p>The requested URL %s was not found on this server.</p>\n"
"</body></html>\n";

void EP1_SERVER_respond (const EP1_NET_packet* req, EP1_NET_packet* resp) {
  /* Pacote NOTFOUND encontra-se em packets/notfound */
  FILE *notfound = fopen("packets/notfound", "r");
  /* Guarda a linha de requisição do pacote req */
  request_line reqline;
  /* Lê a linha de requisição do pacote req */
  parse_reqline(req->data, &reqline);
  printf(notfoundpacket, strlen(notfoundhtml));
  printf(notfoundhtml, reqline.uri);
  /* Lê o arquivo e guarda os dados no pacote resp */
  resp->size = fread(resp->data, 1, EP1_PACKETSIZE, notfound);
  resp->data[resp->size] = '\0';
  fclose(notfound);
}

