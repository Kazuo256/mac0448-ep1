
#ifndef EP1_H_
#define EP1_H_

/* Cuida da conexão usando um servidor limitado.
 * Retorna quando a conexão for encerrada por falta de requisições do cliente.
 * Parâmetros:
 *  connfd: Descritor da conexão a ser tratada. */
void EP1_handle (int connfd);

#endif

