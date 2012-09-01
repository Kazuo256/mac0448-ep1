
#define _GNU_SOURCE
#include "ep1/date.h"

#include <stdio.h>
#include <time.h>

#include <sys/types.h>
#include <sys/stat.h>

/* Nomes dos dias da semana */
static const char weekdays[7][4] = {
  "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
};

/* Nomes dos meses */
static const char months[12][4] = {
  "Jan", "Feb", "Mar", "Apr",
  "May", "Jun", "Jul", "Aug",
  "Sep", "Oct", "Nov", "Dec"
};

/* Converte tempo absoluto em uma string de data formatada. */
static void format_time (time_t *tp, date_buf buf) {
  /* Armazena a data desejada */
  struct tm date;
  /* Obtém as informações da data */
  gmtime_r(tp, &date);
  /* Gera a string formatada seguindo o mesmo padrão que o Apache. */
  sprintf(
    buf,
    "%s, %02d %s %04d %02d:%02d:%02d GMT",
    weekdays[date.tm_wday],
    date.tm_mday,
    months[date.tm_mon],
    1900+date.tm_year,
    date.tm_hour,
    date.tm_min,
    date.tm_sec
  );
}

void EP1_DATE_current (date_buf buf) {
  /* Aramzena o tempo absoluto atual */
  time_t t;
  /* Obtém o tempo atual */
  time(&t);
  /* Produz a data formatada */
  format_time(&t, buf);
}

void EP1_DATE_lastmodified (const char* filepath, date_buf buf) {
  /* Armazena as informações do arquivo em questão */
  struct stat stat_buf;
  /* Obtém as informações do arquivo */
  stat(filepath, &stat_buf);
  /* Produz a data formatada da última modificação no arquivo */
  format_time(&stat_buf.st_mtime, buf);
}

