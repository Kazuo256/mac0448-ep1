
#include "ep1.h"

#include <stdio.h>

#include "ep1/net.h"

static void packet_notfound (EP1_NET_packet* sendpack) {
  /* Pacote NOTFOUND encontra-se em packets/notfound */
  FILE *notfound = fopen("packets/notfound", "r");
  /* Lê o arquivo e guarda os dados no pacote */
  sendpack->size = fread(sendpack->data, 1, EP1_PACKETSIZE, notfound);
  sendpack->data[sendpack->size] = '\0';
  fclose(notfound);
}

void EP1_handle_connection (int connfd) {
  /* Armazena pacotes recebido do cliente */
  EP1_NET_packet  recvpack;
  /* Armazena pacotes enviados para o cliente */
  EP1_NET_packet  sendpack;
  /* Por enquanto, ouve todos os pacotes e responde com NOTFOUND */
  while (EP1_NET_receive(connfd, &recvpack)) {
    packet_notfound(&sendpack);
    if (!EP1_NET_send(connfd, &sendpack))
      perror("send packet failed\n");
  }
}

