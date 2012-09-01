
#ifndef EP1_DATE_H_
#define EP1_DATE_H_

#define EP1_DATESIZE 32

/* Tipo de buffer usado para manter datas formatadas. */
typedef char date_buf[EP1_DATESIZE];

/* Obtém a data atual.
 * Parâmetros:
 *  buf: Buffer onde a data formatada será armazenada. */
void EP1_DATE_current (date_buf buf);

/* Obtém a data da última modificação de um determinado arquivo.
 * Parâmetros:
 *  filepath: Caminho para o arquivo, relativo "current working directory".
 *  buf: Buffer onde a data formatada será armazenada. */
void EP1_DATE_lastmodified (const char* filepath, date_buf buf);

#endif

