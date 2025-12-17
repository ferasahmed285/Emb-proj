/******************************************************************************
 * File: eeprom.c
 * Module: EEPROM Driver (TM4C123GH6PM)
 ******************************************************************************/

#include "eeprom.h"
#include <stdint.h>
#include <stdbool.h>

/* TivaWare includes */
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/eeprom.h"

void EEPROM_Init(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_EEPROM0);
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_EEPROM0))
        ;
    EEPROMInit();
}

void EEPROM_WritePassword(uint8_t *password)
{
    EEPROMProgram((uint32_t *)password, PASSWORD_ADDRESS, 8);
}

void EEPROM_ReadPassword(uint8_t *password)
{
    EEPROMRead((uint32_t *)password, PASSWORD_ADDRESS, 8);
}

void EEPROM_WriteTimeout(uint8_t timeout_seconds)
{
    uint32_t timeout_data = timeout_seconds;
    EEPROMProgram(&timeout_data, TIMEOUT_ADDRESS, 4);
}

uint8_t EEPROM_ReadTimeout(void)
{
    uint32_t timeout_data;
    EEPROMRead(&timeout_data, TIMEOUT_ADDRESS, 4);

    if (timeout_data >= 5 && timeout_data <= 30)
    {
        return (uint8_t)timeout_data;
    }
    return 10; // Default
}

bool EEPROM_IsPasswordSet(void)
{
    uint32_t setup_flag;
    EEPROMRead(&setup_flag, SETUP_FLAG_ADDRESS, 4);
    return (setup_flag == SETUP_COMPLETE);
}

void EEPROM_MarkPasswordSet(void)
{
    uint32_t setup_flag = SETUP_COMPLETE;
    EEPROMProgram(&setup_flag, SETUP_FLAG_ADDRESS, 4);
}