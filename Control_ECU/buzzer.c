#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "buzzer.h"

//
// Function to initialize GPTM Timer for buzzer delays
//
static void buzzer_timer_init(void)
{
    // Enable Timer1 peripheral
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER1))
    {
    }

    // Configure Timer1 as a 32-bit periodic timer
    TimerConfigure(TIMER1_BASE, TIMER_CFG_PERIODIC);
}

//
// Function to delay using GPTM Timer (in milliseconds)
//
static void buzzer_delay_ms(uint32_t milliseconds)
{
    // Calculate timer load value for millisecond delay
    // System clock / 1000 gives ticks per millisecond
    uint32_t load_value = (SysCtlClockGet() / 1000) * milliseconds;

    // Set timer load value
    TimerLoadSet(TIMER1_BASE, TIMER_A, load_value - 1);

    // Enable the timer
    TimerEnable(TIMER1_BASE, TIMER_A);

    // Wait for timer to timeout
    while (TimerValueGet(TIMER1_BASE, TIMER_A) != 0)
    {
    }

    // Disable the timer
    TimerDisable(TIMER1_BASE, TIMER_A);
}

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

    // Initialize GPTM Timer for buzzer delays
    buzzer_timer_init();
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

        // Beep for 150ms using GPTM
        buzzer_delay_ms(150);

        // Turn off buzzer
        GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 0);

        // Gap between beeps: 100ms using GPTM
        if (i < 2) // No gap after last beep
        {
            buzzer_delay_ms(100);
        }
    }
}
