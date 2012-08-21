
#include "ep1/server.h"

#include <stdio.h>

void EP1_SERVER_respond (const EP1_NET_packet* req, EP1_NET_packet* resp) {
  /* Pacote NOTFOUND encontra-se em packets/notfound */
  FILE *notfound = fopen("packets/notfound", "r");
  /* Lê o arquivo e guarda os dados no pacote */
  resp->size = fread(resp->data, 1, EP1_PACKETSIZE, notfound);
  resp->data[resp->size] = '\0';
  fclose(notfound);
}

