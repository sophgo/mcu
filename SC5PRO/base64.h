#ifndef __BASE64_H__
#define __BASE64_H__

int base64_encode(void *dst, void *src, unsigned long size);
int base64_decode(void *dst, void *src,unsigned long size);
void base64_show(void *str, unsigned long size);

#endif
