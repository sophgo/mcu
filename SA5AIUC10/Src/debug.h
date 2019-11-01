#ifndef __DEBUG_H__
#define __DEBUG_H__

int mprintf(const char *fmt, ...);

#ifdef DEBUG
#define dprintf	mprintf
#else
#define dprintf(fmt, ...)	do {} while(0)
#endif

#endif
