# CSE322 Door Lock System

Two-ECU door lock system built on TM4C123GH6PM (Tiva-C). The HMI ECU handles keypad, LCD, LEDs, ADC (potentiometer), and user flow. The Control ECU manages EEPROM-stored password, motor actuation, buzzer alarm, and command processing. ECUs communicate over UART2 (115200, 8N1) using a simple ASCII protocol.

## Overview
- Architecture: Dual-MCU
  - HMI_ECU: User interface and control flow
  - Control_ECU: Secure store + actuators
- Communication: UART2 over PD6 (RX) / PD7 (TX) on both boards
- Timing: SysTick for delays; GPTM timers for precise buzzer/motor timing
- Storage: On-chip EEPROM for password and door timeout seconds

## Features
- Initial setup if no password found; enforced via `SETUP_COMPLETE` flag in EEPROM
- 5-digit numeric password entry and verification
- Three-attempt lockout with 20s delay and audible alarm
- Change password (requires current password, also 3-attempt policy)
- Adjustable door hold-open timeout (5–30s) via potentiometer (ADC0/PE3)
- Motor sequence: unlock → wait timeout → lock
- Simple, robust UART protocol with 1-byte ACK (`'1'`) or NACK (`'0'`)

## Repository Structure
- [Control_ECU/](Control_ECU)
  - Core: [main.c](Control_ECU/main.c), [uart.c](Control_ECU/uart.c) + [uart.h](Control_ECU/uart.h), [systick.c](Control_ECU/systick.c) + [systick.h](Control_ECU/systick.h)
  - Actuators: [motor.c](Control_ECU/motor.c) + [motor.h](Control_ECU/motor.h), [buzzer.c](Control_ECU/buzzer.c) + [buzzer.h](Control_ECU/buzzer.h)
  - Storage: [eeprom.c](Control_ECU/eeprom.c) + [eeprom.h](Control_ECU/eeprom.h)
  - IAR project: `embProj.ewp`, `project.eww` (and debug/settings folders)
- [HMI_ECU/](HMI_ECU)
  - Core: [main.c](HMI_ECU/main.c), [uart.c](HMI_ECU/uart.c) + [uart.h](HMI_ECU/uart.h), [systick.c](HMI_ECU/systick.c) + [systick.h](HMI_ECU/systick.h)
  - UI: [lcd.c](HMI_ECU/lcd.c) + [lcd.h](HMI_ECU/lcd.h), [keypad.c](HMI_ECU/keypad.c) + [keypad.h](HMI_ECU/keypad.h)
  - GPIO HAL: [dio.c](HMI_ECU/dio.c) + [dio.h](HMI_ECU/dio.h)
  - LEDs: [led.c](HMI_ECU/led.c) + [led.h](HMI_ECU/led.h)
  - ADC (potentiometer): [adc.c](HMI_ECU/adc.c) + [adc.h](HMI_ECU/adc.h)
  - IAR project: `embProj.ewp`, `project.eww` (and debug/settings folders)

## Hardware & Pinout
Targets: TM4C123GH6PM (Tiva-C Series). Both ECUs configure UART2 and different peripherals.

- Shared (both ECUs)
  - UART2: PD6=U2RX, PD7=U2TX (PD7 requires NMI unlock handled in code)
  - System clock: 16 MHz external crystal; SysTick configured for ms-scale delays

- Control_ECU
  - Motor: PD0 (IN1), PD1 (IN2)
  - Buzzer: PA3 (digital out)
  - EEPROM: On-chip EEPROM0 (addresses defined in [eeprom.h](Control_ECU/eeprom.h))
  - Timers: Timer0 (motor seconds), Timer1 (buzzer milliseconds)

- HMI_ECU
  - LCD (4-bit): PB0=RS, PB1=EN, PB2=D4, PB3=D5, PB4=D6, PB5=D7
  - Keypad 4x4: Rows PA2–PA5 (inputs with pull-ups), Cols PC4–PC7 (outputs)
  - LEDs (RGB): PF1=RED, PF2=BLUE, PF3=GREEN
  - ADC Potentiometer: PE3 = AIN0 (ADC0, SS3)

