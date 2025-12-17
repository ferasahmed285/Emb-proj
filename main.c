/******************************************************************************
 * File: main_receiver.c
 * Description: Main application for RECEIVER (Control_ECU)
 * Author: Updated for UART2
 * Date: December 15, 2025
 *
 * This program receives commands via UART2 from HMI_ECU and:
 * 1. Controls a DC motor (door lock/unlock)
 * 2. Controls a buzzer (alarm)
 * 3. Sends acknowledgment back
 *
 * Hardware:
 *   - UART2: PD6 (RX), PD7 (TX) from HMI_ECU
 *   - Motor: PWM controlled DC motor
 *   - Buzzer: GPIO controlled buzzer
 *   - LED: PF1 for reception indicator
 ******************************************************************************/

#include <stdint.h>
#include <string.h>
#include "tm4c123gh6pm.h"
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "systick.h"
#include "uart.h"
#include "motor.h"
#include "buzzer.h"
/* Buffer for command reception */
#define BUFFER_SIZE 32

/* LED Definitions for status indication */
#define LED_PORT GPIO_PORTF_BASE
#define RX_LED GPIO_PIN_1     /* Red: Data received */
#define MOTOR_LED GPIO_PIN_2  /* Blue: Motor active */
#define BUZZER_LED GPIO_PIN_3 /* Green: Buzzer active */

/******************************************************************************
 *                          Function Prototypes                                *
 ******************************************************************************/
void LED_Init(void);
void LED_Set(uint8_t led, bool state);
void SendAck(const char *message);

/******************************************************************************
 *                              Main Function                                  *
 ******************************************************************************/

int main(void)
{
    char receivedChar;
    char buffer[BUFFER_SIZE];
    uint8_t bufferIndex = 0;

    /* ===== CRITICAL: SET CLOCK TO 16MHz ===== */
    SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC |
                   SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);

    /* Initialize peripherals */
    SysTick_Init(16000, SYSTICK_NOINT); /* 1ms delay */
    LED_Init();
    UART2_Init();    /* UPDATED: UART2 not UART1 */
    enable_motor();  /* Initialize motor driver */
    enable_buzzer(); /* Initialize buzzer driver */

    /* Turn off motor initially */
    motor_stop();

    /* Clear buffer */
    memset(buffer, 0, BUFFER_SIZE);

    /* Send ready message to HMI_ECU */
    UART2_SendString("Control_ECU Ready (UART2)\r\n");

    /* Initial LED blink */
    LED_Set(RX_LED, true);
    DelayMs(500);
    LED_Set(RX_LED, false);

    while (1)
    {
        /* Check if data is available from UART2 */
        if (UART2_IsDataAvailable())
        {
            /* Visual feedback - blink RX LED */
            LED_Set(RX_LED, true);

            /* Read character */
            receivedChar = UART2_ReceiveChar();

            /* Store in buffer for command parsing */
            if (bufferIndex < (BUFFER_SIZE - 1))
            {
                buffer[bufferIndex++] = receivedChar;
                buffer[bufferIndex] = '\0'; /* Keep null-terminated */

                /* Check for end of line/command */
                if (receivedChar == '\n' || receivedChar == '\r')
                {
                    /* Process complete command */
                    if (strstr(buffer, "CMD:BUZZER_ON") != NULL)
                    {
                        /* Trigger buzzer alarm */
                        LED_Set(BUZZER_LED, true);
                        alarm();
                        LED_Set(BUZZER_LED, false);
                        SendAck("BUZZER ALARM");
                    }
                    else if (strstr(buffer, "CMD:DOOR_OPEN") != NULL)
                    {
                        /* Open door (motor forward) */
                        motor_forward();
                        LED_Set(MOTOR_LED, true);
                        SendAck("DOOR OPENING");

                        /* Wait and close (simulate timeout) */
                        DelayMs(5000); /* 5 seconds open */
                        motor_stop();
                        LED_Set(MOTOR_LED, false);
                        SendAck("DOOR CLOSED");
                    }
                    else if (strstr(buffer, "CMD:DOOR_CLOSE") != NULL)
                    {
                        /* Close door immediately */
                        motor_stop();
                        LED_Set(MOTOR_LED, false);
                        SendAck("DOOR CLOSED");
                    }
                    else if (strstr(buffer, "CMD:TIMEOUT_5") != NULL)
                    {
                        SendAck("TIMEOUT 5s SET");
                    }
                    else if (strstr(buffer, "CMD:TIMEOUT_10") != NULL)
                    {
                        SendAck("TIMEOUT 10s SET");
                    }

                    /* Reset buffer */
                    bufferIndex = 0;
                    memset(buffer, 0, BUFFER_SIZE);
                }
            }
            else
            {
                /* Buffer overflow - reset */
                bufferIndex = 0;
                memset(buffer, 0, BUFFER_SIZE);
            }

            /* Also process single character commands immediately */
            if (receivedChar == '0')
            {
                /* Buzzer alarm (3 beeps in 1 second) */
                LED_Set(BUZZER_LED, true);
                alarm();
                LED_Set(BUZZER_LED, false);
                SendAck("BUZZER ALARM (char)");
            }
            else if (receivedChar == '1')
            {
                /* Motor turn right */
                LED_Set(MOTOR_LED, true);
                motor_turn();
                SendAck("MOTOR RIGHT (char)");
            }

            /* Turn off RX LED after processing */
            DelayMs(50);
            LED_Set(RX_LED, false);
        }

        /* Small delay to reduce CPU usage */
        DelayMs(10);
    }
}

/******************************************************************************
 *                          Function Implementations                           *
 ******************************************************************************/

/*
 * LED_Init
 * Initializes LEDs on Port F for status indication.
 */
void LED_Init(void)
{
    /* Enable GPIOF peripheral */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF))
        ;

    /* Configure LED pins as outputs */
    GPIOPinTypeGPIOOutput(LED_PORT, RX_LED | MOTOR_LED | BUZZER_LED);

    /* Turn off all LEDs initially */
    GPIOPinWrite(LED_PORT, RX_LED | MOTOR_LED | BUZZER_LED, 0);
}

/*
 * LED_Set
 * Sets the state of a specific LED.
 */
void LED_Set(uint8_t led, bool state)
{
    if (state)
        GPIOPinWrite(LED_PORT, led, led);
    else
        GPIOPinWrite(LED_PORT, led, 0);
}

/*
 * SendAck
 * Sends an acknowledgment message back to HMI_ECU.
 */
void SendAck(const char *message)
{
    UART2_SendString("ACK: ");
    UART2_SendString(message);
    UART2_SendString("\r\n");
}