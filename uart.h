/******************************************************************************
    * File: uart.h
    * Module: UART (Universal Asynchronous Receiver/Transmitter)
    * Description: Header file for TM4C123GH6PM UART1 Driver (TivaWare)
    * Author: Ahmedhh
    * Date: December 10, 2025
    * 
    * Configuration:
    *   - UART1 (PB0: RX, PB1: TX)
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
    * UART1_Init
    * Initializes UART1 with 115200 baud rate, 8N1 configuration using TivaWare.
    * Uses PB0 (RX) and PB1 (TX).
    * System clock is read at runtime with SysCtlClockGet().
    */
    void UART1_Init(void);

    /*
    * UART1_SendChar
    * Transmits a single character through UART1.
    * Blocks until the transmit FIFO is ready.
    * 
    * Parameters:
    *   data - Character to transmit
    */
    void UART1_SendChar(char data);

    /*
    * UART1_ReceiveChar
    * Receives a single character from UART1.
    * Blocks until a character is available in the receive FIFO.
    * 
    * Returns:
    *   Received character
    */
    char UART1_ReceiveChar(void);

    /*
    * UART1_SendString
    * Transmits a null-terminated string through UART1.
    * 
    * Parameters:
    *   str - Pointer to null-terminated string to transmit
    */
    void UART1_SendString(const char *str);

    /*
    * UART1_IsDataAvailable
    * Checks if data is available in the receive FIFO.
    * 
    * Returns:
    *   1 if data is available, 0 otherwise
    */
    uint8_t UART1_IsDataAvailable(void);

    #endif /* UART_H_ */