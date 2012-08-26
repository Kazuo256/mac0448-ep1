
#include "ep1/server.h"

#include <stdio.h>
#include <string.h>

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

void EP1_SERVER_respond (const EP1_NET_packet* req, EP1_NET_packet* resp) {
	FILE *returnPage = tryToGetPage(req);

  /* Lê o arquivo e guarda os dados no pacote */
  resp->size = fread(resp->data, 1, EP1_PACKETSIZE, returnPage);
  resp->data[resp->size] = '\0';
  fclose(returnPage);
}