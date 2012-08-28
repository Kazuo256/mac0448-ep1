
#ifndef EP1_SERVER_H_
#define EP1_SERVER_H_

#include <stdio.h>
#include "ep1/net.h"

#define EP1_DATATYPE_MEM  0
#define EP1_DATATYPE_IO   1

typedef union {
  unsigned char type;
  struct {
    unsigned char type;
    size_t        size;
    char          *content;
  } mem;
  struct {
    unsigned char type;
    size_t        size;
    FILE          *file;
    char          *header;
  } stream;
} EP1_SERVER_data;

void EP1_SERVER_accept (const EP1_NET_packet* req, EP1_SERVER_data* data);

int EP1_SERVER_respond (EP1_NET_packet* resp, EP1_SERVER_data* data);

#endif

