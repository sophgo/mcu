#include "utc.h"
#include "uart.h"
#include "util.h"

static char utc_str[] = "!DATETIME[01234567]";

static char needutc = 0;
static unsigned long last_utc_time = 0;

static unsigned long g_utc = 0;

void utcsend_beg(unsigned long curtime)
{
	last_utc_time = curtime;
	needutc = 1;
}
void utcsend_run(unsigned long curtime)
{
	if (needutc)
	{
		if (curtime - last_utc_time < 5)
		{
			uart_send_bytes(&utc_str,sizeof(utc_str));
		}
		else
		{
			needutc = 0;
		}
	}
}

void utc_set(unsigned char* utctime4)
{
	g_utc = *(unsigned long *)&utctime4[0];
}

void utc_get(unsigned char* utctime4)
{
	unsigned char uc8[8];
	// update array
	*(long *)&utctime4[0] = g_utc;
	// update str
#if 0	
	asc2hex(&utc_str[10],&utctime8[0],8);
#else
	asc2hex(&uc8[0],&utctime4[0],4);
	utc_str[10 +  0] = uc8[6];
	utc_str[10 +  1] = uc8[7];
	utc_str[10 +  2] = uc8[4];
	utc_str[10 +  3] = uc8[5];
	utc_str[10 +  4] = uc8[2];
	utc_str[10 +  5] = uc8[3];
	utc_str[10 +  6] = uc8[0];
	utc_str[10 +  7] = uc8[1];
#endif
}

void utc_inc()
{
	g_utc ++;
}

