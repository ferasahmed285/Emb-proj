/******************************************************************************
 * File: uart.h
 * Module: UART (Universal Asynchronous Receiver/Transmitter)
 * Description: Header file for TM4C123GH6PM UART2 Driver (TivaWare)
 * Author: Ahmedhh
 * Date: December 10, 2025
 *
 * Configuration:
 *   - UART2 (PD6: RX, PD7: TX)
 *   - Baud Rate: 115200
 *   - Data: 8 bits
 *   - Parity: None
 *   - Stop: 1 bit
 ******************************************************************************/

#ifndef UART_H_
#define UART_H_

#include <stdint.h>
#include <stdbool.h>

/*
 * UART2_Init
 * Initializes UART2 with 115200 baud rate, 8N1 configuration using TivaWare.
 * Uses PD6 (RX) and PD7 (TX).
 * System clock is read at runtime with SysCtlClockGet().
 */
void UART2_Init(void);

/*
 * UART2_SendChar
 * Transmits a single character through UART2.
 * Blocks until the transmit FIFO is ready.
 *
 * Parameters:
 *   data - Character to transmit
 */
void UART2_SendChar(char data);

/*
 * UART2_ReceiveChar
 * Receives a single character from UART2.
 * Blocks until a character is available in the receive FIFO.
 *
 * Returns:
 *   Received character
 */
char UART2_ReceiveChar(void);

/*
 * UART2_SendString
 * Transmits a null-terminated string through UART2.
 *
 * Parameters:
 *   str - Pointer to null-terminated string to transmit
 */
void UART2_SendString(const char *str);

/*
 * UART2_IsDataAvailable
 * Checks if data is available in the receive FIFO.
 *
 * Returns:
 *   1 if data is available, 0 otherwise
 */
uint8_t UART2_IsDataAvailable(void);

#endif /* UART_H_ */