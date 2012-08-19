
#include "responses.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define MAX_LINE 4096

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

void send_notfound (int connfd) {
  char buffer[MAX_LINE+1];
  FILE *notfound = fopen("packets/notfound", "r");
  int n;
  n = fread(buffer, 1, MAX_LINE, notfound);
  buffer[n] = '\0';
  /*write(connfd, recvline, strlen(recvline));*/
  puts(buffer);
  write(connfd, buffer, n);
}

