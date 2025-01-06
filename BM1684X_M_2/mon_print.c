#include <tick.h>
#include <stdlib.h>
#include <common.h>
#include <mon.h>
#include <stdio.h>
#include <debug.h>
#include <mp5475.h>
static int mon_print_handle;

void mon_print_fun(void)
{
    printf("board tmp = %d(C), soc tmp = %d(C), i12v = %d(mA)\n",\
    get_board_temp(), get_soc_temp(), get_current());

    for (int i = 0; i < 4; i++){
        printf("buck%c:%lu(mV) ", 'A' + i, mp5475_output_voltage(i));
    }

    printf("mp5475 tmp = %d(C)\n", mp5475_get_tmp());
}

void mon_print_init(void)
{
    mon_print_handle = tick_register_task(mon_print_fun, 0);
    if (mon_print_handle == -1)
        debug("mon print task register failed\n");
    else
        debug("!!!!mon print task register success!!!!\n");
}

void enable_mon_print_task(int second)
{
    if (second == 0)
        tick_set_task_interval(mon_print_handle, 60000);
    else
        tick_set_task_interval(mon_print_handle, second*1000);
}
