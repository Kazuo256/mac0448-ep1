
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

FILE* tryToGetPage(const EP1_NET_packet* req) {
	char getReq[256], page[256];
	int i, firstLineSize = 0;
	FILE* returnPage = NULL;

	for (i = 0; req->data[i] != '\n'; i++) firstLineSize++;
	if (firstLineSize >= 256) {
		returnPage = fopen("packets/notfound", "r");
	} else {
		strncpy(getReq, req->data, firstLineSize);
		getReq[firstLineSize] = '\0';
		if (strncmp("GET ", getReq, 4) == 0 ) {
			for (i = 4; i < 252; i++ ) {
				if (strncmp("HTTP", getReq+i, 4) == 0) {
					strcpy(page, ".www");
					strncat(page, getReq+4, i-4);
					page[i-1] = '\0';
					puts(page);
					returnPage = fopen(page, "r");
					if (returnPage == NULL) {
						returnPage = fopen("packets/notfound", "r");
					}
				}
			}
		}
	}
	return returnPage;
}

static const char *notfoundpacket = 
"HTTP/1.1 404 Not Found\n"
"Date: Sun, 31 Jul 2011 18:45:56 GMT\n"
"Server: ep1\n"
"Content-Length: %d\n"
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
  /* Buffer que guarda o código html gerado */
  char line[EP1_LINESIZE];
  /* Guarda o tamanho do código htmp gerado */
  int n;
  /* Guarda a linha de requisição do pacote req */
  request_line reqline;
	/*FILE *returnPage = tryToGetPage(req);*/
  /* Lê a linha de requisição do pacote req */
  parse_reqline(req->data, &reqline);
  /* Gera código html, sempre NOTFOUND */
  n = sprintf(line, notfoundhtml, reqline.uri);
  if (n < 0) perror("html generation failed\n");
  /* Monta o pacote de resposta */
  resp->size = sprintf(resp->data, notfoundpacket, n);
  strcat(resp->data, line);
  resp->size += n;
}

