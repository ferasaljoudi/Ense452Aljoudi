#include "main.h"
#include <stdio.h>
#include <string.h>
#include "cmsis_os.h"

TIM_HandleTypeDef htim2;
UART_HandleTypeDef huart2;
osThreadId defaultTaskHandle;
extern volatile uint8_t isEmergency;
volatile uint32_t elapsedTime = 0;
char elevatorState[20] = "Idle";
volatile uint8_t currentFloor = 1;

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_TIM2_Init(void);
void StartDefaultTask(void const *argument);
void StatusWindowTask(void const *argument);
void ProcessUserInputTask(void const *argument);

void UpdateElevatorVisuals(void);
void UpdateStatusWindow(void);
void UpdateInputWindow(const char *inputBuffer);

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_USART2_UART_Init();
    MX_TIM2_Init();

    osThreadDef(defaultTask, StartDefaultTask, osPriorityLow, 0, 128);
    defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

    osThreadDef(statusTask, StatusWindowTask, osPriorityNormal, 0, 256);
    osThreadCreate(osThread(statusTask), NULL);

    osThreadDef(userInputTask, ProcessUserInputTask, osPriorityNormal, 0, 128);
    osThreadCreate(osThread(userInputTask), NULL);

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

void StatusWindowTask(void const *argument)
{
    for (;;)
    {
        osDelay(1000);
        elapsedTime++;
        UpdateStatusWindow();
    }
}

void ProcessUserInputTask(void const *argument)
{
    uint8_t receivedChar;
    char inputBuffer[200] = {0}; // To store user inputs
    int inputLen = 0;

    for (;;)
    {
        if (HAL_UART_Receive(&huart2, &receivedChar, 1, HAL_MAX_DELAY) == HAL_OK)
        {
            if (receivedChar != '\r' && receivedChar != '\n')
            {
                inputBuffer[inputLen++] = receivedChar;
                inputBuffer[inputLen] = '\0'; // Null-terminate the string
            }
            else
            {
                // Process the command on Enter key
                switch (inputBuffer[0])
                {
                case 'o':
                    strcpy(elevatorState, "Door opened");
                    UpdateStatusWindow();
                    HAL_Delay(3000);
                    strcpy(elevatorState, "Door closed");
                    UpdateStatusWindow();
                    HAL_Delay(2000);
                    strcpy(elevatorState, "Idle");
                    UpdateStatusWindow();
                    break;
                case 'c':
                    strcpy(elevatorState, "Door closed");
                    UpdateStatusWindow();
                    HAL_Delay(2000);
                    strcpy(elevatorState, "Idle");
                    UpdateStatusWindow();
                    break;
                case '1':
                case '2':
                case '3':
                case '4':
                	uint8_t targetFloor = inputBuffer[0] - '0';
                	while (currentFloor != targetFloor)
					{
						if (currentFloor < targetFloor)
							currentFloor++;
						else if (currentFloor > targetFloor)
							currentFloor--;

						// Update visuals and status for each floor
						strcpy(elevatorState, "Elevator moving");
						UpdateStatusWindow();
						HAL_Delay(1500);
					}

					// After reaching the target floor
					strcpy(elevatorState, "Door opened");
					UpdateStatusWindow();
					HAL_Delay(3000);

					strcpy(elevatorState, "Door closed");
					UpdateStatusWindow();
					HAL_Delay(2000);

					strcpy(elevatorState, "Idle");
					UpdateStatusWindow();
                    break;
                case 'M':
                    strcpy(elevatorState, "Maintenance Mode");
                    UpdateStatusWindow();
                    while (1)
                    {
                        osDelay(100);
                    }
                    break;
                default:
                    break;
                }
                // Clear the input buffer
                inputLen = 0;
                inputBuffer[0] = '\0';
            }

            // Update the input window
            UpdateInputWindow(inputBuffer);
        }
    }
}

void UpdateInputWindow(const char *inputBuffer)
{
    char buffer[300];
    int len = 0;

    // Position the cursor at row 14
    len += snprintf(buffer + len, sizeof(buffer) - len, "\033[14;1H");

    // Display the input prompt
    len += snprintf(buffer + len, sizeof(buffer) - len, "User Input: %s", inputBuffer);

    // Clear the remaining line
    len += snprintf(buffer + len, sizeof(buffer) - len, "\033[K");

    // Transmit the input buffer to the terminal
    HAL_UART_Transmit(&huart2, (uint8_t *)buffer, len, HAL_MAX_DELAY);
}

void UpdateStatusWindow(void)
{
    char buffer[100];
    const char clearCommand[] = "\033[H\033[J"; // Clear screen
    HAL_UART_Transmit(&huart2, (uint8_t *)clearCommand, sizeof(clearCommand) - 1, HAL_MAX_DELAY);

    int len = snprintf(buffer, sizeof(buffer),
                       "Time: %lus\r\nState: %s\r\nFloor: %u\r\n\n********************************\r\n\n",
                       elapsedTime, elevatorState, currentFloor);
    HAL_UART_Transmit(&huart2, (uint8_t *)buffer, len, HAL_MAX_DELAY);

    UpdateElevatorVisuals();

    // Move cursor to the input window row
    const char inputPrompt[] = "\033[14;1H";
    HAL_UART_Transmit(&huart2, (uint8_t *)inputPrompt, sizeof(inputPrompt) - 1, HAL_MAX_DELAY);
}

void UpdateElevatorVisuals(void) {
    char buffer[800];
    int len = 0;

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
