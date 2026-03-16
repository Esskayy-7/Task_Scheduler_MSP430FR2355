# MSP430 Cooperative Task Scheduler

A bare-metal cooperative task scheduler implemented from scratch in C for the **Texas Instruments MSP430FR2355** microcontroller — no RTOS, no HAL abstractions, no external libraries. Every peripheral driver, interrupt handler, and scheduling mechanism is hand-built.

Built as a hands-on exploration of how real-time task management works at the hardware level on resource-constrained embedded targets.

---

## Overview

Most embedded projects lean on an RTOS like FreeRTOS to handle task switching and timing. This project deliberately avoids that — the goal was to understand the underlying mechanics: how a tick-based timer drives a scheduler, how ISRs interact with shared state, and how peripheral drivers are structured cleanly without abstraction overhead.

The result is a system that manages **four concurrent tasks** — two LED blinkers, an ADC sampler, and a UART transmitter — all driven by a single hardware timer and coordinated by a lightweight round-robin scheduler.

---

## Features

- **Cooperative round-robin scheduler** — tick-based, period-configurable per task, no dynamic memory allocation
- **Timer driver** — TimerB0 in continuous mode on ACLK (~32 kHz), generating system ticks at ~10 ms intervals
- **ADC driver** — 8-bit conversion on P1.2 (A2), interrupt-driven, dynamically adjusts LED blink periods based on analog input
- **UART driver** — UART A1 configured via SMCLK at 115200 baud; transmits ADC readings over serial
- **GPIO driver** — Dual LED outputs (P1.0, P6.6) with interrupt-driven button input (P2.3)
- **Double-click detection** — Button ISR distinguishes single vs. double press using tick-delta timing (< 500 ms threshold), adjusting LED periods accordingly
- **Hardware-in-the-loop control** — ADC ISR directly recalculates LED1 and LED2 blink periods from the sampled analog value in real time

---

## Hardware

| Component        | Detail                          |
|------------------|---------------------------------|
| Microcontroller  | Texas Instruments MSP430FR2355 |
| IDE              | Code Composer Studio (CCS)      |
| LED 1            | P1.0 (onboard red LED)          |
| LED 2            | P6.6 (onboard green LED)        |
| Button           | P2.3 (onboard S2, active low)   |
| ADC Input        | P1.2 / A2 (potentiometer or analog source) |
| UART TX          | P4.3 (UART A1)                  |
| Clock Sources    | ACLK ~32 kHz (timer), SMCLK (UART/ADC) |

---

## System Architecture

The codebase is organised into three layers. Each layer only depends on the one below it — the scheduler never touches registers directly, and the drivers never know about task logic.

```
┌─────────────────────────────────────────────┐
│              Scheduler Layer                │
│         Scheduler.c / Scheduler.h          │
│   Round-robin loop, tick tracking,         │
│   task table, period management            │
└────────────────┬────────────────────────────┘
                 │ calls
┌────────────────▼────────────────────────────┐
│           Application Task Layer           │
│  ADC_Task   GPIO_Task  Timer_Task UART_Task │
│  Thin wrappers: init functions + task functions  │
└────────────────┬────────────────────────────┘
                 │ calls
┌────────────────▼────────────────────────────┐
│            Hardware Driver Layer           │
│     ADC.c   GPIO.c   Timer.c   UART.c      │
│   Register-level peripheral configuration  │
│   + ISR definitions                        │
└─────────────────────────────────────────────┘
```

---

## How It Works

### The Tick System

TimerB0 runs in continuous mode on ACLK (~32 kHz). CCR1 is set to fire every **328 counts**, which gives approximately one tick every **10 ms**. Each ISR increments a global `tracker` counter:

```c
#pragma vector = TIMER0_B1_VECTOR
__interrupt void ISR_TB0_CCR1(void) {
    tracker++;
    TB0CCR1 += COUNTS_PER_TICK;
    TB0CCTL1 &= ~CCIFG;
}
```

`tracker` is the heartbeat of the entire system — everything else is measured relative to it.

### The Scheduler

The scheduler holds a static array of `Task` structs:

```c
typedef struct {
    unsigned int period;   // ticks between executions
    void (*pf)(void);      // pointer to task function
    uint32_t last_run;     // tick value when last executed
} Task;
```

The main loop iterates over all tasks and fires any whose time has come:

```c
void run_scheduler(void) {
    while(1) {
        for(i = 0; i < NUMBER_OF_TASKS; i++) {
            if(tracker - tasks[i].last_run >= tasks[i].period) {
                tasks[i].pf();
                tasks[i].last_run = tracker;
            }
        }
    }
}
```

