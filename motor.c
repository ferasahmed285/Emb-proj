#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
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
    // Configure PD0 (IN1), PD1 (IN2), and PD2 (Enable) as outputs for the motor.
    //
    GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2);

    //
    // Increase drive strength to 8mA for motor control pins.
    //
    GPIOPadConfigSet(GPIO_PORTD_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2, GPIO_STRENGTH_8MA_SC, GPIO_PIN_TYPE_STD);
}

//
// Function to test the motor
// Turn right 1s -> Wait 5s -> Turn left 1s
//
void motor_test(void)
{
    // 1. Turn Right (IN1 High, IN2 Low)
    GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_0 | GPIO_PIN_1, GPIO_PIN_0);

    // Run for 1 second
    SysCtlDelay(16666667);

    // 2. Wait 5 seconds (Stop motor)
    GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_0 | GPIO_PIN_1, 0);
    SysCtlDelay(16666667 * 5);

    // 3. Turn Left (IN1 Low, IN2 High)
    GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_0 | GPIO_PIN_1, GPIO_PIN_1);

    // Run for 1 second
    SysCtlDelay(16666667);

    // Stop motor after sequence
    GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_0 | GPIO_PIN_1, 0);
}
