#ifndef MOTOR_H
#define MOTOR_H

#include <stdint.h>

void enable_motor(void);
void motor_turn(void);
uint32_t read_uart_delay(void);

#endif // MOTOR_H
