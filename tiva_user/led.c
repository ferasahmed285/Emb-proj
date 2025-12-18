#include "led.h"

#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"

static uint8_t LED_PinMask(LEDColor color)
{
    switch (color)
    {
    case LED_RED:
        return GPIO_PIN_1; // PF1
    case LED_GREEN:
        return GPIO_PIN_3; // PF3
    case LED_BLUE:
        return GPIO_PIN_2; // PF2
    default:
        return 0u;
    }
}

void LED_Init(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF))
        ;
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, 0); // All off
}

void LED_On(LEDColor color)
{
    uint8_t mask = LED_PinMask(color);
    if (mask != 0u)
    {
        GPIOPinWrite(GPIO_PORTF_BASE, mask, mask);
    }
}

void LED_Off(LEDColor color)
{
    uint8_t mask = LED_PinMask(color);
    if (mask != 0u)
    {
        GPIOPinWrite(GPIO_PORTF_BASE, mask, 0);
    }
}

void LED_AllOff(void)
{
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, 0);
}
