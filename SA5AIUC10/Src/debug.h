#ifndef __DEBUG_H__
#define __DEBUG_H__

int mprintf(const char *fmt, ...);

int dbg_enable(void);
int dbg_disable(void);
int dbg_restore(void);

#ifdef DEBUG
#define dprintf	mprintf
#else
#define dprintf(fmt, ...)	do {} while(0)
#endif

#endif
