/******************************************************************************
 * File: main_sender.c (HMI_ECU)
 * Description: Full System with 3-Attempt Lockout for Open & Change Password
 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "adc.h"
#include "keypad.h"
#include "lcd.h"
#include "led.h"
#include "systick.h"
#include "uart.h"

#define PASSWORD_LENGTH 5

/******************************************************************************
 * Function Prototypes
 ******************************************************************************/
void SystemLoginSequence(void);
void MainMenu(void);
void OpenDoorSequence(void);
void ChangePasswordSequence(void);
void SetTimeoutSequence(void);
void CollectPassword(char *password);
void SendCommandToControl(const char *prefix, const char *data);
char CheckSystemStatus(void);
char WaitForResponse(void);
void LED_Init(void);
/******************************************************************************
 * Main Function
 ******************************************************************************/
int main(void)
{
    /* Initialize system */
    SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC |
                   SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);

    SysTick_Init(16000, SYSTICK_NOINT);
    UART2_Init();
    LCD_Init();
    Keypad_Init();
    ADC_Init(); // Initialize Potentiometer
    LED_Init(); // Initialize LED

    /* Startup Message */
    LCD_Clear();
    LCD_SetCursor(0, 0);
    LCD_WriteString("Door Lock System");
    DelayMs(1000);

    /* Step 1: Login/Setup */
    SystemLoginSequence();

    /* Step 2: Main Menu */
    while (1)
    {
        MainMenu();
    }
}

/******************************************************************************
 * Step 1: Login/Setup
 ******************************************************************************/
void SystemLoginSequence(void)
{
    char pass1[PASSWORD_LENGTH + 1];
    char pass2[PASSWORD_LENGTH + 1];
    char status, response;
    bool access_granted = false;
    uint8_t i;

    while (!access_granted)
    {
        for (i = 0; i < PASSWORD_LENGTH + 1; i++)
        {
            pass1[i] = 0;
            pass2[i] = 0;
        }

        LCD_Clear();
        LCD_SetCursor(0, 0);
        LCD_WriteString("Enter Password:");
        LCD_SetCursor(1, 0);
        CollectPassword(pass1);

        LCD_Clear();
        LCD_WriteString("Processing...");

        status = CheckSystemStatus();

        if (status == '1')
        {
            /* Login Mode */
            SendCommandToControl("CHK:", pass1);
            response = WaitForResponse();
            if (response == '1')
            {
                LED_On(LED_GREEN);
                LCD_Clear();
                LCD_WriteString("Welcome Back!");
                DelayMs(1000);
                access_granted = true;
                LED_AllOff();
            }
            else
            {
                LED_On(LED_RED);
                LCD_Clear();
                LCD_WriteString("Wrong Password");
                DelayMs(2000);
                LED_AllOff();
            }
        }
        else
        {
            /* Setup Mode */
            LCD_Clear();
            LCD_WriteString("Re-enter to Set:");
            LCD_SetCursor(1, 0);
            CollectPassword(pass2);
            if (strcmp(pass1, pass2) == 0)
            {
                SendCommandToControl("SET:", pass1);
                if (WaitForResponse() == '1')
                {
                    LED_On(LED_GREEN);
                    LCD_Clear();
                    LCD_WriteString("Setup Complete!");
                    DelayMs(1000);
                    access_granted = true;
                    LED_AllOff();
                }
                else
                {
                    LED_On(LED_RED);
                    LCD_Clear();
                    LCD_WriteString("Setup Failed!");
                    DelayMs(1000);
                    LED_AllOff();
                }
            }
            else
            {
                LED_On(LED_RED);
                LCD_Clear();
                LCD_WriteString("Mismatch!");
                DelayMs(2000);
                LED_AllOff();
            }
        }
    }
}

/******************************************************************************
 * Step 2: Main Menu
 ******************************************************************************/
void MainMenu(void)
{
    char key = 0;
    LCD_Clear();
    LCD_SetCursor(0, 0);
    LCD_WriteString("A:Open B:ChgPass");
    LCD_SetCursor(1, 0);
    LCD_WriteString("*:Set Timeout");

    while (1)
    {
        key = Keypad_GetKey();
        if (key == 'A')
        {
            OpenDoorSequence();
            break;
        }
        else if (key == 'B')
        {
            ChangePasswordSequence();
            break;
        }
        else if (key == '*')
        {
            SetTimeoutSequence();
            break;
        }
        DelayMs(50);
    }
}

/******************************************************************************
 * Step 3: Open Door (3 Attempts Logic)
 ******************************************************************************/
