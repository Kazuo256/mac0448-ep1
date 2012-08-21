
#include "ep1/server.h"

#include <stdio.h>

void EP1_SERVER_answer_packet (EP1_NET_packet* recv, EP1_NET_packet* send) {
  /* Pacote NOTFOUND encontra-se em packets/notfound */
  FILE *notfound = fopen("packets/notfound", "r");
  /* Lê o arquivo e guarda os dados no pacote */
  send->size = fread(send->data, 1, EP1_PACKETSIZE, notfound);
  send->data[send->size] = '\0';
  fclose(notfound);
}

