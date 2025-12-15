// #include <stdint.h>
// #include <stdbool.h>
// #include "inc/hw_memmap.h"
// #include "inc/hw_types.h"
// #include "driverlib/sysctl.h"
// #include "driverlib/gpio.h"
// #include "buzzer.h"
// #include "motor.h"

// int main(void)
// {
//   //
//   // Set the clocking to run at 50 MHz from the PLL.
//   //
//   SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ |
//                  SYSCTL_OSC_MAIN);

//   // Initialize buzzer (and ensure it is OFF)
//   enable_buzzer();

//   enable_motor();
//   while (1)
//   {
//     buzzer_test();
//     SysCtlDelay(16666667);
//     motor_test();
//   }
// }
/******************************************************************************
 * File: main.c
 * Project: UART LED Color Control Demo (TivaWare Version)
 * Description: TivaC receives color commands via UART from PuTTY and controls
 *              RGB LED accordingly. Commands are echoed back and displayed on LCD.
 *              This version uses TivaWare peripheral driver library.
 * Author: Ahmedhh
 * Date: December 10, 2025
 *
 * Configuration:
 *   - UART3: 115200 baud, 8N1 (PC6: RX, PC7: TX)
 *   - RGB LED on Port F (PF1: Red, PF2: Blue, PF3: Green)
 *   - LCD on Port B
 *
 * Commands:
 *   R - Red
 *   G - Green
 *   B - Blue
 *   P - Purple (Red + Blue)
 *   Y - Yellow (Red + Green)
 *   C - Cyan (Blue + Green)
 *   W - White (Red + Blue + Green)
 *   O - Off (All LEDs off)
 ******************************************************************************/

#include "uart.h"
#include "dio.h"
#include "lcd.h"
#include "systick.h"
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
void LED_SetColor(char command);
void DisplayColorOnLCD(char command);

/******************************************************************************
 *                              Main Function                                  *
 ******************************************************************************/

int main(void)
{
  char receivedChar;

  /* Initialize peripherals */
  SysTick_Init(16000, SYSTICK_NOINT); /* Initialize system tick timer */
  UART_Init();                        /* Initialize UART3 at 115200 baud (TivaWare) */
  LED_Init();                         /* Initialize RGB LED on Port F */
  LCD_Init();                         /* Initialize LCD */

  /* Display welcome message on LCD */
  LCD_Clear();
  LCD_SetCursor(0, 0);
  LCD_WriteString("UART LED Ctrl");
  LCD_SetCursor(1, 0);
  LCD_WriteString("TivaWare Ready");

  /* Send welcome message to PuTTY */
  UART_SendString("\r\n*** TivaC UART LED Control Demo (TivaWare) ***\r\n");
  UART_SendString("Commands: R G B P Y C W O\r\n");
  UART_SendString("Ready to receive commands...\r\n\r\n");

  /* Main loop */
  while (1)
  {
    /* Wait for a character from UART */
    receivedChar = UART_ReceiveChar();

    /* Echo the character back to PuTTY */
    UART_SendChar(receivedChar);
    UART_SendString("\r\n"); /* New line for better readability */

    /* Convert to uppercase if lowercase */
    if (receivedChar >= 'a' && receivedChar <= 'z')
    {
      receivedChar = receivedChar - 32; /* Convert to uppercase */
    }

    /* Process the command */
    LED_SetColor(receivedChar);

    /* Display the color on LCD */
    DisplayColorOnLCD(receivedChar);
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

/*
 * LED_SetColor
 * Sets the LED color based on the received command.
 *
 * Parameters:
 *   command - Color command character (R, G, B, P, Y, C, W, O)
 */
void LED_SetColor(char command)
{
  /* Turn off all LEDs first */
  DIO_WritePin(LED_PORT, RED_LED, LOW);
  DIO_WritePin(LED_PORT, BLUE_LED, LOW);
  DIO_WritePin(LED_PORT, GREEN_LED, LOW);

  /* Set color based on command */
  switch (command)
  {
  case 'R': /* Red */
    DIO_WritePin(LED_PORT, RED_LED, HIGH);
    break;

  case 'G': /* Green */
    DIO_WritePin(LED_PORT, GREEN_LED, HIGH);
    break;

  case 'B': /* Blue */
    DIO_WritePin(LED_PORT, BLUE_LED, HIGH);
    break;

  case 'P': /* Purple (Red + Blue) */
    DIO_WritePin(LED_PORT, RED_LED, HIGH);
    DIO_WritePin(LED_PORT, BLUE_LED, HIGH);
    break;

  case 'Y': /* Yellow (Red + Green) */
    DIO_WritePin(LED_PORT, RED_LED, HIGH);
    DIO_WritePin(LED_PORT, GREEN_LED, HIGH);
    break;

  case 'C': /* Cyan (Blue + Green) */
    DIO_WritePin(LED_PORT, BLUE_LED, HIGH);
    DIO_WritePin(LED_PORT, GREEN_LED, HIGH);
    break;

  case 'W': /* White (All LEDs) */
    DIO_WritePin(LED_PORT, RED_LED, HIGH);
    DIO_WritePin(LED_PORT, BLUE_LED, HIGH);
    DIO_WritePin(LED_PORT, GREEN_LED, HIGH);
    break;

  case 'O': /* Off (All LEDs off) */
    /* Already turned off above */
    break;

  default:
    /* Invalid command - do nothing */
    break;
  }
}

/*
 * DisplayColorOnLCD
 * Displays the selected color name on the LCD.
 *
 * Parameters:
 *   command - Color command character (R, G, B, P, Y, C, W, O)
 */
void LED_SetStatus(char command)
{
  /* Turn off all LEDs first */
  DIO_WritePin(LED_PORT, RED_LED, LOW);
  DIO_WritePin(LED_PORT, BLUE_LED, LOW);
  DIO_WritePin(LED_PORT, GREEN_LED, LOW);

  /* Set status based on command */
  switch (command)
  {
  case 'T': /* Door opened - Green */
    DIO_WritePin(LED_PORT, GREEN_LED, HIGH);
    break;

  case 'F': /* Door closed - Red */
    DIO_WritePin(LED_PORT, RED_LED, HIGH);
    break;

  default:
    /* Invalid command - do nothing */
    break;
  }
}

/*
 * DisplayStatusOnLCD
 * Displays the door status on the LCD.
 *
 * Parameters:
 *   command - Status command character (T, F)
 */
void DisplayStatusOnLCD(char command)
{
  /* Clear the second line */
  LCD_SetCursor(1, 0);
  LCD_WriteString("                "); /* Clear line */

  /* Display status */
  LCD_SetCursor(1, 0);

  switch (command)
  {
  case 'T':
    LCD_WriteString("Door: Opened");
    break;

  case 'F':
    LCD_WriteString("Door: Closed");
    break;

  default:
    LCD_WriteString("Invalid Cmd");
    break;
  }
}