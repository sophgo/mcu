/* 
 * File:   recovery.h
 * Author: kexuan.guo
 *
 * Created on December 12, 2018, 5:00 PM
 */

#ifndef RECOVERY_H
#define	RECOVERY_H

extern unsigned long int Sencond_Count;
char recovery[] = {"!recovery!recovery!recovery"};


void uart_send_recovery();
int uart_issend();
void test_send_recovery();

#endif	/* RECOVERY_H */