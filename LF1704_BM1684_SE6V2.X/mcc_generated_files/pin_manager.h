/**
  @Generated Pin Manager Header File

  @Company:
    Microchip Technology Inc.

  @File Name:
    pin_manager.h

  @Summary:
    This is the Pin Manager file generated using PIC10 / PIC12 / PIC16 / PIC18 MCUs

  @Description
    This header file provides APIs for driver for .
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.81.8
        Device            :  PIC16LF1704
        Driver Version    :  2.11
    The generated drivers are tested against the following:
        Compiler          :  XC8 2.36 and above
        MPLAB 	          :  MPLAB X 6.00	
*/

/*
    (c) 2018 Microchip Technology Inc. and its subsidiaries. 
    
    Subject to your compliance with these terms, you may use Microchip software and any 
    derivatives exclusively with Microchip products. It is your responsibility to comply with third party 
    license terms applicable to your use of third party software (including open source software) that 
    may accompany Microchip software.
    
    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY 
    IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS 
    FOR A PARTICULAR PURPOSE.
    
    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP 
    HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO 
    THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL 
    CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT 
    OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS 
    SOFTWARE.
*/

#ifndef PIN_MANAGER_H
#define PIN_MANAGER_H

/**
  Section: Included Files
*/

#include <xc.h>

#define INPUT   1
#define OUTPUT  0

#define HIGH    1
#define LOW     0

#define ANALOG      1
#define DIGITAL     0

#define PULL_UP_ENABLED      1
#define PULL_UP_DISABLED     0

// get/set POWER_KEY aliases
#define POWER_KEY_TRIS                 TRISAbits.TRISA0
#define POWER_KEY_LAT                  LATAbits.LATA0
#define POWER_KEY_PORT                 PORTAbits.RA0
#define POWER_KEY_WPU                  WPUAbits.WPUA0
#define POWER_KEY_OD                   ODCONAbits.ODA0
#define POWER_KEY_ANS                  ANSELAbits.ANSA0
#define POWER_KEY_SetHigh()            do { LATAbits.LATA0 = 1; } while(0)
#define POWER_KEY_SetLow()             do { LATAbits.LATA0 = 0; } while(0)
#define POWER_KEY_Toggle()             do { LATAbits.LATA0 = ~LATAbits.LATA0; } while(0)
#define POWER_KEY_GetValue()           PORTAbits.RA0
#define POWER_KEY_SetDigitalInput()    do { TRISAbits.TRISA0 = 1; } while(0)
#define POWER_KEY_SetDigitalOutput()   do { TRISAbits.TRISA0 = 0; } while(0)
#define POWER_KEY_SetPullup()          do { WPUAbits.WPUA0 = 1; } while(0)
#define POWER_KEY_ResetPullup()        do { WPUAbits.WPUA0 = 0; } while(0)
#define POWER_KEY_SetPushPull()        do { ODCONAbits.ODA0 = 0; } while(0)
#define POWER_KEY_SetOpenDrain()       do { ODCONAbits.ODA0 = 1; } while(0)
#define POWER_KEY_SetAnalogMode()      do { ANSELAbits.ANSA0 = 1; } while(0)
#define POWER_KEY_SetDigitalMode()     do { ANSELAbits.ANSA0 = 0; } while(0)

// get/set POWER_LED aliases
#define POWER_LED_TRIS                 TRISAbits.TRISA1
#define POWER_LED_LAT                  LATAbits.LATA1
#define POWER_LED_PORT                 PORTAbits.RA1
#define POWER_LED_WPU                  WPUAbits.WPUA1
#define POWER_LED_OD                   ODCONAbits.ODA1
#define POWER_LED_ANS                  ANSELAbits.ANSA1
#define POWER_LED_SetHigh()            do { LATAbits.LATA1 = 1; } while(0)
#define POWER_LED_SetLow()             do { LATAbits.LATA1 = 0; } while(0)
#define POWER_LED_Toggle()             do { LATAbits.LATA1 = ~LATAbits.LATA1; } while(0)
#define POWER_LED_GetValue()           PORTAbits.RA1
#define POWER_LED_SetDigitalInput()    do { TRISAbits.TRISA1 = 1; } while(0)
#define POWER_LED_SetDigitalOutput()   do { TRISAbits.TRISA1 = 0; } while(0)
#define POWER_LED_SetPullup()          do { WPUAbits.WPUA1 = 1; } while(0)
#define POWER_LED_ResetPullup()        do { WPUAbits.WPUA1 = 0; } while(0)
#define POWER_LED_SetPushPull()        do { ODCONAbits.ODA1 = 0; } while(0)
#define POWER_LED_SetOpenDrain()       do { ODCONAbits.ODA1 = 1; } while(0)
#define POWER_LED_SetAnalogMode()      do { ANSELAbits.ANSA1 = 1; } while(0)
#define POWER_LED_SetDigitalMode()     do { ANSELAbits.ANSA1 = 0; } while(0)

