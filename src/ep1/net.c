
#include "ep1/net.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int EP1_NET_receive (int connfd, EP1_NET_packet* recvpack) {
  /* Armazena tamanho da string lida do cliente */
  ssize_t n;
  /* Lê dados enviados pelo cliente */
  n = read(connfd, recvpack->data, EP1_PACKETSIZE);
  /* Devolve falha se não leu nenhum dado ou se deu erro */
  if (n <= 0) return 0;
  /* Caso contrário, monta o pacote */
  recvpack->data[n] = '\0';
  recvpack->size = (size_t)n;
  printf("[Cliente conectado no processo filho %d enviou:]\n",getpid());
  if ((fputs(recvpack->data,stdout)) == EOF) {
     perror("fputs :( \n");
     exit(6);
  }
  return 1;
}

int EP1_NET_send (int connfd, EP1_NET_packet* sendpack) {
  /* Envia os dados do pacote para o cliente */
  puts(sendpack->data);
  /*write(connfd, recvline, strlen(recvline));*/
  return write(connfd, sendpack->data, sendpack->size) >= 0;
}

