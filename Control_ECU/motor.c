/******************************************************************************
 * File: motor.c (Control_ECU)
 * Description: Motor Control with EEPROM Timeout using GPTM Timers
 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "motor.h"
#include "eeprom.h"

//
// Function to initialize GPTM Timer for delays
//
static void motor_timer_init(void)
{
    // Enable Timer0 peripheral
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER0))
    {
    }

    // Configure Timer0A as one-shot; we reload each second
    TimerConfigure(TIMER0_BASE, TIMER_CFG_A_ONE_SHOT);
}

//
// Function to delay using GPTM Timer (in seconds)
//
static void motor_delay_seconds(uint8_t seconds)
{
    uint8_t i;
    for (i = 0; i < seconds; i++)
    {
        // Load 1s interval and clear any stale flag before starting
        TimerLoadSet(TIMER0_BASE, TIMER_A, SysCtlClockGet() - 1);
        TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

        // Enable the timer
        TimerEnable(TIMER0_BASE, TIMER_A);

        // Wait for timeout flag
        while (!(TimerIntStatus(TIMER0_BASE, false) & TIMER_TIMA_TIMEOUT))
        {
        }

        // Clear flag and stop timer
        TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
        TimerDisable(TIMER0_BASE, TIMER_A);
    }
}

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

    // Increase drive strength
    GPIOPadConfigSet(GPIO_PORTD_BASE, GPIO_PIN_0 | GPIO_PIN_1, GPIO_STRENGTH_8MA_SC, GPIO_PIN_TYPE_STD);

    // Initialize Motor: Stopped (IN1=0, IN2=0)
    GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_0 | GPIO_PIN_1, 0);

    // Initialize GPTM Timer for motor delays
    motor_timer_init();
}

//
// Motor sequence: Turn right -> Wait (Variable Timeout) -> Turn left
//
void motor_sequence(void)
{
    // Read timeout from EEPROM (Returns 5-30, or 10 default)
    uint8_t delay_seconds = EEPROM_ReadTimeout();

    // 1. Turn Right (Unlocking)
    // PD0=1, PD1=0
    GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_0 | GPIO_PIN_1, GPIO_PIN_0);
    motor_delay_seconds(1); // 1 second delay using GPTM

    // 2. Stop motor and wait for configured timeout
    GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_0 | GPIO_PIN_1, 0);

    // Wait for configured timeout using GPTM timer
    motor_delay_seconds(delay_seconds);

    // 3. Turn Left (Locking)
    // PD0=0, PD1=1
    GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_0 | GPIO_PIN_1, GPIO_PIN_1);
    motor_delay_seconds(1); // 1 second delay using GPTM

    // 4. Stop motor
    GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_0 | GPIO_PIN_1, 0);
}