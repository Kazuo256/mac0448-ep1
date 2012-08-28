
#ifndef EP1_SERVER_H_
#define EP1_SERVER_H_

#include "ep1/net.h"

typedef struct {
  size_t size,
         capacity;
  char   *content,
         *last;
} EP1_SERVER_data;

void EP1_SERVER_accept (const EP1_NET_packet* req, EP1_SERVER_data* data);

int EP1_SERVER_respond (EP1_NET_packet* resp, EP1_SERVER_data* data);

#endif

