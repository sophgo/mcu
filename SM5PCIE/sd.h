#ifndef __SD_H__
#define __SD_H__

int sd_init(void);
int sd_read(void *data, unsigned long sector, unsigned long num);
int sd_write(void *data, unsigned long sector, unsigned long num);
int sd_test(void);

#endif
