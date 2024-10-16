#include "cli.h"
#include <string.h>

char input_buffer[100];
int buffer_index = 0;

volatile uint8_t received_char;
volatile uint8_t data_received = 0;
UART_HandleTypeDef huart2;

void uart_transmit(const char *str) {
    // Wait until UART is ready then send in non-blocking method
    while (HAL_UART_GetState(&huart2) != HAL_UART_STATE_READY);
    HAL_UART_Transmit_IT(&huart2, (uint8_t*)str, strlen(str));
}

void upper_window(void) {
	// Clear the terminal and move the cursor to the top left
	uart_transmit("\x1b[2J");
	uart_transmit("\x1b[0;0H");
	uart_transmit("\x1b[36mENSE452-LAB-3-CLI2:\r\n");
	uart_transmit("Available Commands:\r\n");
	uart_transmit("'1' to: Turn the LED on\r\n");
	uart_transmit("'0' to: Turn the LED off\r\n");
	uart_transmit("'s' or 'S' to: Check the LED state\r\n");
	uart_transmit("'h' or 'H' to: Show this help message\r\n");
	uart_transmit("********************************************\r\n\x1b[0m");
    // Set a scrollable window to start at row 10
	uart_transmit("\x1b[10;r");
    // Move the cursor to the first row of the scrollable region
	uart_transmit("\x1b[10;0H");
	uart_transmit("> ");
}

// Function to continuously run the CLI and process incoming UART data
void execute_cli(void) {
	// Checking if data has been received through the UART
    if (data_received) {
        data_received = 0;
        // Checking if the received character is a newline or carriage return
        if (received_char == '\r' || received_char == '\n') {
            input_buffer[buffer_index] = '\0';
            handle_input(input_buffer);
            buffer_index = 0;
        // Checking if the received character is a backspace
        } else if (received_char == '\b') {
            if (buffer_index > 0) {
                buffer_index--;
                uart_transmit("\b \b");
            }
        // Handle any other received character
        } else {
            input_buffer[buffer_index++] = received_char;
            uart_transmit((const char *)&received_char);
            // Check if the buffer has exceeded its size
            if (buffer_index >= sizeof(input_buffer)) {
                buffer_index = 0;
                uart_transmit("\r\nThe buffer has exceeded its size.\r\n");
                uart_transmit("> ");
            }
        }
        // Re-enabling the non-blocking UART receive interrupt to handle the next character
        HAL_UART_Receive_IT(&huart2, (uint8_t *)&received_char, 1);
    }
}

// Handle the user input
void handle_input(const char *cmd) {
	uart_transmit("\r\n");

    if (strcmp(cmd, "1") == 0) {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
        uart_transmit("\x1b[33mYou turned the LED ON\r\n\x1b[0m");
    } else if (strcmp(cmd, "0") == 0) {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
        uart_transmit("\x1b[33m You turned the LED OFF\r\n\x1b[0m");
    } else if (strcmp(cmd, "s") == 0 || strcmp(cmd, "S") == 0) {
    	if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5)) {
    		uart_transmit("\x1b[32mLED is ON\r\n\x1b[0m");
		} else {
			uart_transmit("\x1b[31mLED is OFF\r\n\x1b[0m");
		}
    } else if (strcmp(cmd, "h") == 0 || strcmp(cmd, "H") == 0) {
    	uart_transmit("\x1b[33mAvailable Commands:\r\n");
    	uart_transmit("'1' to: Turn the LED on\r\n");
    	uart_transmit("'0' to: Turn the LED off\r\n");
    	uart_transmit("'s' or 'S' to: Check the LED state\r\n");
    	uart_transmit("'h' or 'H' to: Show this help message\r\n\x1b[0m");
    } else {
    	uart_transmit("Unknown command! Type 'H' for help.");
    	uart_transmit("\r\n");
    }
    uart_transmit("> ");
}

// Callback function triggered when a UART receive interrupt occurs
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART2) {
        data_received = 1;
    }
}
