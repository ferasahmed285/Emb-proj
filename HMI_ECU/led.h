#ifndef LED_H
#define LED_H

#include <stdint.h>

typedef enum { LED_RED = 0, LED_GREEN = 1, LED_BLUE = 2 } LEDColor;

void LED_Init(void);
void LED_On(LEDColor color);
void LED_Off(LEDColor color);
void LED_AllOff(void);

#endif // LED_H
