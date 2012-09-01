
#define _GNU_SOURCE
#include "ep1/net.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

void EP1_NET_init (EP1_NET_packet *packet) {
  /* Inicializa tamanho e aloca memória inicial padrão */
  packet->size = 0;
  packet->data = (char*)malloc((EP1_PACKETSIZE+1)*sizeof(char));
  bzero(packet->data, EP1_PACKETSIZE+1);
}

void EP1_NET_clear (EP1_NET_packet *packet) {
  /* Limpa valores e libera memória usada */
  packet->size = 0;
  free(packet->data);
  packet->data = NULL;
}

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
#ifdef EP1_DEBUG
  printf("[Cliente conectado no processo filho %d enviou:]\n",getpid());
  if ((fputs(recvpack->data,stdout)) == EOF) {
     perror("fputs :( \n");
     exit(6);
  }
#endif
  return 1;
}

int EP1_NET_send (int connfd, EP1_NET_packet* sendpack) {
#ifdef EP1_DEBUG
  puts(sendpack->data);
#endif
  /* Envia os dados do pacote para o cliente */
  return write(connfd, sendpack->data, sendpack->size) >= 0;
}

