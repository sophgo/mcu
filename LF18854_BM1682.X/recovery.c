/*******************************************************************************
PIC16LF18854 BM1682 CORE BOARD
RECOVERY CODE
Author: kexuan.guo 
BITMAIN 
DATE: 12/12/2018
/****************************************************************/
#include"User_define.h"
#include"recovery.h"
#include "uart.h"


unsigned long last_send_time = 0;

int uart_issend()
{
    int a;
    a = Sencond_Count-last_send_time;
    if (a<5)
        uart_send = 1;
    else
        uart_send = 0;
    return uart_send;
}
void uart_send_recovery()
{
    last_send_time = Sencond_Count;
    uart_send = 1;
    //uart_send_bytes(&recovery, sizeof(recovery));
}
void test_send_recovery()
{
    uart_send_recovery();
}