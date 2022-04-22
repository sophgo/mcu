#ifndef __YMODEM_H__
#define __YMODEM_H__

unsigned long xmodem_receive(int (*save)(void *priv, void *data,
					 unsigned long len),
			     void *priv);

#endif