Because this is a **cooperative** scheduler, tasks are never preempted — each one runs to completion before the next is checked. This keeps shared state management simple.

### Task Table

| Task         | Period (ticks) | Period (~ms) | Function      |
|--------------|---------------|--------------|---------------|
| Blink LED 1  | Dynamic        | Variable     | `blink_led_1` |
| Blink LED 2  | Dynamic        | Variable     | `blink_led_2` |
| UART Transmit| 25             | ~250 ms      | `UART_read`   |
| ADC Sample   | 50             | ~500 ms      | `ADC_read`    |

LED periods are dynamically adjusted at runtime via button presses and ADC input.

### ADC-Driven Period Control

The ADC ISR fires after each 8-bit conversion and directly updates the LED blink periods:

```c
#pragma vector = ADC_VECTOR
__interrupt void ADC_ISR(void) {
    *T_LED_1 = T_LED_1_MAX * (ADCMEM0) / (1 << RES_EXP);
    *T_LED_2 = T_LED_2_MAX * ((1 << RES_EXP) - ADCMEM0) / (1 << RES_EXP);
}
```

As the ADC value increases, LED1 speeds up and LED2 slows down — they trade off inversely. Rotating a potentiometer on P1.2 produces a real-time visual effect.

### Button & Double-Click Detection

The button ISR on P2.3 calls `set_polarity()`, which checks whether the time since the last press is under the double-click threshold (50 ticks ≈ 500 ms):

```c
static int set_polarity(void) {
    if((tracker - last_timer_value) < DOUBLE_PRESS_TIME_LIMIT)
        return 0;  // double click
    return 1;      // single click
}
```

- **Single click** → LED1 max period increases by 250 ms (blink slows down)
- **Double click** → LED1 max period decreases by 250 ms net (blink speeds up)
- LED2 is always kept at 2× LED1's max period

---

## Project Structure

```
├── main.c                        # Entry point — inits + starts scheduler
│
├── Scheduler/
│   ├── Scheduler.h               # Task struct, extern shared state, period pointers
│   └── Scheduler.c               # Task table, run_scheduler(), tracker variable
│
├── Application/
│   ├── ADC_Task.h / ADC_Task.c   # run_ADC_once(), ADC_read()
│   ├── GPIO_Task.h / GPIO_Task.c # run_GPIO_once(), blink_led_1(), blink_led_2()
│   ├── Timer_Task.h / Timer_Task.c # run_TIMER_once()
│   └── UART_Task.h / UART_Task.c # run_UART_once(), UART_read()
│
└── Drivers/
    ├── ADC.h / ADC.c             # ADC init, start_conversion(), ADC ISR
    ├── GPIO.h / GPIO.c           # GPIO init, toggle fns, button ISR, double-click logic
    ├── Timer.h / Timer.c         # TimerB0 init, tick ISR
    └── UART.h / UART.c           # UART A1 init, transmit_character()
```

---

## Build & Flash

1. Open **Code Composer Studio (CCS)**
2. Create a new CCS project targeting your MSP430 variant
3. Import all source files, preserving the folder structure above
4. Build the project
5. Connect your LaunchPad via USB and flash (`Run → Debug`)

> Make sure your CCS project includes the correct MSP430 device support package for your specific chip variant.

---

## Demo

> _Add a short GIF or video here of the LEDs blinking and responding to the potentiometer and button. Even a 5-second phone recording makes a significant difference for anyone reviewing the project._

Suggested demo content:
- Rotating the potentiometer → LED1 and LED2 visibly trade blink speeds
- Single button press → both LEDs slow down
- Double button press → both LEDs speed up
- UART output captured in a serial monitor (e.g. PuTTY or CCS terminal) showing ADC values streaming at ~250 ms intervals

---

## Future Improvements

- **Priority levels** — assign high/low priority to tasks so critical tasks (e.g. ADC sampling) are always served first
- **Preemptive scheduling** — move to a preemptive model using timer interrupts to context-switch, removing the cooperative constraint
- **Overflow handling** — add protection for `tracker` rollover on long runtimes
- **Dynamic task registration** — allow tasks to be added/removed at runtime rather than a fixed static table
- **Watchdog timer integration** — use the MSP430 watchdog to recover from any task that hangs

---

## Author

**Kayode Joshua Seriki**
BSc Computer Engineering — Hacettepe University, Ankara
[github.com/Esskayy-7](https://github.com/Esskayy-7) · serikikayode075@gmail.com
