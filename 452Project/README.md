# Elevator Control System Project

## Project Description

The goal of this project is to design an elevator control system using the STM32 Cortex-M3 board with FreeRTOS and hardware interrupts. The system includes a CLI for user interaction to control elevator operations such as floor selection, door opening, door closing, and maintenance mode activation.

## Project Objectives

- Using the STM32 Cortex-M3 hardware peripherals (GPIO, USART2, Timer2).
- Implement a multitasking environment with FreeRTOS.
- Enable inter-task and ISR-task communication via FreeRTOS queues.
- Provide a user-friendly CLI for runtime control and testing.

## Features

<b>1. CLI Commands:</b>

- `1`, `2`, `3`, `4`: Select floors.
- `o`, `c`: Open and close doors.
- `M`: Activate maintenance mode.

<b>2. Tasks:</b>

- `StatusWindowTask` Increase the time per seconds and update the status window (Top side of terminal).
- `ProcessUserInputTask` Handles user input via CLI.
- `StartDefaultTask` Manages emergency button states.

<b>3. Interrupts:</b>

- `Emergency Button` Triggered by EXTI0_IRQHandler to enter emergency mode.
- `UART Communication` Processes user commands.

### [Milestone.pdf](./Milestone.pdf) outlines the completed work in the first phase and future tasks for the final phase.

### [ElevatorProject.pdf](./ElevatorProject.pdf) outlines the completed work in the final phase.
