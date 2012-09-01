
#define _GNU_SOURCE
#include "ep1/date.h"

#include <stdio.h>
#include <time.h>

static const char weekdays[7][4] = {
  "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
};

static const char months[12][4] = {
  "Jan", "Feb", "Mar", "Apr",
  "May", "Jun", "Jul", "Aug",
  "Sep", "Oct", "Nov", "Dec"
};

void EP1_DATE_current (date_buf buf) {
  time_t    t;
  struct tm date;
  time(&t);
    gmtime_r(&t, &date);
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

