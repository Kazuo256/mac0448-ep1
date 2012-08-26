
#include "ep1/server.h"

#include <stdio.h>
#include <string.h>

void tryToGetPage(const EP1_NET_packet* req, FILE* returnPage) {
	char page[256];
	int i, firstLineSize = 0;
	for (i = 0; req->data[i] != '\n'; i++) firstLineSize++;
	if (firstLineSize >= 256) {
		returnPage = fopen("packets/notfound", "r");
	} else {
		strncpy(page, req->data, firstLineSize);
		page[firstLineSize] = '\0';
		if (strcmp("GET", strncpy(NULL, req->data, 3)) == 0 ) {
		puts(page);

		}
	}
}

void EP1_SERVER_respond (const EP1_NET_packet* req, EP1_NET_packet* resp) {
	FILE *returnPage; 

	tryToGetPage(req, returnPage);
  
  /* Lê o arquivo e guarda os dados no pacote */
  resp->size = fread(resp->data, 1, EP1_PACKETSIZE, returnPage);
  resp->data[resp->size] = '\0';
  fclose(returnPage);
}