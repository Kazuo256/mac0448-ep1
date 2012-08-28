
#ifndef EP1_NET_H_
#define EP1_NET_H_

#include <stddef.h>

#define EP1_PACKETSIZE 4096

typedef struct {
  size_t  size;
  char    data[EP1_PACKETSIZE];
} EP1_NET_packet;

int EP1_NET_receive (int connfd, EP1_NET_packet *recvpack);

int EP1_NET_send (int connfd, EP1_NET_packet *sendpack);

#endif
