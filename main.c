#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"

// Function prototypes
void enable_buzzer(void);
void enable_motor(void);
void buzzer_test(void);
void motor_test(void);

int main(void)
{
  //
  // Set the clocking to run at 50 MHz from the PLL.
  //
  SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ |
                 SYSCTL_OSC_MAIN);
  enable_buzzer();
  enable_motor();
  while (1)
  {
    buzzer_test();
    SysCtlDelay(16666667);
    motor_test();
  }
}

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
}

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
  // Configure PD0 (IN1) and PD1 (IN2) as outputs for the motor.
  //
  GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, GPIO_PIN_0 | GPIO_PIN_1);
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
