
#include "ep1.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define MAXLINE 4096

/*
"HTTP/1.1 404 Not Found"
"Date: Sun, 31 Jul 2011 18:45:56 GMT"
"Server: Apache/2.2.9"
"Vary: Accept-Encoding"
"Content-Encoding: gzip"
"Content-Length: 186"
"Keep-Alive: timeout=15, max=100"
"Connection: Keep-Alive"
"Content-Type: text/html; charset=iso-8859-1"
""
"M10w~.@$A$%)  REow{lN\\BXt1Be=%Jrl/i'PiAp]ixnG<NdLIsJvrl{A`Ug%W~r"
*/

void send_notfound (int connfd, char sendline[]) {
  FILE    *notfound = fopen("packets/notfound", "r");
  size_t  n;
  n = fread(sendline, 1, MAXLINE, notfound);
  sendline[n] = '\0';
  /*write(connfd, recvline, strlen(recvline));*/
  puts(sendline);
  if (write(connfd, sendline, n) == -1)
    perror("send packet failed\n");
}

void EP1_handle_connection (int connfd) {
  /* Armazena linhas recebidas do cliente */
  char    recvline[MAXLINE+1];
  /* Armazena linhas a serem enviadas para o cliente */
  char    sendline[MAXLINE+1];
  /* Armazena o tamanho da string lida do cliente */
  ssize_t n;
  while ((n=read(connfd, recvline, MAXLINE)) > 0) {
    recvline[n]=0;
    printf("[Cliente conectado no processo filho %d enviou:]\n",getpid());
    if ((fputs(recvline,stdout)) == EOF) {
       perror("fputs :( \n");
       exit(6);
    }
    /*write(connfd, recvline, strlen(recvline));*/
    send_notfound(connfd, sendline);
  }
}

