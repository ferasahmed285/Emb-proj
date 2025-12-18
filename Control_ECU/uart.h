/******************************************************************************
 * File: uart.h
 * Module: UART (Universal Asynchronous Receiver/Transmitter)
 * Description: Header file for TM4C123GH6PM UART2 Driver
 * Author: Updated for UART2
 * Date: December 15, 2025
 *
 * Configuration:
 *   - UART2 (PD6: RX, PD7: TX)
 *   - Baud Rate: 115200
 *   - Data: 8 bits
 *   - Parity: None
 *   - Stop: 1 bit
 *   - PD7 requires unlocking (NMI pin)
 ******************************************************************************/

#ifndef UART_H_
#define UART_H_

#include <stdint.h>
#include <stdbool.h>

/*
 * UART2_Init
 * Initializes UART2 on PD6 (RX) and PD7 (TX) with the following settings:
 *   - Baud Rate: 115200
 *   - 8 data bits, no parity, 1 stop bit
 *   - Unlocks PD7 (NMI pin) automatically
 */
void UART2_Init(void);

/*
 * UART2_SendChar
 * Sends a single character over UART2 (blocking).
 *
 * Parameters:
 *   data - Character to send
 */
void UART2_SendChar(char data);

/*
 * UART2_ReceiveChar
 * Receives a single character from UART2 (blocking).
 *
 * Returns:
 *   Received character
 */
char UART2_ReceiveChar(void);

/*
 * UART2_SendString
 * Sends a null-terminated string over UART2.
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