void OpenDoorSequence(void)
{
    char password[PASSWORD_LENGTH + 1];
    char response;
    uint8_t i, attempts = 0;

    while (attempts < 3)
    {
        for (i = 0; i < PASSWORD_LENGTH + 1; i++)
            password[i] = 0;

        LCD_Clear();
        LCD_SetCursor(0, 0);
        LCD_WriteString("Enter Password:");
        LCD_SetCursor(1, 0);
        CollectPassword(password);

        LCD_Clear();
        LCD_WriteString("Verifying...");

        SendCommandToControl("PWD:", password);
        response = WaitForResponse();

        if (response == '1')
        {
            LED_On(LED_GREEN); // Turn on green LED
            LCD_Clear();
            LCD_SetCursor(0, 0);
            LCD_WriteString("Access Granted");
            LCD_SetCursor(1, 0);
            LCD_WriteString("Door Unlocking");
            DelayMs(3000);
            LED_AllOff(); // Turn off green LED
            return;
        }
        else
        {
            LED_On(LED_RED);
            attempts++;
            if (attempts < 3)
            {
                LCD_Clear();
                LCD_SetCursor(0, 0);
                LCD_WriteString("Wrong Password");
                LCD_SetCursor(1, 0);
                LCD_WriteString("Try Again");
                DelayMs(1500);
                LED_AllOff();
            }
            else
            {
                SendCommandToControl("ALM", "");
                LCD_Clear();
                LCD_SetCursor(0, 0);
                LCD_WriteString("System Locked!");
                LCD_SetCursor(1, 0);
                LCD_WriteString("Wait 20s...");
                for (i = 0; i < 20; i++)
                    DelayMs(1000);
                LED_AllOff();
            }
        }
    }
}

/******************************************************************************
 * Step 4: Change Password (Updated: 3 Attempts for Old Password)
 ******************************************************************************/
void ChangePasswordSequence(void)
{
    char old_pass[PASSWORD_LENGTH + 1];
    char new_pass1[PASSWORD_LENGTH + 1];
    char new_pass2[PASSWORD_LENGTH + 1];
    uint8_t attempts = 0;
    uint8_t i;
    bool old_pass_correct = false;

    // --- Part 1: Verify Old Password (Max 3 Attempts) ---
    while (attempts < 3)
    {
        // Clear buffer
        for (i = 0; i < PASSWORD_LENGTH + 1; i++)
            old_pass[i] = 0;

        LCD_Clear();
        LCD_SetCursor(0, 0);
        LCD_WriteString("Enter Old Pass:");
        LCD_SetCursor(1, 0);
        CollectPassword(old_pass);

        LCD_Clear();
        LCD_WriteString("Checking...");

        SendCommandToControl("CHK:", old_pass);

        if (WaitForResponse() == '1')
        {
            LED_On(LED_GREEN);
            old_pass_correct = true;
            LED_AllOff();
            break; // Proceed to Step 2
        }
        else
        {
            LED_On(LED_RED);
            attempts++;
            if (attempts < 3)
            {
                LCD_Clear();
                LCD_WriteString("Wrong Old Pass");
                DelayMs(1500);
                LED_AllOff();
            }
            else
            {
                // 3rd Failure: Alarm + Lockout
                SendCommandToControl("ALM", "");
                LCD_Clear();
                LCD_SetCursor(0, 0);
                LCD_WriteString("System Locked!");
                LCD_SetCursor(1, 0);
                LCD_WriteString("Wait 20s...");
                for (i = 0; i < 20; i++)
                    DelayMs(1000);
                LED_AllOff();
                return; // Return to Main Menu
            }
        }
    }

    // --- Part 2: Set New Password (If old was correct) ---
    if (old_pass_correct)
    {
        bool new_pass_set = false;
        while (!new_pass_set)
        {
            for (i = 0; i < PASSWORD_LENGTH + 1; i++)
            {
                new_pass1[i] = 0;
                new_pass2[i] = 0;
            }

            LCD_Clear();
            LCD_SetCursor(0, 0);
            LCD_WriteString("Enter New Pass:");
            LCD_SetCursor(1, 0);
            CollectPassword(new_pass1);

            LCD_Clear();
            LCD_SetCursor(0, 0);
            LCD_WriteString("Confirm New:");
            LCD_SetCursor(1, 0);
            CollectPassword(new_pass2);

            if (strcmp(new_pass1, new_pass2) == 0)
            {
                LCD_Clear();
                LCD_WriteString("Saving...");
                SendCommandToControl("SET:", new_pass1);

                if (WaitForResponse() == '1')
                {
                    LED_On(LED_GREEN);
                    LCD_Clear();
                    LCD_WriteString("Pass Changed!");
                    DelayMs(2000);
                    new_pass_set = true;
                    LED_AllOff();
                }
                else
                {
                    LED_On(LED_RED);
                    LCD_Clear();
                    LCD_WriteString("Save Error!");
                    DelayMs(2000);
                    LED_AllOff();
                }
            }
            else
            {
                LED_On(LED_RED);
                LCD_Clear();
                LCD_WriteString("Mismatch!");
                DelayMs(2000);
                LED_AllOff();
                // Loop repeats to ask for new password again
            }
        }
    }
}

