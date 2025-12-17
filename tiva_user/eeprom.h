/******************************************************************************
 * File: eeprom.h
 * Module: EEPROM Driver (TM4C123GH6PM)
 * Description: Driver for internal EEPROM password and configuration storage
 * Date: December 17, 2025
 ******************************************************************************/

#ifndef EEPROM_H_
#define EEPROM_H_

#include <stdint.h>
#include <stdbool.h>

/******************************************************************************
 * Definitions
 ******************************************************************************/
#define PASSWORD_LENGTH     5       // 5-digit password
#define PASSWORD_ADDRESS    0x0000  // EEPROM address for password
#define TIMEOUT_ADDRESS     0x0010  // EEPROM address for timeout
#define SETUP_FLAG_ADDRESS  0x0020  // EEPROM address for setup flag
#define SETUP_COMPLETE      0xAA    // Flag indicating password is set

/******************************************************************************
 * Function Prototypes
 ******************************************************************************/

void EEPROM_Init(void);
void EEPROM_WritePassword(uint8_t* password);
void EEPROM_ReadPassword(uint8_t* password);
void EEPROM_WriteTimeout(uint8_t timeout_seconds);
uint8_t EEPROM_ReadTimeout(void);
bool EEPROM_IsPasswordSet(void);
void EEPROM_MarkPasswordSet(void);

#endif /* EEPROM_H_ */