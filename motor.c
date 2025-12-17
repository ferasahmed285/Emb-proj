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
    // Enable GPIO Port D (Motor)
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);

    // Wait for the GPIO module to be ready
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOD))
    {
    }

    // Configure PD0 (IN1) and PD1 (IN2) as outputs for the motor
    GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    // Increase drive strength to 8mA for motor control pins
    GPIOPadConfigSet(GPIO_PORTD_BASE, GPIO_PIN_0 | GPIO_PIN_1, GPIO_STRENGTH_8MA_SC, GPIO_PIN_TYPE_STD);

    // Initialize Motor: Stopped (IN1=0, IN2=0)
    GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_0 | GPIO_PIN_1, 0);
}

//
// Motor sequence: Turn right (1s) -> Wait (UART delay) -> Turn left (1s)
//
void motor_sequence(void)
{
    // char receivedChar;
    // uint32_t delay_seconds = 5; // Default 5 seconds

    // // Read first digit from UART
    // receivedChar = UART2_ReceiveChar();

    // if (receivedChar >= '0' && receivedChar <= '9')
    // {
    //     delay_seconds = receivedChar - '0';

    //     // Check for second digit (for values 10-30)
    //     receivedChar = UART2_ReceiveChar();

    //     if (receivedChar >= '0' && receivedChar <= '9')
    //     {
    //         delay_seconds = delay_seconds * 10 + (receivedChar - '0');
    //     }
    // }

    // // Clamp to 5-30 second range
    // if (delay_seconds < 5)
    //     delay_seconds = 5;
    // if (delay_seconds > 30)
    //     delay_seconds = 30;

    // 1. Turn Right (PD0=1, PD1=0)
    GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_0 | GPIO_PIN_1, GPIO_PIN_0);
    SysCtlDelay(5333333); // 1 second at 16MHz

    // 2. Stop motor and wait for delay
    GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_0 | GPIO_PIN_1, 0);
    // SysCtlDelay(delay_seconds * 5333333); // Variable delay
    SysCtlDelay(5333333); // 1 second at 16MHz

    // 3. Turn Left (PD0=0, PD1=1)
    GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_0 | GPIO_PIN_1, GPIO_PIN_1);
    SysCtlDelay(5333333); // 1 second at 16MHz

    // 4. Stop motor
    GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_0 | GPIO_PIN_1, 0);
}
