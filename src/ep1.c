
#define _GNU_SOURCE
#include "ep1.h"

#include <stdio.h>
#include <strings.h>

#include "ep1/net.h"
#include "ep1/server.h"

void EP1_handle (int connfd) {
  /* Armazena pacotes recebido do cliente */
  EP1_NET_packet  recvpack;
  /* Armazena pacotes enviados para o cliente */
  EP1_NET_packet  sendpack;
  /* Inicializa pacote */
  EP1_NET_init(&recvpack);
  /* Por enquanto, ouve todos os pacotes e responde com NOTFOUND */
  while (EP1_NET_receive(connfd, &recvpack)) {
    /* Inicializa pacote */
    EP1_NET_init(&sendpack);
    /* Obtém resposta do servidor */
    EP1_SERVER_respond(&recvpack, &sendpack);
    /* Envia pacote */
    if (!EP1_NET_send(connfd, &sendpack))
      perror("send packet failed\n");
    /* Limpa pacote para ser usado de novo */
    EP1_NET_clear(&sendpack);
  }
  /* Limpa pacote */
  EP1_NET_clear(&recvpack);
}

