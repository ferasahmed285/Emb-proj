/******************************************************************************
 * File: main.c
 * Project: Buzzer and Motor Control via UART (TivaWare Version)
 * Description: TivaC receives commands via UART from another TivaC and
 *              controls buzzer and motor accordingly.
 * Author: Ahmedhh
 * Date: December 10, 2025
 *
 * Configuration:
 *   - UART2: 115200 baud, 8N1 (PD6: RX, PD7: TX)
 *   - Buzzer on PA3
 *   - Motor on PD0 (IN1), PD1 (IN2)
 *
 * Commands:
 *   A - Buzzer beep (1 second)
 *   B - Motor turn right
 *   C - Motor turn left
 *   M - Motor sequence (right -> delay -> left)
 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "uart.h"
#include "buzzer.h"
#include "motor.h"

/******************************************************************************
 *                              Main Function                                  *
 ******************************************************************************/

int main(void)
{
  char receivedChar;

  /* Set the clocking to run at 16 MHz from the main oscillator */
  SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_XTAL_16MHZ |
                 SYSCTL_OSC_MAIN);

  /* Initialize peripherals */
  UART1_Init();    /* Initialize UART2 at 115200 baud */
  enable_buzzer(); /* Initialize Buzzer on PA3 */
  enable_motor();  /* Initialize Motor on PD0, PD1 */

  /* Send ready message */
  UART1_SendString("\r\n*** TivaC Ready ***\r\n");

  /* Main loop */
  while (1)
  {
    /* Wait for a character from UART */
    receivedChar = UART1_ReceiveChar();

    /* Quick buzzer beep to confirm character received */
    GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, GPIO_PIN_3);
    SysCtlDelay(533333); /* 0.1 second beep */
    GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 0);

    /* Echo the character back */
    UART1_SendChar(receivedChar);
    UART1_SendString("\r\n");

    /* Process commands */
    if (receivedChar == 'A' || receivedChar == 'a')
    {
      buzzer_test(); /* Buzzer beep for 1 second */
    }
    else if (receivedChar == 'B' || receivedChar == 'b')
    {
      motor_right(); /* Turn motor right */
    }
    else if (receivedChar == 'C' || receivedChar == 'c')
    {
      motor_left(); /* Turn motor left */
    }
    else if (receivedChar == 'D' || receivedChar == 'd')
    {
      motor_turn(); /* Motor sequence: right -> delay -> left */
    }
  }
}
