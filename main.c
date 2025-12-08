#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"

int main(void)
{
  //
  // Set the clocking to run at 50 MHz from the PLL.
  //
  SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ |
                 SYSCTL_OSC_MAIN);

  //
  // Enable the GPIO Port D.
  // IN1 is connected to PD0, IN2 is connected to PD1.
  //
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);

  //
  // Wait for the GPIO module to be ready.
  //
  while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOD))
  {
  }

  //
  // Configure PD0 and PD1 as outputs.
  //
  GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, GPIO_PIN_0 | GPIO_PIN_1);

  //
  // Loop forever.
  //
  while (1)
  {
    //
    // 1. Turn Right (1 second)
    // IN1 (PD0) = High, IN2 (PD1) = Low
    //
    GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_0 | GPIO_PIN_1, GPIO_PIN_0);

    // Delay 1 second
    // (1 * 50,000,000) / 3 = 16,666,667
    SysCtlDelay(16666667);

    //
    // 2. Stop (1 second)
    // IN1 (PD0) = Low, IN2 (PD1) = Low
    //
    GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_0 | GPIO_PIN_1, 0);

    // Delay 1 second
    SysCtlDelay(16666667);

    //
    // 3. Turn Left (1 second)
    // IN1 (PD0) = Low, IN2 (PD1) = High
    //
    GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_0 | GPIO_PIN_1, GPIO_PIN_1);

    // Delay 1 second
    SysCtlDelay(16666667);
  }
}
