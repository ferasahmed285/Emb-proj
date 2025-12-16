/******************************************************************************
 * File: uart.c
 * Module: UART (Universal Asynchronous Receiver/Transmitter)
 * Description: Source file for TM4C123GH6PM UART2 Driver (TivaWare)
 * Author: Ahmedhh
 * Date: December 10, 2025
 *
 * Configuration:
 *   - UART2 (PD6: RX, PD7: TX)
 *   - Baud Rate: 115200
 *   - Data: 8 bits
 *   - Parity: None
 *   - Stop: 1 bit
 *   - System Clock: read at runtime via SysCtlClockGet()
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
#include "inc/hw_gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"
#include "driverlib/pin_map.h"

/******************************************************************************
 *                              Definitions                                    *
 ******************************************************************************/

#define BAUD_RATE 115200 /* Target baud rate */

/******************************************************************************
 *                          Function Implementations                           *
 ******************************************************************************/

/*
 * UART2_Init
 * Initializes UART2 with 115200 baud rate, 8N1 configuration using TivaWare.
 *
 * TivaWare functions used:
 *   - SysCtlPeripheralEnable(): Enable peripheral clocks
 *   - GPIOPinConfigure(): Configure pin muxing
 *   - GPIOPinTypeUART(): Configure pins for UART alternate function
 *   - UARTConfigSetExpClk(): Configure UART parameters
 *   - UARTEnable(): Enable UART module
 */
void UART2_Init(void)
{
    uint32_t systemClockHz = SysCtlClockGet();

    /* 1. Enable peripheral clocks for UART2 and GPIOD */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART2);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);

    /* Wait for peripherals to be ready */
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_UART2))
        ;
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOD))
        ;

    /* Unlock PD7 (NMI pin) - required before configuring as UART */
    HWREG(GPIO_PORTD_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
    HWREG(GPIO_PORTD_BASE + GPIO_O_CR) |= GPIO_PIN_7;
    HWREG(GPIO_PORTD_BASE + GPIO_O_LOCK) = 0;

    /* 2. Configure GPIO pins for UART functionality */
    /* PD6: U2RX (UART2 Receive) */
    /* PD7: U2TX (UART2 Transmit) */
    GPIOPinConfigure(GPIO_PD6_U2RX);
    GPIOPinConfigure(GPIO_PD7_U2TX);

    /* Set pin type to UART */
    GPIOPinTypeUART(GPIO_PORTD_BASE, GPIO_PIN_6 | GPIO_PIN_7);

    /* 3. Configure UART parameters */
    /* System clock, baud rate, 8 data bits, 1 stop bit, no parity */
    UARTConfigSetExpClk(UART2_BASE, systemClockHz, BAUD_RATE,
                        (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                         UART_CONFIG_PAR_NONE));

    /* 4. Enable UART2 */
    UARTEnable(UART2_BASE);
}

/*
 * UART1_SendChar
 * Transmits a single character through UART1 using TivaWare.
 * Uses UARTCharPut() which blocks until FIFO has space.
 */
void UART2_SendChar(char data)
{
    /* UARTCharPut() blocks until space is available in TX FIFO */
    UARTCharPut(UART2_BASE, data);
}

/*
 * UART1_ReceiveChar
 * Receives a single character from UART1 using TivaWare.
 * Uses UARTCharGet() which blocks until data is available.
 */
char UART2_ReceiveChar(void)
{
    /* UARTCharGet() blocks until data is available in RX FIFO */
    return (char)UARTCharGet(UART2_BASE);
}

/*
 * UART1_SendString
 * Transmits a null-terminated string through UART1.
 */
void UART2_SendString(const char *str)
{
    while (*str != '\0')
    {
        UART2_SendChar(*str);
        str++;
    }
}

/*
 * UART1_IsDataAvailable
 * Checks if data is available in the receive FIFO using TivaWare.
 * Uses UARTCharsAvail() to check RX FIFO status.
 */
uint8_t UART2_IsDataAvailable(void)
{
    /* UARTCharsAvail() returns true if characters are available */
    return (UARTCharsAvail(UART2_BASE)) ? 1U : 0U;
}