/******************************************************************************
 * Step 5: Set Timeout (Potentiometer)
 ******************************************************************************/
void SetTimeoutSequence(void)
{
    char str_buffer[16];
    char password[PASSWORD_LENGTH + 1];
    uint32_t adc_val;
    uint8_t timeout_val;
    char key;
    bool confirmed = false;

    // 1. Live Adjust Loop
    LCD_Clear();
    LCD_SetCursor(0, 0);
    LCD_WriteString("Adjust Timeout:");

    while (!confirmed)
    {
        adc_val = ADC_Read();
        // Map 0-4095 to 5-30 seconds
        timeout_val = 5 + (adc_val * 25) / 4095;

        sprintf(str_buffer, "%d Seconds   ", timeout_val);
        LCD_SetCursor(1, 0);
        LCD_WriteString(str_buffer);

        key = Keypad_GetKey();
        if (key == '#')
            confirmed = true;

        DelayMs(100);
    }

    // 2. Security Check
    LCD_Clear();
    LCD_WriteString("Confirm w/ Pass:");
    DelayMs(1000);

    LCD_Clear();
    LCD_WriteString("Enter Password:");
    LCD_SetCursor(1, 0);

    for (uint8_t i = 0; i < PASSWORD_LENGTH + 1; i++)
        password[i] = 0;
    CollectPassword(password);

    SendCommandToControl("CHK:", password);
    if (WaitForResponse() == '1')
    {
        sprintf(str_buffer, "TMO:%d", timeout_val);
        char *ptr = str_buffer;
        while (*ptr)
            UART2_SendChar(*ptr++);
        UART2_SendChar('\n');

        if (WaitForResponse() == '1')
        {
            LED_On(LED_GREEN);
            LCD_Clear();
            LCD_WriteString("Timeout Saved!");
        }
        else
        {
            LED_On(LED_RED);
            LCD_Clear();
            LCD_WriteString("Save Error!");
        }
        DelayMs(1500);
        LED_AllOff();
    }
    else
    {
        LED_On(LED_RED);
        LCD_Clear();
        LCD_WriteString("Wrong Password");
        DelayMs(1500);
        LED_AllOff();
    }
}

/******************************************************************************
 * Helper Functions & Drivers
 ******************************************************************************/
/* ADC_Init and ADC_Read moved to adc.c */

void CollectPassword(char *password)
{
    char key;
    uint8_t count = 0;
    while (count < PASSWORD_LENGTH)
    {
        key = Keypad_GetKey();
        if (key != 0)
        {
            if (key == '#')
            {
                count = 0;
                LCD_SetCursor(1, 0);
                LCD_WriteString("                ");
                LCD_SetCursor(1, 0);
                for (uint8_t j = 0; j < PASSWORD_LENGTH; j++)
                    password[j] = 0;
            }
            else if (key >= '0' && key <= '9')
            {
                password[count] = key;
                LCD_SetCursor(1, count);
                LCD_WriteChar('*');
                count++;
            }
            DelayMs(300);
        }
        DelayMs(50);
    }
}

void SendCommandToControl(const char *prefix, const char *data)
{
    while (*prefix)
        UART2_SendChar(*prefix++);
    while (*data)
        UART2_SendChar(*data++);
    UART2_SendChar('\n');
}

char CheckSystemStatus(void)
{
    uint8_t r = 0;
    while (r < 5)
    {
        while (UART2_IsDataAvailable())
            UART2_ReceiveChar();
        UART2_SendChar('S');
        UART2_SendChar('T');
        UART2_SendChar('S');
        UART2_SendChar('\n');
        DelayMs(100);
        if (UART2_IsDataAvailable())
            return UART2_ReceiveChar();
        DelayMs(200);
        r++;
    }
    return '0';
}

char WaitForResponse(void)
{
    uint16_t t = 0;
    while (t < 5000)
    {
        if (UART2_IsDataAvailable())
        {
            char c = UART2_ReceiveChar();
            if (c == '0' || c == '1')
                return c;
        }
        DelayMs(1);
        t++;
    }
    return 'X';
}

/******************************************************************************
 * LED Functions (Green LED on PF3, Red LED on PF1)
 ******************************************************************************/
/* LED_Init moved to led.c */

/* LED_AllOff moved to led.c */

/* LED_PinMask moved to led.c */

/* LED_On moved to led.c */