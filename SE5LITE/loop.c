#include <loop.h>
#include <stdbool.h>

static struct loop {
	int num;
	loop_entry entry[MAIN_LOOP_ENTRY_MAX];
} loop_ctx;

void loop_add(loop_entry entry)
{
	loop_ctx.entry[loop_ctx.num] = entry;
	++loop_ctx.num;
}

void loop_start(void)
{
	int i;

	while (true) {
		for (i = 0; i < loop_ctx.num; ++i)
			loop_ctx.entry[i]();
	}
}
