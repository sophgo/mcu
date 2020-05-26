/**
  I2C Generated Driver API Header File

  @Company
    Microchip Technology Inc.

  @File Name
    i2c_slave.h

  @Summary
    This is the generated header file for the I2C driver using PIC10 / PIC12 / PIC16 / PIC18 MCUs

  @Description
    This header file provides APIs for driver for I2C.
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.81.0
        Device            :  PIC16F1704
        Driver Version    :  1.0.0
    The generated drivers are tested against the following:
        Compiler          :  XC8 2.10 and above or later
        MPLAB             :  MPLAB X 5.35
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

#ifndef I2C_SLAVE_H
#define I2C_SLAVE_H

#include <stdbool.h>
#include <stdint.h>

typedef void (*interruptHandler)(void);
/**
 * \brief Initialize I2C interface
 * If module is configured to disabled state, the clock to the I2C is disabled
 * if this is supported by the device's clock system.
 *
 * \return None
 */
void I2C_Initialize(void);

/**
 * \brief Open the I2C for communication. Enables the module if disabled.
 *
 * \return Nothing
 */
void I2C_Open(void);

/**
 * \brief Close the I2C for communication. Disables the module if enabled.
 * Disables address recognition.
 *
 * \return Nothing
 */
void I2C_Close(void);

/**
 * \brief Read data from I2C communication. 
 *
 * \return Read Data
 */
uint8_t I2C_Read(void);

/**
 * \brief Write data over the communication. 
 *
 * \return None
 */
void I2C_Write(uint8_t data);

/**
 * \brief Enable the communication by initialization of hardware 
 *
 * \return None
 */
void I2C_Enable(void);

/**
 * \brief Send the Ack Signal to Master 
 *
 * \return None
 */
void I2C_SendAck(void);

/**
 * \brief Send the Nack Signal to Master 
 *
 * \return None
 */
void I2C_SendNack(void);

/**
 * \brief The function called by the I2C Irq handler.
 * Can be called in a polling loop in a polled driver.
 *
 * \return Nothing
 */
void I2C_SlaveSetIsrHandler(interruptHandler handler);
void I2C_SlaveSetAddrIntHandler(interruptHandler handler);
void I2C_SlaveSetReadIntHandler(interruptHandler handler);
void I2C_SlaveSetWriteIntHandler(interruptHandler handler);
void I2C_SlaveSetBusColIntHandler(interruptHandler handler);
void I2C_SlaveSetWrColIntHandler(interruptHandler handler);

void (*MSSP_InterruptHandler)(void);
void (*I2C_SlaveRdInterruptHandler)(void);
void (*I2C_SlaveWrInterruptHandler)(void);
void (*I2C_SlaveAddrInterruptHandler)(void);
void (*I2C_SlaveBusColInterruptHandler)(void);
void (*I2C_SlaveWrColInterruptHandler)(void);


#endif /* I2C_SLAVE_H */