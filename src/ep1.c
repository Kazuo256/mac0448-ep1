
#include "ep1.h"

#include <stdio.h>

#include "ep1/net.h"
#include "ep1/server.h"

void EP1_handle (int connfd) {
  /* Armazena pacotes recebido do cliente */
  EP1_NET_packet  recvpack;
  /* Armazena pacotes enviados para o cliente */
  EP1_NET_packet  sendpack;
  /* Armazena dados usados pelo servidor */
  EP1_SERVER_data server_data;
  /* Por enquanto, ouve todos os pacotes e responde com NOTFOUND */
  while (EP1_NET_receive(connfd, &recvpack)) {
    EP1_SERVER_accept(&recvpack, &server_data);
    while (EP1_SERVER_respond(&sendpack, &server_data))
      if (!EP1_NET_send(connfd, &sendpack))
        perror("send packet failed\n");
  }
}

