/******************************************************************************
 * File: uart.c
 * Module: UART (Universal Asynchronous Receiver/Transmitter)
 * Description: Source file for TM4C123GH6PM UART2 Driver
 * Author: Updated for UART2
 * Date: December 15, 2025
 * 
 * Configuration:
 *   - UART2 (PD6: RX, PD7: TX)
 *   - Baud Rate: 115200
 *   - Data: 8 bits
 *   - Parity: None
 *   - Stop: 1 bit
 *   - PD7 requires unlocking (NMI pin)
 ******************************************************************************/

#include "uart.h"
#include <stdint.h>
#include <stdbool.h>

/* TivaWare includes */
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"      // ADDED for GPIO unlocking
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"
#include "driverlib/pin_map.h"

/* Desired baud rate */
#define UART2_BAUD_RATE   115200U

/*
 * UART2_Init
 * Initializes UART2 on PD6 (RX) and PD7 (TX).
 * UNLOCKS PD7 which is locked by default (NMI pin).
 */
void UART2_Init(void)
{
    uint32_t sysClock;

    /* 1) Get current system clock */
    sysClock = SysCtlClockGet();

    /* 2) Enable UART2 and Port D peripherals */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART2);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);

    /* Wait for peripherals to be ready */
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_UART2));
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOD));

    /* 3) CRITICAL: UNLOCK PD7 (NMI pin) before configuring */
    /* Unlock sequence for PD7 */
    HWREG(GPIO_PORTD_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;   /* 0x4C4F434B */
    HWREG(GPIO_PORTD_BASE + GPIO_O_CR) |= GPIO_PIN_7;       /* Allow PD7 changes */
    HWREG(GPIO_PORTD_BASE + GPIO_O_LOCK) = 0;               /* Lock again */

    /* 4) Configure PD6 = U2RX, PD7 = U2TX */
    GPIOPinConfigure(GPIO_PD6_U2RX);
    GPIOPinConfigure(GPIO_PD7_U2TX);
    GPIOPinTypeUART(GPIO_PORTD_BASE, GPIO_PIN_6 | GPIO_PIN_7);

    /* 5) Configure UART2: 115200 baud, 8N1 */
    UARTConfigSetExpClk(UART2_BASE,
                        sysClock,
                        UART2_BAUD_RATE,
                        (UART_CONFIG_WLEN_8 |
                         UART_CONFIG_STOP_ONE |
                         UART_CONFIG_PAR_NONE));

    /* 6) Enable UART2 module */
    UARTEnable(UART2_BASE);
}

/*
 * UART2_SendChar
 * Sends a single character (blocking).
 */
void UART2_SendChar(char data)
{
    UARTCharPut(UART2_BASE, data);  /* UART2_BASE not UART1_BASE */
}

/*
 * UART2_ReceiveChar
 * Receives a single character (blocking).
 */
char UART2_ReceiveChar(void)
{
    return (char)UARTCharGet(UART2_BASE);  /* UART2_BASE not UART1_BASE */
}

/*
 * UART2_SendString
 * Sends a null-terminated string over UART2.
 */
void UART2_SendString(const char *str)
{
    if (str == 0)
    {
        return;
    }

    while (*str != '\0')
    {
        UART2_SendChar(*str);
        str++;
    }
}

/*
 * UART2_IsDataAvailable
 * Checks if data is available in the RX FIFO.
 */
uint8_t UART2_IsDataAvailable(void)
{
    /* UARTCharsAvail() returns true if characters are available */
    return (UARTCharsAvail(UART2_BASE)) ? 1u : 0u;  /* UART2_BASE */
}