
#include "ep1/server.h"

#include <stdio.h>
#include <string.h>

void tryToGetPage(const EP1_NET_packet* req, FILE *returnPage) {
	char getReq[256], page[256];
	int i, firstLineSize = 0;
	for (i = 0; req->data[i] != '\n'; i++) firstLineSize++;
	if (firstLineSize >= 256) {
		returnPage = fopen("packets/notfound", "r");
	} else {
		strncpy(getReq, req->data, firstLineSize);
		getReq[firstLineSize] = '\0';
		if (strncmp("GET ", getReq, 4) == 0 ) {
			for (i = 4; i < 252; i++ ) {
				if (strncmp("HTTP", getReq+i, 4) == 0) {
					strncpy(page, getReq+4, i-4);
					page[i-5] = '\0';
				}
			}
		}
	}
}

void EP1_SERVER_respond (const EP1_NET_packet* req, EP1_NET_packet* resp) {
	FILE *returnPage = NULL; 

	tryToGetPage(req, returnPage);
  
  /* Lê o arquivo e guarda os dados no pacote */
  resp->size = fread(resp->data, 1, EP1_PACKETSIZE, returnPage);
  resp->data[resp->size] = '\0';
  fclose(returnPage);
}