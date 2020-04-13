#include "integer.h"        /* Type redefinition for portability */
#include <tick.h>

static unsigned long start_time;
static unsigned long timeout;

void SPI_Timer_On (WORD ms)
{
	timeout = ms;
	start_time = tick_get();
}

BOOL SPI_Timer_Status (void)
{
	return tick_get() - start_time <= timeout;
}

void SPI_Timer_Off (void)
{
}
