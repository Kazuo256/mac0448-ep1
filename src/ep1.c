
#include "ep1.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define MAXPACKET 4096

typedef struct {
  char    data[MAXPACKET];
  size_t  size;
} EP1_packet;

static int receive (int connfd, EP1_packet* recvpack) {
  ssize_t n;
  n = read(connfd, recvpack->data, MAXPACKET);
  if (n <= 0) return 0;
  recvpack->data[n] = '\0';
  recvpack->size = (size_t)n;
  printf("[Cliente conectado no processo filho %d enviou:]\n",getpid());
  if ((fputs(recvpack->data,stdout)) == EOF) {
     perror("fputs :( \n");
     exit(6);
  }
  return 1;
}

static int send (int connfd, EP1_packet* sendpack) {
  puts(sendpack->data);
  /*write(connfd, recvline, strlen(recvline));*/
  return write(connfd, sendpack->data, sendpack->size) >= 0;
}

static void packet_notfound (EP1_packet* sendpack) {
  FILE    *notfound = fopen("packets/notfound", "r");
  sendpack->size = fread(sendpack->data, 1, MAXPACKET, notfound);
  sendpack->data[sendpack->size] = '\0';
  fclose(notfound);
}

void EP1_handle_connection (int connfd) {
  /* Armazena pacotes recebido do cliente */
  EP1_packet  recvpack;
  /* Armazena pacotes enviados para o cliente */
  EP1_packet  sendpack;
  while (receive(connfd, &recvpack)) {
    packet_notfound(&sendpack);
    if (!send(connfd, &sendpack))
      perror("send packet failed\n");
  }
}

