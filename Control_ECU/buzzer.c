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
    // Enable GPIO Port A (Buzzer).
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    //
    // Wait for the GPIO module to be ready.
    //
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA))
    {
    }

    //
    // Configure PA3 as an output for the buzzer.
    //
    GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, GPIO_PIN_3);
    // High drive strength for buzzer
    GPIOPadConfigSet(GPIO_PORTA_BASE, GPIO_PIN_3, GPIO_STRENGTH_8MA_SC, GPIO_PIN_TYPE_STD);

    //
    // Explicitly turn off the buzzer to prevent floating state noise.
    //
    GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 0);
}

//
// Alarm function: beeps 3 times in a row within 1 second
// Each beep: 150ms on, 100ms off (except last has no off delay)
//
void alarm(void)
{
    uint8_t i;

    // Beep 3 times
    for (i = 0; i < 3; i++)
    {
        // Turn on buzzer (PA3 High)
        GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, GPIO_PIN_3);

        // Beep for ~150ms (800000 cycles at 16MHz ≈ 150ms)
        SysCtlDelay(800000);

        // Turn off buzzer
        GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 0);

        // Gap between beeps: ~100ms (533333 cycles)
        if (i < 2) // No gap after last beep
        {
            SysCtlDelay(533333);
        }
    }
}
