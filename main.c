#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "buzzer.h"
#include "motor.h"

int main(void)
{
  //
  // Set the clocking to run at 50 MHz from the PLL.
  //
  SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ |
                 SYSCTL_OSC_MAIN);

  // Initialize buzzer (and ensure it is OFF)
  enable_buzzer();

  enable_motor();
  while (1)
  {
    buzzer_test();
    SysCtlDelay(16666667);
    motor_test();
  }
}
