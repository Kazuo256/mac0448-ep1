
#define _GNU_SOURCE
#include "ep1/date.h"

#include <stdio.h>
#include <time.h>

#include <sys/types.h>
#include <sys/stat.h>

static const char weekdays[7][4] = {
  "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
};

static const char months[12][4] = {
  "Jan", "Feb", "Mar", "Apr",
  "May", "Jun", "Jul", "Aug",
  "Sep", "Oct", "Nov", "Dec"
};

static void format_time (time_t *tp, date_buf buf) {
  struct tm date;
  gmtime_r(tp, &date);
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
  time_t    t;
  time(&t);
  format_time(&t, buf);
}

void EP1_DATE_lastmodified (const char* filepath, date_buf buf) {
  struct stat stat_buf;
  stat(filepath, &stat_buf);
  format_time(&stat_buf.st_mtime, buf);
}