// get/set POWER_ON aliases
#define POWER_ON_TRIS                 TRISCbits.TRISC2
#define POWER_ON_LAT                  LATCbits.LATC2
#define POWER_ON_PORT                 PORTCbits.RC2
#define POWER_ON_WPU                  WPUCbits.WPUC2
#define POWER_ON_OD                   ODCONCbits.ODC2
#define POWER_ON_ANS                  ANSELCbits.ANSC2
#define POWER_ON_SetHigh()            do { LATCbits.LATC2 = 1; } while(0)
#define POWER_ON_SetLow()             do { LATCbits.LATC2 = 0; } while(0)
#define POWER_ON_Toggle()             do { LATCbits.LATC2 = ~LATCbits.LATC2; } while(0)
#define POWER_ON_GetValue()           PORTCbits.RC2
#define POWER_ON_SetDigitalInput()    do { TRISCbits.TRISC2 = 1; } while(0)
#define POWER_ON_SetDigitalOutput()   do { TRISCbits.TRISC2 = 0; } while(0)
#define POWER_ON_SetPullup()          do { WPUCbits.WPUC2 = 1; } while(0)
#define POWER_ON_ResetPullup()        do { WPUCbits.WPUC2 = 0; } while(0)
#define POWER_ON_SetPushPull()        do { ODCONCbits.ODC2 = 0; } while(0)
#define POWER_ON_SetOpenDrain()       do { ODCONCbits.ODC2 = 1; } while(0)
#define POWER_ON_SetAnalogMode()      do { ANSELCbits.ANSC2 = 1; } while(0)
#define POWER_ON_SetDigitalMode()     do { ANSELCbits.ANSC2 = 0; } while(0)

// get/set IO_RC5 aliases
#define IO_RC5_TRIS                 TRISCbits.TRISC5
#define IO_RC5_LAT                  LATCbits.LATC5
#define IO_RC5_PORT                 PORTCbits.RC5
#define IO_RC5_WPU                  WPUCbits.WPUC5
#define IO_RC5_OD                   ODCONCbits.ODC5
#define IO_RC5_ANS                  ANSELCbits.ANSC5
#define IO_RC5_SetHigh()            do { LATCbits.LATC5 = 1; } while(0)
#define IO_RC5_SetLow()             do { LATCbits.LATC5 = 0; } while(0)
#define IO_RC5_Toggle()             do { LATCbits.LATC5 = ~LATCbits.LATC5; } while(0)
#define IO_RC5_GetValue()           PORTCbits.RC5
#define IO_RC5_SetDigitalInput()    do { TRISCbits.TRISC5 = 1; } while(0)
#define IO_RC5_SetDigitalOutput()   do { TRISCbits.TRISC5 = 0; } while(0)
#define IO_RC5_SetPullup()          do { WPUCbits.WPUC5 = 1; } while(0)
#define IO_RC5_ResetPullup()        do { WPUCbits.WPUC5 = 0; } while(0)
#define IO_RC5_SetPushPull()        do { ODCONCbits.ODC5 = 0; } while(0)
#define IO_RC5_SetOpenDrain()       do { ODCONCbits.ODC5 = 1; } while(0)
#define IO_RC5_SetAnalogMode()      do { ANSELCbits.ANSC5 = 1; } while(0)
#define IO_RC5_SetDigitalMode()     do { ANSELCbits.ANSC5 = 0; } while(0)

/**
   @Param
    none
   @Returns
    none
   @Description
    GPIO and peripheral I/O initialization
   @Example
    PIN_MANAGER_Initialize();
 */
void PIN_MANAGER_Initialize (void);

/**
 * @Param
    none
 * @Returns
    none
 * @Description
    Interrupt on Change Handling routine
 * @Example
    PIN_MANAGER_IOC();
 */
void PIN_MANAGER_IOC(void);



#endif // PIN_MANAGER_H
/**
 End of File
*/