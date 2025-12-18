/******************************************************************************
 * File: eeprom.h
 * Description: EEPROM Setup - Changed Flag to force New Password Setup
 ******************************************************************************/

#ifndef EEPROM_H_
#define EEPROM_H_

#include <stdint.h>
#include <stdbool.h>

/******************************************************************************
 * Definitions
 ******************************************************************************/
#define PASSWORD_LENGTH 5
#define PASSWORD_ADDRESS 0x0000
#define TIMEOUT_ADDRESS 0x0010
#define SETUP_FLAG_ADDRESS 0x0020

/*
 * CHANGED VALUE: 0x55
 * Changing this value forces the system to ignore old passwords
 * stored in EEPROM and treat the board as "New".
 */
#define SETUP_COMPLETE 0x55

/******************************************************************************
 * Function Prototypes
 ******************************************************************************/

void EEPROM_Init(void);
void EEPROM_WritePassword(uint8_t *password);
void EEPROM_ReadPassword(uint8_t *password);
void EEPROM_WriteTimeout(uint8_t timeout_seconds);
uint8_t EEPROM_ReadTimeout(void);
bool EEPROM_IsPasswordSet(void);
void EEPROM_MarkPasswordSet(void);

#endif /* EEPROM_H_ */