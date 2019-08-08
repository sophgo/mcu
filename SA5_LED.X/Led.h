/* Microchip Technology Inc. and its subsidiaries.  You may use this software 
 * and any derivatives exclusively with Microchip products. 
 * 
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES, WHETHER 
 * EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED 
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A 
 * PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION 
 * WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION. 
 *
 * IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
 * INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
 * WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS 
 * BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE 
 * FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS 
 * IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF 
 * ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE 
 * TERMS. 
 */

/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef XC_HEADER_TEMPLATE_H
#define	XC_HEADER_TEMPLATE_H

#include <xc.h> // include processor files - each processor file is guarded.  

// TODO Insert appropriate #include <>
#include "User_define.h"
// TODO Insert C++ class definitions if appropriate

// TODO Insert declarations

//#include <stdio.h>  
//#define LED_SWITCH(name) led_ ## name ## _switch(void) {}
//
//#define PRINT_XN(n) printf("x" #n " = %d/n", x ## n);
//
//LED_SWITCH(hdd)
//LED_SWITCH(eth)
//LED_SWITCH(sys)
//LED_SWITCH(fan)
//LED_SWITCH(pwr)        
//
//typedef void (* led_set)(void);
//
//struct led_func_switch {
//    led_set set;
//    int led_index;
//};
///*
//* led0  R:HDD_GOOD Y:HDD_FAULT
// * led1  R:ETH_GOOD Y:ETH_FAULT
// * led2  R:SYS_GOOD Y:SYS_FAULT
// * led3  R:FAN_GOOD Y:FAN_FAULT
// * led4  R:PWR_FAULT   Y:PWR_GOOD
// */
//struct led_func_switch led_func_table[] = {
//    {led_hdd_switch, 0},
//    {led_eth_switch, 1},
//    {led_sys_switch, 2},
//    {led_fan_switch, 3},
//    {led_pwr_switch, 4},
//};
void led_hdd_switch(void);
void led_eth_switch(void);
void led_sys_switch(void);
void led_fan_switch(void);
void led_pwr_switch(void);
void set_frequence(void);

typedef void (* led_set)(void);

struct led_func_switch {
    led_set set;
    int led_index;
};

struct led_func_switch led_func_table[] = {
    {led_hdd_switch, 0},
    {led_eth_switch, 1},
    {led_sys_switch, 2},
    {led_fan_switch, 3},
    {led_pwr_switch, 4},
};
// Comment a function and leverage automatic documentation with slash star star
/**
    <p><b>Function prototype:</b></p>
  
    <p><b>Summary:</b></p>

    <p><b>Description:</b></p>

    <p><b>Precondition:</b></p>

    <p><b>Parameters:</b></p>

    <p><b>Returns:</b></p>

    <p><b>Example:</b></p>
    <code>
 
    </code>

    <p><b>Remarks:</b></p>
 */
// TODO Insert declarations or function prototypes (right here) to leverage 
// live documentation

#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

    // TODO If C++ is being used, regular C code needs function names to have C 
    // linkage so the functions can be used by the c code. 

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* XC_HEADER_TEMPLATE_H */

