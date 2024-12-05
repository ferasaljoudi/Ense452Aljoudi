#include "main.h"
#include <stdio.h>
#include <string.h>
#include "cmsis_os.h"
#include "elevator.h"

// Global variables
TIM_HandleTypeDef htim2;
UART_HandleTypeDef huart2;
osThreadId defaultTaskHandle;
extern volatile uint8_t isEmergency;
volatile uint32_t elapsedTime = 0;
char elevatorState[20] = "Idle";
volatile uint8_t currentFloor = 1;

// Function prototypes for system configuration and tasks
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_TIM2_Init(void);
void StartDefaultTask(void const *argument);
void StatusWindowTask(void const *argument);
void ProcessUserInputTask(void const *argument);


int main(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_USART2_UART_Init();
    MX_TIM2_Init();

    // Create RTOS tasks
    // Emergency handling task
    osThreadDef(defaultTask, StartDefaultTask, osPriorityLow, 0, 128);
    defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

    // Timer task
    osThreadDef(statusTask, StatusWindowTask, osPriorityNormal, 0, 256);
    osThreadCreate(osThread(statusTask), NULL);

    // Handling user commands task
    osThreadDef(userInputTask, ProcessUserInputTask, osPriorityNormal, 0, 128);
    osThreadCreate(osThread(userInputTask), NULL);

    // Start the RTOS kernel
    osKernelStart();

    while (1)
    {
    }
}

void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL16;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
    {
        Error_Handler();
    }
}

static void MX_TIM2_Init(void)
{
    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};

    htim2.Instance = TIM2;
    htim2.Init.Prescaler = 7199;
    htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim2.Init.Period = 9999;
    htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
    {
        Error_Handler();
    }
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
    {
        Error_Handler();
    }
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
    {
        Error_Handler();
    }
}

static void MX_USART2_UART_Init(void)
{
    huart2.Instance = USART2;
    huart2.Init.BaudRate = 115200;
    huart2.Init.WordLength = UART_WORDLENGTH_8B;
    huart2.Init.StopBits = UART_STOPBITS_1;
    huart2.Init.Parity = UART_PARITY_NONE;
    huart2.Init.Mode = UART_MODE_TX_RX;
    huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart2.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&huart2) != HAL_OK)
    {
        Error_Handler();
    }
}

static void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

    GPIO_InitStruct.Pin = B1_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = LD2_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_0;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(EXTI0_IRQn);
}

// Task for handling emergency mode and resetting the elevator state
void StartDefaultTask(void const *argument)
{
    for (;;)
    {
        strcpy(elevatorState, "Emergency Mode");
        UpdateStatusWindow();

        while (isEmergency)
        {
            osDelay(100);
        }

        strcpy(elevatorState, "Idle");
        UpdateStatusWindow();
    }
    UpdateStatusWindow();
    osDelay(100);
}

// Task to keep updating the timer on the terminal
void StatusWindowTask(void const *argument)
{
    for (;;)
    {
        osDelay(1000);
        elapsedTime++;
        UpdateStatusWindow();
    }
}

// Task to process user input from the UART terminal
void ProcessUserInputTask(void const *argument)
{
    uint8_t receivedChar;
    char inputBuffer[200] = {0}; // To store user inputs
    int inputLen = 0;

    for (;;)
    {
    	// Wait for a character from the UART
        if (HAL_UART_Receive(&huart2, &receivedChar, 1, HAL_MAX_DELAY) == HAL_OK)
        {
        	// Ignore newline characters
            if (receivedChar != '\r' && receivedChar != '\n')
            {
                inputBuffer[inputLen++] = receivedChar; // Add the character to input buffer
                inputBuffer[inputLen] = '\0'; // Null-terminate the string
            }
            else
            {
                // Process the command
                switch (inputBuffer[0])
                {
                // Open the elevator door
                case 'o':
                	if (strcmp(elevatorState, "Emergency Mode") == 0) break;
                    strcpy(elevatorState, "Door opened");
                    elapsedTime++;
                    UpdateStatusWindow();
                    HAL_Delay(1500);
                    strcpy(elevatorState, "Door closed");
                    elapsedTime++;
                    UpdateStatusWindow();
                    HAL_Delay(1500);
                    strcpy(elevatorState, "Idle");
                    elapsedTime++;
                    UpdateStatusWindow();
                    break;
                // Close the elevator door
                case 'c':
                	if (strcmp(elevatorState, "Emergency Mode") == 0) break;
                    strcpy(elevatorState, "Door closed");
                    elapsedTime++;
                    UpdateStatusWindow();
                    HAL_Delay(1500);
                    strcpy(elevatorState, "Idle");
                    elapsedTime++;
                    UpdateStatusWindow();
                    break;
                case '1': // Move to floor 1
                case '2': // Move to floor 2
                case '3': // Move to floor 3
                case '4': // Move to floor 4
                	if (strcmp(elevatorState, "Emergency Mode") == 0) break;
                	uint8_t targetFloor = inputBuffer[0] - '0';
                	while (currentFloor != targetFloor)
					{
                		if (strcmp(elevatorState, "Emergency Mode") == 0) {
                			targetFloor = currentFloor; // In case of emergency
                			break;
                		}
						if (currentFloor < targetFloor)
							currentFloor++;
						else if (currentFloor > targetFloor)
							currentFloor--;
						// Update visuals and status for each floor
						strcpy(elevatorState, "Elevator moving");
						elapsedTime++;
						UpdateStatusWindow();
						HAL_Delay(1300);
					}
                	if (strcmp(elevatorState, "Emergency Mode") == 0) break;
					// Door operations after reaching the target floor
					strcpy(elevatorState, "Door opened");
					elapsedTime++;
					UpdateStatusWindow();
					HAL_Delay(1500);

					strcpy(elevatorState, "Door closed");
					elapsedTime++;
					UpdateStatusWindow();
					HAL_Delay(1500);

					strcpy(elevatorState, "Idle");
					elapsedTime++;
					UpdateStatusWindow();
                    break;
                case 'M': // Enter maintenance mode
                    strcpy(elevatorState, "Maintenance Mode");
                    UpdateStatusWindow();
                    while (1)
                    {
                    	HAL_Delay(1000);
                    	elapsedTime++;
                    	UpdateStatusWindow();
                    }
                    break;
				// Handle invalid input
                default:
                	invalidEntry();
                	elapsedTime++;
                    break;
                }
                // Clear the input buffer
                inputLen = 0;
                inputBuffer[0] = '\0';
            }
        }
        // Update the input window
		UpdateInputWindow(inputBuffer);
    }
}

void Error_Handler(void)
{
    __disable_irq();
    while (1)
    {
    }
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{
}
#endif
