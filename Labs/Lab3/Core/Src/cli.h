#ifndef CLI_H
#define CLI_H

#include "stm32f1xx_hal.h"

extern char input_buffer[100];
extern int buffer_index;
extern UART_HandleTypeDef huart2;

extern volatile uint8_t received_char;

// Function declaration
void upper_window(void);
void execute_cli(void);
void uart_transmit(const char *str);
void handle_input(const char *cmd);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);

#endif
