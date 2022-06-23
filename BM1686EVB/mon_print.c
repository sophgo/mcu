#include <tick.h>
#include <stdlib.h>
#include <common.h>
#include <mon.h>
#include <stdio.h>
#include <debug.h>
static int mon_print_handle;

void mon_print_fun(void)
{
    printf("local tmp = %d, remote tmp = %d, i12v = %dmA\n",\
    get_board_temp(), get_soc_temp(), get_current());
}

void mon_print_init(void)
{
    mon_print_handle = tick_register_task(mon_print_fun, 0);
    if (mon_print_handle == -1)
        debug("mon print task register failed\n");
}

void enable_mon_print_task(int second)
{
    if (second == 0)
        tick_set_task_interval(mon_print_handle, 60000);
    else
        tick_set_task_interval(mon_print_handle, second*1000);
}
