
#include "responses.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>

void send_notfound (int connfd) {
  const char *dunno =
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
    "M10w~.@$A$%)  REow{lN\\BXt1Be=%Jrl/i'PiAp]ixnG<NdLIsJvrl{A`Ug%W~r";
  /*write(connfd, recvline, strlen(recvline));*/
  write(connfd, dunno, strlen(dunno));
}

