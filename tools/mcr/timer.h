#ifndef __TIMER_H__
#define __TIMER_H__

#include <stdint.h>

struct timer_ctx {
	uint64_t timestamp;
	uint64_t timeout;
};

/**
 * start a timer
 *
 * @param timeout timeout of this timer
 * @return 0 on success, or an error number is returned
 */
int timer_start(struct timer_ctx *ctx, int timeout);

/**
 * check if a timer is timed out
 *
 * @param ctx timer context
 * @return 0 if timer is timed out, otherwise a positive number to indicate how
 * long it will take until timed out.
 */
int timer_remain(struct timer_ctx *ctx);

/**
 * stop the timer
 *
 * @param ctx timer context
 */
void timer_stop(struct timer_ctx *ctx);

#endif
