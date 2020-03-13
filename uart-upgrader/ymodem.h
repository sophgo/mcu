#ifndef __YMODEM_H__
#define __YMODEM_H__

unsigned long ymodem_receive(int (*save)(void *data, unsigned long len),
			     unsigned long length);

#endif