- Wiring Between ECUs (UART)
  - HMI TX (PD7) → Control RX (PD6)
  - HMI RX (PD6) ← Control TX (PD7)
  - Common GND between boards

Voltage levels: 3.3V on Tiva-C; no level shifting required between identical boards.

## UART Protocol
Commands are ASCII, newline-terminated. Responses are single bytes unless noted.

- `STS` → `'1'` if password set, `'0'` otherwise
- `SET:xxxxx` → store 5-digit password; returns `'1'` on success
- `CHK:xxxxx` → verify password only; returns `'1'` (match) or `'0'` (mismatch)
- `PWD:xxxxx` → verify then run door sequence; returns `'1'` on match, `'0'` on mismatch
- `ALM` → trigger buzzer alarm (3 short beeps), no response payload beyond timing
- `TMO:xx` → set door hold-open timeout seconds (range 5–30); returns `'1'` on success, `'0'` if out-of-range

Notes:
- Passwords are numeric-only and fixed length 5.
- Control ECU filters digits after `:` and ignores other characters before parsing.

## Behavior Summary
- Boot
  - HMI shows splash then checks `STS`.
  - If no password, HMI requests password twice and sends `SET` on match.
  - If password exists, HMI asks for password and verifies via `CHK`.
- Main Menu (HMI)
  - `A`: Open door (3 attempts). On 3rd failure: `ALM` and 20s lockout.
  - `B`: Change password. Verifies old password (3 attempts) then prompts for new twice and sends `SET`.
  - `*`: Set timeout. Read potentiometer (maps 0–4095 → 5–30s). Requires password via `CHK`, then sends `TMO`.
- Control ECU door sequence (on valid `PWD`)
  - Drive motor to unlock for 1s → stop and wait configured timeout → drive to lock for 1s → stop.

## Build & Flash (IAR EWARM)
- IDE: IAR Embedded Workbench for ARM (EWARM)
- Toolchain: TivaWare driverlib expected in include paths
- Projects:
  - Open [Control_ECU/project.eww](Control_ECU/project.eww) and [HMI_ECU/project.eww](HMI_ECU/project.eww)
  - Target: TM4C123GH6PM @ 16 MHz external crystal
  - Ensure driverlib and device headers are available in the IAR environment
- Flash each ECU to its respective board; then connect UART2 cross-over and ground.

## Key Configuration
- Password length: 5 (`PASSWORD_LENGTH` in both ECUs)
- EEPROM layout (see [eeprom.h](Control_ECU/eeprom.h))
  - `PASSWORD_ADDRESS` `0x0000` (8 bytes used)
  - `TIMEOUT_ADDRESS` `0x0010` (uint32)
  - `SETUP_FLAG_ADDRESS` `0x0020` (uint32, value `0x55` => setup complete)
- Default timeout if unset/out-of-range: 10s

## Troubleshooting
- No UART response
  - Verify TX/RX cross, shared ground, and both at 115200 8N1
  - PD7 unlock is handled, but ensure both boards run the provided UART2 init
- LCD shows no text
  - Confirm LCD wiring on Port B (RS/EN/D4–D7) and common ground
- Keypad not reading
  - Check pull-ups on PA2–PA5 and drive of PC4–PC7; verify row/col mapping in [keypad.c](HMI_ECU/keypad.c)
- ADC timeout not changing
  - Verify potentiometer on PE3 (AIN0) and ADC0 SS3 config in [adc.c](HMI_ECU/adc.c)

## Notes & Limitations
- Protocol is plaintext without CRC/authentication; intended for lab use
- Password is numeric and stored in EEPROM without hashing (educational scope)
- Blocking UART/API calls; timing is cooperative via delays

## Acknowledgments
- Course: CSE322 Introduction to Embedded Systems
- Target MCU: TI TM4C123GH6PM (Tiva-C)
