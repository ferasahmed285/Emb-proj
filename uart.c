/******************************************************************************
 * File: uart.c
 * Module: UART (Universal Asynchronous Receiver/Transmitter)
 * Description: Source file for TM4C123GH6PM UART0 Driver (TivaWare)
 * Author: Ahmedhh
 * Date: December 10, 2025
 *
 * Configuration:
 *   - UART3 (PC6: RX, PC7: TX)
 *   - Baud Rate: 115200
 *   - Data: 8 bits
 *   - Parity: None
 *   - Stop: 1 bit
 *   - System Clock: 16 MHz
 *
 * Note: This implementation uses TivaWare peripheral driver library.
 *       TivaWare functions simplify UART configuration and provide
 *       higher-level abstractions compared to direct register access.
 ******************************************************************************/

#include "uart.h"
#include <stdint.h>
#include <stdbool.h>

/* TivaWare includes */
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"
#include "driverlib/pin_map.h"

/******************************************************************************
 *                              Definitions                                    *
 ******************************************************************************/

#define SYSTEM_CLOCK 16000000 /* 16 MHz system clock */
#define BAUD_RATE 115200      /* Target baud rate */

/******************************************************************************
 *                          Function Implementations                           *
 ******************************************************************************/

/*
 * UART_Init
 * Initializes UART3 with 115200 baud rate, 8N1 configuration using TivaWare.
 *
 * TivaWare functions used:
 *   - SysCtlPeripheralEnable(): Enable peripheral clocks
 *   - GPIOPinConfigure(): Configure pin muxing
 *   - GPIOPinTypeUART(): Configure pins for UART alternate function
 *   - UARTConfigSetExpClk(): Configure UART parameters
 *   - UARTEnable(): Enable UART module
 */
void UART_Init(void)
{
    /* 1. Enable peripheral clocks for UART3 and GPIOC */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART3);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);

    /* Wait for peripherals to be ready */
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_UART3))
        ;
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOC))
        ;

    /* 2. Configure GPIO pins for UART functionality */
    /* PC6: U3RX (UART3 Receive) */
    /* PC7: U3TX (UART3 Transmit) */
    GPIOPinConfigure(GPIO_PC6_U3RX);
    GPIOPinConfigure(GPIO_PC7_U3TX);

    /* Set pin type to UART */
    GPIOPinTypeUART(GPIO_PORTC_BASE, GPIO_PIN_6 | GPIO_PIN_7);

    /* 3. Configure UART parameters */
    /* System clock, baud rate, 8 data bits, 1 stop bit, no parity */
    UARTConfigSetExpClk(UART3_BASE, SYSTEM_CLOCK, BAUD_RATE,
                        (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                         UART_CONFIG_PAR_NONE));

    /* 4. Enable UART3 */
    UARTEnable(UART3_BASE);
}

/*
 * UART_SendChar
 * Transmits a single character through UART3 using TivaWare.
 * Uses UARTCharPut() which blocks until FIFO has space.
 */
void UART_SendChar(char data)
{
    /* UARTCharPut() blocks until space is available in TX FIFO */
    UARTCharPut(UART3_BASE, data);
}

/*
 * UART_ReceiveChar
 * Receives a single character from UART3 using TivaWare.
 * Uses UARTCharGet() which blocks until data is available.
 */
char UART_ReceiveChar(void)
{
    /* UARTCharGet() blocks until data is available in RX FIFO */
    return (char)UARTCharGet(UART3_BASE);
}

/*
 * UART_SendString
 * Transmits a null-terminated string through UART3.
 */
void UART_SendString(const char *str)
{
    while (*str != '\0')
    {
        UART_SendChar(*str);
        str++;
    }
}

/*
 * UART_IsDataAvailable
 * Checks if data is available in the receive FIFO using TivaWare.
 * Uses UARTCharsAvail() to check RX FIFO status.
 */
uint8_t UART_IsDataAvailable(void)
{
    /* UARTCharsAvail() returns true if characters are available */
    return (UARTCharsAvail(UART3_BASE)) ? 1 : 0;
}
