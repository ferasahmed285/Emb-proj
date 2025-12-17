/******************************************************************************
 * File: main.c
 * Description: Simple UART command receiver
 *
 * Hardware:
 *   - UART2: PD6 (RX), PD7 (TX)
 *   - Motor: PD0 (IN1), PD1 (IN2)
 *   - Buzzer: PA3
 *
 * Commands:
 *   '0' - Buzzer alarm (3 beeps)
 *   '1' - Motor turn right then delay then turn left
 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "uart.h"
#include "motor.h"
#include "buzzer.h"

/******************************************************************************
 *                              Main Function                                  *
 ******************************************************************************/

int main(void)
{
    char receivedChar;

    /* Set clock to 16 MHz */
    SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC |
                   SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);

    /* Initialize peripherals */
    UART2_Init();
    enable_motor();
    enable_buzzer();

    /* Send ready message */
    UART2_SendString("Ready\r\n");

    while (1)
    {
        /* Check if data is available from UART2 */
        if (UART2_IsDataAvailable())
        {
            /* Read character */
            receivedChar = UART2_ReceiveChar();

            /* Process single character commands */
            if (receivedChar == '0')
            {
                /* Buzzer alarm (3 beeps) */
                alarm();
            }
            else if (receivedChar == '1')
            {
                /* Motor sequence: right -> wait (UART delay) -> left */
                motor_sequence();
            }
        }
    }
}