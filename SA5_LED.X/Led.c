/*
 * File:   Led.c
 * Author: taiqiang.cao
 *
 * Created on June 3, 2019, 2:39 PM
 */


#include "Led.h"

/***************
 key  led0 led1  led2
 *         led3  led4 
 ***************/
/*'
 * R: red  Y: yellow
 * led0  R:HDD_GOOD Y:HDD_FAULT
 * led1  R:ETH_GOOD Y:ETH_FAULT
 * led2  R:SYS_GOOD Y:SYS_FAULT
 * led3  R:FAN_GOOD Y:FAN_FAULT
 * led4  R:PWR_FAULT   Y:PWR_GOOD
 */
void led_hdd_switch(void)
{
    if (I2C_Array[0] > 3)//process in ISR
    {
       if (I2C_Array[0] & BIT0 == 0)
       {
           one_hot_led = BIT0;
       }
       else {
           one_hot_led = BIT1;
       }
       return ;       
    }
    
    HDD_GOOD = I2C_Array[0] & 0x01;
    __delay_ms(1);//wait 1 millisecond for stable
    HDD_FAULT = (I2C_Array[0] >> 1) & 0x01;
    
    return;
}

void led_eth_switch(void)
{
    if (I2C_Array[1] > 3)//process in ISR
    {
       if (I2C_Array[1] & BIT0 == 0)
       {
           one_hot_led = BIT2;
       }
       else {
           one_hot_led = BIT3;
       }
       return ;       
    }
        
    ETH_GOOD = I2C_Array[1] & 0x01;
    __delay_ms(1);//wait 1 millisecond for stable
    ETH_FAULT = (I2C_Array[1] >> 1) & 0x01;
    
    return;
}

void led_sys_switch(void)
{
    if (I2C_Array[2] > 3)//process in ISR
    {
       if (I2C_Array[2] & BIT0 == 0)
       {
           one_hot_led = BIT4;
       }
       else {
           one_hot_led = BIT5;
       }
       return ;       
    }
        
    SYS_GOOD = I2C_Array[2] & 0x01;
    __delay_ms(1);//wait 1 millisecond for stable
    SYS_FAULT = (I2C_Array[2] >> 1) & 0x01;
    
    return;
}

void led_fan_switch(void)
{
    if (I2C_Array[3] > 3)//process in ISR
    {
       if (I2C_Array[3] & BIT0 == 0)
       {
           one_hot_led = BIT6;
       }
       else {
           one_hot_led = BIT7;
       }
       return ;       
    }
        
    FAN_GOOD = I2C_Array[3] & 0x01;
    __delay_ms(1);//wait 1 millisecond for stable
    FAN_FAULT = (I2C_Array[3] >> 1) & 0x01;
    
    return;
}

void led_pwr_switch(void)
{
    if (I2C_Array[4] > 3)//process in ISR
    {
       if (I2C_Array[4] & BIT0 == 0)
       {
           one_hot_led = BIT8;
       }
       else {
           one_hot_led = BIT9;
       }
       return ;       
    }
        
    PWR_FAULT = I2C_Array[4] & 0x01;
    __delay_ms(1);//wait 1 millisecond for stable
    PWR_GOOD = (I2C_Array[4] >> 1) & 0x01;
    
    return;
}

static pwr_cnt = 0;
static pwr_status = 0;

void led_bd_pwr(void)
{
    pwr_status = LED_BD_PWR;
    __delay_ms(500);
    if ((LED_BD_PWR == pwr_status) && (LED_BD_PWR == 0)) {  //key press ,light on  
        if (pwr_cnt % 2 == 0)
            pwr_cnt = 0;
        I2C_Array[5] = pwr_cnt % 2;
        pwr_cnt++;
    }
    
    if (I2C_Array[5] == 0)
    {
        LED_PWR_P = 0;
        __delay_ms(1);//wait 1 millisecond for stable
        LED_PWR_N = 1;
    } else {
        LED_PWR_P = 1;
        __delay_ms(1);//wait 1 millisecond for stable
        LED_PWR_N = 0;
    }
    return;
}

static uid_cnt = 0;
static uid_status = 0;

void led_bd_uid(void)
{
    uid_status = LED_BD_UID;
    __delay_ms(500);
    if ((uid_status == LED_BD_UID) &&(LED_BD_UID == 0)) {
        if (uid_cnt % 2 == 0)
            uid_cnt = 0;
        I2C_Array[6] = uid_cnt % 2;
        uid_cnt++;
    }
    
    if (I2C_Array[6] == 0)
    {
        LED_UID_P = 0;
        __delay_ms(1);//wait 1 millisecond for stable
        LED_UID_N = 1;
    } else {
        LED_UID_P = 1;
        __delay_ms(1);//wait 1 millisecond for stable
        LED_UID_N = 0;
    }
    return;
}
/*
 * 0100 = 1:16
 * 0011 = 1:8
 * 0010 = 1:4
 * 0001 = 1:2
 * 0000 = 1:1
 */
void set_frequence(void)
{
    switch(I2C_Array[8])
    {
        case 1:
            T0CON1bits.T0CKPS = 0b0000;//1:1 Prescaler
            break;
        case 2:
            T0CON1bits.T0CKPS = 0b0001;//1:2 Prescaler
            break;
        case 4:
            T0CON1bits.T0CKPS = 0b0010;//1:4 Prescaler
            break;
        case 8:
            T0CON1bits.T0CKPS = 0b0011;//1:8 Prescaler
            break;
        case 16:
            T0CON1bits.T0CKPS = 0b0100;//1:16 Prescaler
            break;
        default :
            T0CON1bits.T0CKPS = 0b0011;//1:1 Prescaler
            break;
    }   
}
