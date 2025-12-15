#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "buzzer.h"

//
// Function to enable and configure the buzzer
//
void enable_buzzer(void)
{
    //
    // Enable GPIO Port B (Buzzer).
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

    //
    // Wait for the GPIO module to be ready.
    //
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOB))
    {
    }

    //
    // Configure PB7 as an output for the buzzer.
    //
    GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_7);
    // High drive strength for buzzer
    GPIOPadConfigSet(GPIO_PORTB_BASE, GPIO_PIN_7, GPIO_STRENGTH_8MA_SC, GPIO_PIN_TYPE_STD);

    //
    // Explicitly turn off the buzzer to prevent floating state noise.
    //
    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_7, 0);
}

//
// Function to test the buzzer
// Turns on buzzer for 1 second
//
void buzzer_test(void)
{
    // Turn on buzzer (PB7 High)
    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_7, GPIO_PIN_7);

    // Delay 1 second
    SysCtlDelay(16666667);

    // Turn off buzzer
    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_7, 0);
}
