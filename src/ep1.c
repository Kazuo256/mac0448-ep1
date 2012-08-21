
#include "ep1.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "ep1/net.h"

static void packet_notfound (EP1_NET_packet* sendpack) {
  FILE    *notfound = fopen("packets/notfound", "r");
  sendpack->size = fread(sendpack->data, 1, EP1_PACKETSIZE, notfound);
  sendpack->data[sendpack->size] = '\0';
  fclose(notfound);
}

void EP1_handle_connection (int connfd) {
  /* Armazena pacotes recebido do cliente */
  EP1_NET_packet  recvpack;
  /* Armazena pacotes enviados para o cliente */
  EP1_NET_packet  sendpack;
  while (EP1_NET_receive(connfd, &recvpack)) {
    packet_notfound(&sendpack);
    if (!EP1_NET_send(connfd, &sendpack))
      perror("send packet failed\n");
  }
}

