/******************************************************************************
 * File: main.c (Control_ECU)
 * FIXED: Flush UART buffer and send response immediately
 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "systick.h"
#include "uart.h"
#include "motor.h"
#include "buzzer.h"
#include "eeprom.h"

#define CORRECT_PASSWORD "12345"
#define PASSWORD_LENGTH 5
#define BUFFER_SIZE 32

/******************************************************************************
 * Function Prototypes
 ******************************************************************************/
bool ValidatePassword(const char *received_password);
void ProcessPasswordCommand(const char *buffer);
void ExtractPassword(const char *buffer, char *password);
void FlushUARTBuffer(void); // ← ADDED

/******************************************************************************
 * Main Function
 ******************************************************************************/
int main(void)
{
    char receivedChar;
    char buffer[BUFFER_SIZE];
    uint8_t bufferIndex = 0;

    /* Set clock to 16 MHz */
    SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC |
                   SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);

    /* Initialize peripherals */
    SysTick_Init(16000, SYSTICK_NOINT);
    UART2_Init();
    EEPROM_Init();
    enable_motor();
    enable_buzzer();

    /* Clear buffer */
    memset(buffer, 0, BUFFER_SIZE);

    /* DON'T send ready message - HMI might read it! */
    /* Or wait for HMI to be ready first */
    DelayMs(100);

    /* Flush any startup garbage */
    FlushUARTBuffer(); // ← ADDED

    while (1)
    {
        /* Check for UART data */
        if (UART2_IsDataAvailable())
        {
            receivedChar = UART2_ReceiveChar();

            /* Store character in buffer */
            if (bufferIndex < (BUFFER_SIZE - 1))
            {
                buffer[bufferIndex++] = receivedChar;
                buffer[bufferIndex] = '\0';

                /* Check for end of command (newline) */
                if (receivedChar == '\n' || receivedChar == '\r')
                {
                    /* Check if this is a password command */
                    if (strstr(buffer, "PWD:") != NULL)
                    {
                        ProcessPasswordCommand(buffer);
                    }
                    /* Check for single char commands */
                    else if (buffer[0] == '0')
                    {
                        /* Buzzer command */
                        UART2_SendChar('0');
                        alarm();
                    }
                    else if (buffer[0] == '1')
                    {
                        /* Motor command */
                        UART2_SendChar('1');
                        motor_sequence();
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
        }
    }
}

/******************************************************************************
 * ProcessPasswordCommand
 * FIXED: Send response IMMEDIATELY before any action
 ******************************************************************************/
void ProcessPasswordCommand(const char *buffer)
{
    char received_password[PASSWORD_LENGTH + 1];

    /* Extract password from "PWD:12345\n" */
    ExtractPassword(buffer, received_password);

    /* Validate password */
    if (ValidatePassword(received_password))
    {
        /* ✅ PASSWORD CORRECT */

        /* Send single character response */
        UART2_SendChar('1');

        /* Wait for transmission to complete */
        DelayMs(10);

        /* Run motor sequence (open door) */
        motor_sequence();
    }
    else
    {
        /* ❌ PASSWORD WRONG */

        /* Send single character response */
        UART2_SendChar('0');

        /* Wait for transmission to complete */
        DelayMs(10);

        /* Trigger buzzer alarm */
        alarm();
    }
}

/******************************************************************************
 * ExtractPassword
 ******************************************************************************/
void ExtractPassword(const char *buffer, char *password)
{
    uint8_t i, pwd_index = 0;
    bool found_colon = false;

    for (i = 0; i < BUFFER_SIZE && buffer[i] != '\0'; i++)
    {
        if (found_colon && buffer[i] >= '0' && buffer[i] <= '9')
        {
            if (pwd_index < PASSWORD_LENGTH)
            {
                password[pwd_index++] = buffer[i];
            }
        }

        if (buffer[i] == ':')
        {
            found_colon = true;
        }
    }

    password[pwd_index] = '\0';
}

/******************************************************************************
 * ValidatePassword
 ******************************************************************************/
bool ValidatePassword(const char *received_password)
{
    uint8_t i;

    if (strlen(received_password) != PASSWORD_LENGTH)
    {
        return false;
    }

    for (i = 0; i < PASSWORD_LENGTH; i++)
    {
        if (received_password[i] != CORRECT_PASSWORD[i])
        {
            return false;
        }
    }

    return true;
}

/******************************************************************************
 * FlushUARTBuffer
 * Clears any leftover data in UART receive buffer
 ******************************************************************************/
void FlushUARTBuffer(void)
{
    /* Read and discard all pending data */
    while (UART2_IsDataAvailable())
    {
        UART2_ReceiveChar();
    }
}