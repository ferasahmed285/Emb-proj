#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "uart.h"
#include "motor.h"

//
// Function to enable and configure the motor
//
void enable_motor(void)
{
    //
    // Enable GPIO Port D (Motor).
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);

    //
    // Wait for the GPIO module to be ready.
    //
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOD))
    {
    }

    //
    // Configure PD0 (IN1) and PD1 (IN2) as outputs for the motor.
    //
    GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    //
    // Increase drive strength to 8mA for motor control pins.
    //
    GPIOPadConfigSet(GPIO_PORTD_BASE, GPIO_PIN_0 | GPIO_PIN_1, GPIO_STRENGTH_8MA_SC, GPIO_PIN_TYPE_STD);

    //
    // Initialize Motor: Stopped (IN1=0, IN2=0)
    //
    GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_0 | GPIO_PIN_1, 0);
}

//
// Function to read delay from UART (receives delay value directly)
// Returns delay in cycles (5-30 seconds)
//
uint32_t read_uart_delay(void)
{
    char receivedChar;
    uint32_t delay_seconds = 5; // Default 5 seconds

    // Read first digit
    receivedChar = UART_ReceiveChar();

    if (receivedChar >= '0' && receivedChar <= '9')
    {
        delay_seconds = receivedChar - '0';

        // Check for second digit (for values 10-30)
        receivedChar = UART_ReceiveChar();

        if (receivedChar >= '0' && receivedChar <= '9')
        {
            delay_seconds = delay_seconds * 10 + (receivedChar - '0');
        }
    }

    // Clamp to 5-30 range
    if (delay_seconds < 5)
        delay_seconds = 5;
    if (delay_seconds > 30)
        delay_seconds = 30;

    // Calculate SysCtlDelay cycles (approx 5,333,333 cycles per second at 16MHz)
    return delay_seconds * 5333333;
}

//
// Function to test the motor with variable delay from UART
// Turn right -> Wait -> Turn left
//
void motor_turn(void)
{
    uint32_t delay_cycles;

    // Read UART to get delay
    delay_cycles = read_uart_delay();

    // 1. Output 1 to PD0 and 0 to PD1 (Turn Right)
    GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_0 | GPIO_PIN_1, GPIO_PIN_0);

    // Run for 1 second
    SysCtlDelay(5333333);

    // 2. Wait for calculated delay (Stop motor)
    GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_0 | GPIO_PIN_1, 0);
    SysCtlDelay(delay_cycles);

    // 3. Output 0 to PD0 and 1 to PD1 (Turn Left)
    GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_0 | GPIO_PIN_1, GPIO_PIN_1);

    // Run for 1 second
    SysCtlDelay(5333333);

    // Stop motor after sequence
    GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_0 | GPIO_PIN_1, 0);
}
