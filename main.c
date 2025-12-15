#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "buzzer.h"
#include "motor.h"

// int main(void)
// {
//   //
//   // Set the clocking to run at 16 MHz from the main oscillator.
//   //
//   SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_XTAL_16MHZ |
//                  SYSCTL_OSC_MAIN);

//   // Initialize buzzer (and ensure it is OFF)
//   enable_buzzer();

//   enable_motor();
//   while (1)
//   {
//     buzzer_test();
//     SysCtlDelay(5333333); // Wait 1 second between tests
//     motor_turn();
//     SysCtlDelay(5333333); // Wait 1 second before repeating
//   }
// }
// #if 0
/******************************************************************************
 * File: main.c
 * Project: UART1 Control Demo (TivaWare Version)
 * Description: TivaC receives commands via UART1 and controls buzzer and RGB LED.
 *              This version uses TivaWare peripheral driver library.
 * Author: Ahmedhh
 * Date: December 10, 2025
 *
 * Configuration:
 *   - UART17: 115200 baud, 8N1 (PE0: RX, PE1: TX)
 *   - RGB LED on Port F (PF1: Red, PF2: Blue, PF3: Green)
 *   - Buzzer on PA3
 *
 * Commands:
 *   A - Buzzer beep (1 second)
 *   B - Blue LED (1 second)
 *   C - Red LED (1 second)
 *   D - Green LED (1 second)
 ******************************************************************************/

#include "UART.h"
#include "dio.h"
#include "systick.h"
#include "buzzer.h"
#include <stdint.h>

/******************************************************************************
 *                              Definitions                                    *
 ******************************************************************************/

/* LED Pin Definitions (Port F) */
#define LED_PORT PORTF
#define RED_LED PIN1
#define BLUE_LED PIN2
#define GREEN_LED PIN3

/******************************************************************************
 *                          Function Prototypes                                *
 ******************************************************************************/

void LED_Init(void);

/******************************************************************************
 *                              Main Function                                  *
 ******************************************************************************/

int main(void)
{
  char receivedChar;

  /* Initialize peripherals */
  SysTick_Init(16000, SYSTICK_NOINT); /* Initialize system tick timer */
  UART1_Init();                       /* Initialize UART13 at 115200 baud (TivaWare) */
  LED_Init();                         /* Initialize RGB LED on Port F */
  enable_buzzer();                    /* Initialize Buzzer on PA3 */

  /* Send welcome message to PuTTY */
  UART1_SendString("\r\n*** TivaC UART1 Control Demo (TivaWare) ***\r\n");
  UART1_SendString("Commands: A=Buzzer, B=Blue, C=Red, D=Green\r\n");
  UART1_SendString("Ready to receive commands...\r\n\r\n");

  /* Main loop */
  while (1)
  {
    /* Wait for a character from UART1 */
    receivedChar = UART1_ReceiveChar();
    if (!receivedChar)
    {
      printf("Error receiving character!\r\n");
    }

    /* Echo the character back to PuTTY */
    UART1_SendChar(receivedChar);
    UART1_SendString("\r\n"); /* New line for better readability */

    /* Check for buzzer commands (a, b, c, d) */
    if (receivedChar == 'A' || receivedChar == 'a')
    {
      buzzer_test(); /* Buzzer beep for 1 second */
    }
    else if (receivedChar == 'B' || receivedChar == 'b')
    {
      /* Turn on Blue LED for 1 second */
      DIO_WritePin(LED_PORT, BLUE_LED, HIGH);
      SysCtlDelay(5333333); /* 1 second at 16MHz */
      DIO_WritePin(LED_PORT, BLUE_LED, LOW);
    }
    else if (receivedChar == 'C' || receivedChar == 'c')
    {
      /* Turn on Red LED for 1 second */
      DIO_WritePin(LED_PORT, RED_LED, HIGH);
      SysCtlDelay(5333333); /* 1 second at 16MHz */
      DIO_WritePin(LED_PORT, RED_LED, LOW);
    }
    else if (receivedChar == 'D' || receivedChar == 'd')
    {
      /* Turn on Green LED for 1 second */
      DIO_WritePin(LED_PORT, GREEN_LED, HIGH);
      SysCtlDelay(5333333); /* 1 second at 16MHz */
      DIO_WritePin(LED_PORT, GREEN_LED, LOW);
    }
  }
}

/******************************************************************************
 *                          Function Implementations                           *
 ******************************************************************************/

/*
 * LED_Init
 * Initializes the RGB LED pins on Port F as outputs.
 * Note: PF0 is locked by default, but we're only using PF1, PF2, PF3.
 */
void LED_Init(void)
{
  /* Initialize LED pins as outputs */
  DIO_Init(LED_PORT, RED_LED, OUTPUT);
  DIO_Init(LED_PORT, BLUE_LED, OUTPUT);
  DIO_Init(LED_PORT, GREEN_LED, OUTPUT);

  /* Turn off all LEDs initially */
  DIO_WritePin(LED_PORT, RED_LED, LOW);
  DIO_WritePin(LED_PORT, BLUE_LED, LOW);
  DIO_WritePin(LED_PORT, GREEN_LED, LOW);
}
