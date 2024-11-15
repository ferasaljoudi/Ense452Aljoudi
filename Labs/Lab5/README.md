# Lab 5: RTOS

This project implements an LED control system on a microcontroller using FreeRTOS and UART communication. The LED's blink rate is adjusted dynamically through a command-line interface (CLI) over UART.

## Features

- <b>LED Blinking Control:</b> The LED blinks at a rate specified by the user.
- <b>CLI Interface:</b> Users can set the blink rate by entering a number directly in the terminal.
- <b>Dynamic Update:</b> After entering a new rate, the CLI clears and displays the updated rate.
