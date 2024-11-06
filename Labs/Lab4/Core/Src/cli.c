#include "cli.h"
#include "timer.h"
#include <string.h>
#include <stdio.h>

char cli_buffer[100];
int buffer_index = 0;
extern UART_HandleTypeDef huart2;

// Function to print a message over UART
void cli_print(const char *message)
{
    HAL_UART_Transmit(&huart2, (uint8_t*)message, strlen(message), HAL_MAX_DELAY);
}

// Function to toggle or provide LED state
void toggle_led(int state)
{
    if (state)
    {
    	// Turn it on
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
        cli_print("You have turned the LED ON\r\n");
    }
    else
    {
    	// Turn it off
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
        cli_print("You have turned the LED OFF\r\n");
    }
}

void query_led()
{
    GPIO_PinState state = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5);
    if (state == GPIO_PIN_SET)
    {
        cli_print("LED is ON\r\n");
    }
    else
    {
        cli_print("LED is OFF\r\n");
    }
}

// Help function
void cli_help()
{
	cli_print("You asked for HELP\r\n");
    cli_print("\x1b[33mAvailable Commands:\r\n");
    cli_print("'1' to: Turn the LED on\r\n");
    cli_print("'0' to: Turn the LED off\r\n");
    cli_print("'s' or 'S' to: Check the LED state\r\n");
    cli_print("'t' or 'T' to: run the timing function\r\n");
    cli_print("'h' or 'H' to: Show this help message\r\n\x1b[0m");
}

void cli_init(void)
{
    cli_print("\x1b[36mENSE452-LAB-4:\r\n");
    cli_print("Available Commands:\r\n");
    cli_print("'1' to: Turn the LED on\r\n");
    cli_print("'0' to: Turn the LED off\r\n");
    cli_print("'s' or 'S' to: Check the LED state\r\n");
    cli_print("'t' or 'T' to: run the timing function\r\n");
    cli_print("'h' or 'H' to: Show this help message\r\n");
    cli_print("********************************************\r\n\x1b[0m");
}

void cli_process(void)
{
    uint8_t rx_char;

    // Receive one character through UART
    HAL_UART_Receive(&huart2, &rx_char, 1, HAL_MAX_DELAY);

    // Echo the received character back to the terminal except for backspace
	if (rx_char != '\b' && rx_char != 127)
	{
		HAL_UART_Transmit(&huart2, &rx_char, 1, HAL_MAX_DELAY);
	}

    // Handle backspace
    if (rx_char == '\b' || rx_char == 127)
	{
		if (buffer_index > 0)
		{
			// Move back in the buffer
			buffer_index--;
			// Null terminate the buffer
			cli_buffer[buffer_index] = '\0';

			const char backspace_seq[] = "\b \b";
			HAL_UART_Transmit(&huart2, (uint8_t*)backspace_seq, sizeof(backspace_seq) - 1, HAL_MAX_DELAY);
		}
		return;
	}

    // If Enter key is pressed
    if (rx_char == '\r' || rx_char == '\n')
    {
        cli_buffer[buffer_index] = '\0';

        // Check which command is being entered
        if (strcmp(cli_buffer, "1") == 0)
        {
            toggle_led(1);
        }
        else if (strcmp(cli_buffer, "0") == 0)
        {
            toggle_led(0);
        }
        else if (strcmp(cli_buffer, "s") == 0 || strcmp(cli_buffer, "S") == 0)
        {
            query_led();
        }
        else if (strcmp(cli_buffer, "t") == 0 || strcmp(cli_buffer, "T") == 0) {
            run_timing_tests();
        }
        else if (strcmp(cli_buffer, "h") == 0 || strcmp(cli_buffer, "H") == 0)
        {
            cli_help();
        }
        else if (strlen(cli_buffer) > 0)
        {
            cli_print("\x1b[31mUnknown command. Type 'H' or 'h' for help.\r\n\x1b[0m");
        }

        // Reset the buffer
        buffer_index = 0;
        // Print the prompt again
        cli_print("> ");
    }
    else if (rx_char >= 32 && rx_char <= 126)
    {
        cli_buffer[buffer_index++] = rx_char;
        if (buffer_index >= sizeof(cli_buffer) - 1)
        {
            buffer_index = 0;
        }
    }
}
