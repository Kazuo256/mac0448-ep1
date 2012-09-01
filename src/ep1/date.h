
#ifndef EP1_DATE_H_
#define EP1_DATE_H_

#define EP1_DATESIZE 32

typedef char date_buf[EP1_DATESIZE];

void EP1_DATE_current (date_buf buf);

void EP1_DATE_lastmodified (const char* filepath, date_buf buf);

#endif

