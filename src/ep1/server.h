
#ifndef EP1_SERVER_H_
#define EP1_SERVER_H_

#include <stdio.h>
#include "ep1/net.h"

#define EP1_HEADERSIZE    1024

void EP1_SERVER_respond (const EP1_NET_packet* req, EP1_NET_packet* resp);

#endif

