
#ifndef EP1_SERVER_H_
#define EP1_SERVER_H_

#include <stdio.h>
#include "ep1/net.h"

/* Produz uma resposta do servidor HTTP a uma requisição do cliente.
 * Parâmetros:
 *  req: Pacote com a requisição HTTP do cliente.
 *  resp: Ponteiro para o pacote onde será armazenada a resposta HTTP do
 *        servidor. */
void EP1_SERVER_respond (const EP1_NET_packet* req, EP1_NET_packet* resp);

#endif

