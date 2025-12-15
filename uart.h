/******************************************************************************
 * File: uart.h
 * Module: UART (Universal Asynchronous Receiver/Transmitter)
 * Description: Header file for TM4C123GH6PM UART0 Driver (TivaWare)
 * Author: Ahmedhh
 * Date: December 10, 2025
 *
 * Configuration:
 *   - UART3 (PC6: RX, PC7: TX)
 *   - Baud Rate: 115200
 *   - Data: 8 bits
 *   - Parity: None
 *   - Stop: 1 bit
 ******************************************************************************/

#ifndef UART_H_
#define UART_H_

#include <stdint.h>
#include <stdbool.h>

/******************************************************************************
 *                          Function Prototypes                                *
 ******************************************************************************/

/*
 * UART_Init
 * Initializes UART3 with 115200 baud rate, 8N1 configuration using TivaWare.
 * Uses PC6 (RX) and PC7 (TX).
 * System clock is assumed to be 16 MHz.
 */
void UART_Init(void);

/*
 * UART_SendChar
 * Transmits a single character through UART3.
 * Blocks until the transmit FIFO is ready.
 *
 * Parameters:
 *   data - Character to transmit
 */
void UART_SendChar(char data);

/*
 * UART_ReceiveChar
 * Receives a single character from UART3.
 * Blocks until a character is available in the receive FIFO.
 *
 * Returns:
 *   Received character
 */
char UART_ReceiveChar(void);

/*
 * UART_SendString
 * Transmits a null-terminated string through UART3.
 *
 * Parameters:
 *   str - Pointer to null-terminated string to transmit
 */
void UART_SendString(const char *str);

/*
 * UART_IsDataAvailable
 * Checks if data is available in the receive FIFO.
 *
 * Returns:
 *   1 if data is available, 0 otherwise
 */
uint8_t UART_IsDataAvailable(void);

#endif /* UART_H_ */
