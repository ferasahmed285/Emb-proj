#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/adc.h"
#include "motor.h"

//
// Function to enable and configure the motor and ADC for potentiometer
//
void enable_motor(void)
{
    //
    // Enable GPIO Port D (Motor) and Port E (ADC).
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);

    //
    // Wait for the GPIO module to be ready.
    //
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOD) || !SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOE))
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

    //
    // Configure ADC0 Sequence 3 for PE3 (AIN0)
    //
    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_3);
    ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);
    ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_CH0 | ADC_CTL_IE | ADC_CTL_END);
    ADCSequenceEnable(ADC0_BASE, 3);
    ADCIntClear(ADC0_BASE, 3);
}

//
// Function to test the motor with variable delay from potentiometer
// Turn right -> Wait -> Turn left
//
void motor_turn(void)
{
    uint32_t ui32ADC0Value[1];
    uint32_t delay_seconds;
    uint32_t delay_cycles;

    // Trigger ADC conversion
    ADCProcessorTrigger(ADC0_BASE, 3);

    // Wait for conversion to be completed
    while (!ADCIntStatus(ADC0_BASE, 3, false))
    {
    }

    // Clear the ADC interrupt flag
    ADCIntClear(ADC0_BASE, 3);

    // Read ADC Value
    ADCSequenceDataGet(ADC0_BASE, 3, ui32ADC0Value);

    // Map ADC value (0-4095) to delay (5-30 seconds)
    // Formula: 5 + (ADC * 25) / 4095
    delay_seconds = 5 + (ui32ADC0Value[0] * 25) / 4095;

    // Calculate SysCtlDelay cycles (approx 5,333,333 cycles per second at 16MHz)
    delay_cycles = delay_seconds * 5333333;

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
