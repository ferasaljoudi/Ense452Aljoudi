#include "elevator.h"
#include <stdio.h>
#include <string.h>
#include "main.h"

// External variables to access them
extern UART_HandleTypeDef huart2;
extern volatile uint32_t elapsedTime;
extern char elevatorState[];
extern volatile uint8_t currentFloor;

// Updates the status window displayed on the top part of the terminal
void UpdateStatusWindow(void)
{
    char buffer[100];
    const char clearCommand[] = "\033[H\033[J"; // Clear screen
    HAL_UART_Transmit(&huart2, (uint8_t *)clearCommand, sizeof(clearCommand) - 1, HAL_MAX_DELAY);

    // Format and transmit the current status
    int len = snprintf(buffer, sizeof(buffer),
                       "Time: %lus\r\nState: %s\r\nFloor: %u\r\n\n********************************\r\n\n",
                       elapsedTime, elevatorState, currentFloor);
    HAL_UART_Transmit(&huart2, (uint8_t *)buffer, len, HAL_MAX_DELAY);

    // Update the color of the elevator
    UpdateElevatorVisuals();

    // Move cursor to the input window row (line 14)
    const char inputPrompt[] = "\033[14;1H";
    HAL_UART_Transmit(&huart2, (uint8_t *)inputPrompt, sizeof(inputPrompt) - 1, HAL_MAX_DELAY);
}

// Updates the visual representation of the elevator (the color) on the middle part of the terminal
void UpdateElevatorVisuals(void) {
    char buffer[800];
    int len = 0;

    // Iterate through the floors, color the currentFloor with green and the rest with default color
    for (int floor = 4; floor >= 1; floor--) {
        if (floor == currentFloor) {
            len += snprintf(buffer + len, sizeof(buffer) - len, "\033[32mFloor %d: [---]\033[0m\r\n", floor);
        } else {
            len += snprintf(buffer + len, sizeof(buffer) - len, "Floor %d: [---]\r\n", floor);
        }
    }

    len += snprintf(buffer + len, sizeof(buffer) - len, "\n\r********************************\r\n\n");

    // Transmit buffer
    HAL_UART_Transmit(&huart2, (uint8_t *)buffer, len, HAL_MAX_DELAY);
}

// Updates the input window (the bottom part on the terminal) to show the user input
void UpdateInputWindow(const char *inputBuffer)
{
    char buffer[300];
    int len = 0;

    // Position the cursor at row 14
    len += snprintf(buffer + len, sizeof(buffer) - len, "\033[14;1H");

    // Display the input prompt
    len += snprintf(buffer + len, sizeof(buffer) - len, "User Input: %s", inputBuffer);

    // Clear the remaining line after the input
    len += snprintf(buffer + len, sizeof(buffer) - len, "\033[K");

    // Transmit the input buffer to the terminal
    HAL_UART_Transmit(&huart2, (uint8_t *)buffer, len, HAL_MAX_DELAY);
}

// Displays an error message when the user enters an invalid command
void invalidEntry(void)
{
    char buffer[300];
    int len = 0;

    // Move the cursor to row 15 (since the user input at row 14)
    len += snprintf(buffer + len, sizeof(buffer) - len, "\033[15;0H");

    // Add the error message
    len += snprintf(buffer + len, sizeof(buffer) - len,
                    "Invalid input. Please enter ('o' for Open, 'c' for Close, 'M' for Maintenance, '1', '2', '3', or '4' for Floors, or push the button for Emergency).\r\n");

    // Transmit the error message to the terminal
    HAL_UART_Transmit(&huart2, (uint8_t *)buffer, len, HAL_MAX_DELAY);
}
