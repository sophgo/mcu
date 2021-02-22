#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <sys/time.h>
#include <timer.h>
#include <log.h>

/*
 * porting guide:
 *	on linux kernel, you can use ktime_get_ns to replace posix gettimeofday.
 *	see: https://www.kernel.org/doc/html/latest/core-api/timekeeping.html
 */

static uint64_t __attribute__((unused)) current_time(void)
{
	struct timeval tv;
	int err;

	err = gettimeofday(&tv, NULL);

	if (err) {
		error("cannot get system time\n");
		assert(false);
	}

	/* to milliseconds */
	return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

int timer_start(struct timer_ctx *ctx, int timeout)
{
	ctx->timestamp = current_time();
	ctx->timeout = timeout;

	return 0;
}

void timer_stop(struct timer_ctx *ctx)
{
}

int timer_remain(struct timer_ctx *ctx)
{
	uint64_t time_elapse, time_remain;

	time_elapse = current_time() - ctx->timestamp;

	/* make sure wait at least timeout milliseconds */
	if (time_elapse == ctx->timeout)
		time_remain = 1;
	else if (time_elapse > ctx->timeout)
		time_remain = 0;
	else
		time_remain = ctx->timeout - time_elapse;

	return time_remain;
}
