
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
  recvpack->size = strlen(recvpack->data);/*(size_t)n;*/
  printf("[Cliente conectado no processo filho %d enviou:]\n",getpid());
  if ((fputs(recvpack->data,stdout)) == EOF) {
     perror("fputs :( \n");
     exit(6);
  }
  return 1;
}

static int send (int connfd, EP1_packet* sendpack) {
  puts(sendpack->data);
  return write(connfd, sendpack->data, sendpack->size) >= 0;
}

static void packet_notfound (EP1_packet* sendpack) {
  FILE    *notfound = fopen("packets/notfound", "r");
  sendpack->size = fread(sendpack->data, 1, MAXPACKET, notfound);
  sendpack->data[sendpack->size] = '\0';
  fclose(notfound);
}

static size_t send_notfound (char sendline[]) {
  FILE    *notfound = fopen("packets/notfound", "r");
  size_t  n;
  n = fread(sendline, 1, MAXPACKET, notfound);
  fclose(notfound);
  sendline[n] = '\0';
  return n;
}

void EP1_handle_connection (int connfd) {
  /* Armazena pacotes recebido do cliente */
  EP1_packet  recvpack;
  /* Armazena pacotes enviados para o cliente */
  EP1_packet  sendpack;
  while (receive(connfd, &recvpack)) {
    /*write(connfd, recvline, strlen(recvline));*/
    packet_notfound(&sendpack);
    puts(sendpack.data);
    if (write(connfd, sendpack.data, sendpack.size) == -1)
      perror("send packet failed\n");
  }
}

