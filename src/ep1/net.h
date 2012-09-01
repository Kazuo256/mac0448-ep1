
#ifndef EP1_NET_H_
#define EP1_NET_H_

#include <stddef.h>

/* Tamanho inicial padrão dos pacotes */
#define EP1_PACKETSIZE 4096

/* Estrutura que representa um pacote "bruto", com apenas os bytes a serem
 * enviados e a quantidade deles. Deve ser inicializado com EP1_NET_init antes
 * de ser usado, e deve ser finalizado com EP1_NET_clear após o término de seu
 * uso. */
typedef struct {
  size_t  size;
  char    *data;
} EP1_NET_packet;

/* Inicializa um pacote.
 * Parâmetros:
 *  packet: O pacote a ser inicializado. */
void EP1_NET_init (EP1_NET_packet* packet);

/* Libera a memória usada por um pacote inicializado anteriormente através de
 * uma chamada a EP1_NET_init().
 * Parâmetros:
 *  packer: O pacote a ser finalizado. */
void EP1_NET_clear (EP1_NET_packet* packet);

/* Recebe um pacote através de uma conexão por socket.
 * Devolve 1 se recebeu algum pacote, e 0 caso contrário.
 * Parâmetros:
 *  connfd: Descritor da conexão pela qual quer se receber o pacote.
 *  recvpacket: Ponteiro para a estrutura onde será armazenado o pacote
 *              recebido. */
int EP1_NET_receive (int connfd, EP1_NET_packet* recvpack);

/* Envia um pacote através de uma conexão por socket.
 * Devolve 1 se conseguiu enviar o pacote, e 0 caso contrário.
 * Parâmetros:
 *  connfd: Descritor da conexão pela qual quer se enviar o pacote.
 *  rendpack: Ponteiro para a estrutura de será obtido o pacote a ser
 *            enviado. */
int EP1_NET_send (int connfd, EP1_NET_packet* sendpack);

#endif

