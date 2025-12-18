/******************************************************************************
 * File: main.c (Control_ECU)
 * Description: Logic for PWD, CHK, SET, ALM, TMO
 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h> // For atoi
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "systick.h"
#include "uart.h"
#include "motor.h"
#include "buzzer.h"
#include "eeprom.h"

#define PASSWORD_LENGTH 5
#define BUFFER_SIZE 32

/******************************************************************************
 * Function Prototypes
 ******************************************************************************/
bool ValidatePassword(const char *received_password);
void SavePassword(const char *received_password);
void ProcessCommand(const char *buffer);
void ExtractData(const char *buffer, char *data);
void FlushUARTBuffer(void);

/******************************************************************************
 * Main Function
 ******************************************************************************/
int main(void)
{
    char receivedChar;
    char buffer[BUFFER_SIZE];
    uint8_t bufferIndex = 0;

    SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC |
                   SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);

    SysTick_Init(16000, SYSTICK_NOINT);
    UART2_Init();
    EEPROM_Init();
    enable_motor();
    enable_buzzer();

    memset(buffer, 0, BUFFER_SIZE);
    FlushUARTBuffer();

    while (1)
    {
        if (UART2_IsDataAvailable())
        {
            receivedChar = UART2_ReceiveChar();
            if (bufferIndex < (BUFFER_SIZE - 1))
            {
                buffer[bufferIndex++] = receivedChar;
                buffer[bufferIndex] = '\0';
                if (receivedChar == '\n' || receivedChar == '\r')
                {
                    ProcessCommand(buffer);
                    bufferIndex = 0;
                    memset(buffer, 0, BUFFER_SIZE);
                }
            }
            else
            {
                bufferIndex = 0;
                memset(buffer, 0, BUFFER_SIZE);
            }
        }
    }
}

/******************************************************************************
 * ProcessCommand
 * Protocol:
 * "STS"        -> '1' (Exists) / '0' (Empty)
 * "SET:xxxxx"  -> Save Pass
 * "CHK:xxxxx"  -> Verify Only
 * "PWD:xxxxx"  -> Verify + Motor (No Alarm on fail)
 * "ALM"        -> Trigger Buzzer
 * "TMO:xx"     -> Save Timeout
 ******************************************************************************/
void ProcessCommand(const char *buffer)
{
    char extracted_data[BUFFER_SIZE];
    ExtractData(buffer, extracted_data);

    /* STS: Status */
    if (strstr(buffer, "STS") != NULL)
    {
        if (EEPROM_IsPasswordSet())
            UART2_SendChar('1');
        else
            UART2_SendChar('0');
    }
    /* SET: Save Password */
    else if (strstr(buffer, "SET:") != NULL)
    {
        SavePassword(extracted_data);
        UART2_SendChar('1');
    }
    /* CHK: Verify Only */
    else if (strstr(buffer, "CHK:") != NULL)
    {
        if (ValidatePassword(extracted_data))
            UART2_SendChar('1');
        else
            UART2_SendChar('0');
    }
    /* PWD: Open Door (Wait for motor logic) */
    else if (strstr(buffer, "PWD:") != NULL)
    {
        if (ValidatePassword(extracted_data))
        {
            UART2_SendChar('1'); // Send ACK first
            DelayMs(50);
            motor_sequence(); // Run motor with stored timeout
        }
        else
        {
            UART2_SendChar('0'); // Just return Fail, HMI handles retry/alarm
        }
    }
    /* ALM: Alarm (Triggered by HMI) */
    else if (strstr(buffer, "ALM") != NULL)
    {
        alarm(); // Buzzer beep 3 times
    }
    /* TMO: Set Timeout */
    else if (strstr(buffer, "TMO:") != NULL)
    {
        // Convert extracted data (e.g. "25") to integer
        int t = atoi(extracted_data);
        if (t >= 5 && t <= 30)
        {
            EEPROM_WriteTimeout((uint8_t)t);
            UART2_SendChar('1');
        }
        else
        {
            UART2_SendChar('0');
        }
    }
}

/******************************************************************************
 * Helper Functions
 ******************************************************************************/
void ExtractData(const char *buffer, char *data)
{
    uint8_t i, data_index = 0;
    bool found_colon = false;
    for (i = 0; i < BUFFER_SIZE && buffer[i] != '\0'; i++)
    {
        if (found_colon)
        {
            // Allow Digits for Password/Timeout
            if ((buffer[i] >= '0' && buffer[i] <= '9'))
            {
                data[data_index++] = buffer[i];
            }
        }
        if (buffer[i] == ':')
            found_colon = true;
    }
    data[data_index] = '\0';
}

void SavePassword(const char *received_password)
{
    uint8_t pwd_bytes[8];
    uint8_t i;
    for (i = 0; i < PASSWORD_LENGTH; i++)
        pwd_bytes[i] = received_password[i];
    EEPROM_WritePassword(pwd_bytes);
    EEPROM_MarkPasswordSet();
}

bool ValidatePassword(const char *received_password)
{
    uint8_t stored_password[8];
    uint8_t i;
    if (strlen(received_password) != PASSWORD_LENGTH)
        return false;
    EEPROM_ReadPassword(stored_password);
    for (i = 0; i < PASSWORD_LENGTH; i++)
    {
        if (received_password[i] != stored_password[i])
            return false;
    }
    return true;
}

void FlushUARTBuffer(void)
{
    while (UART2_IsDataAvailable())
        UART2_ReceiveChar();
}