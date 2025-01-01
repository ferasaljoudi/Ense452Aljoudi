#include "main.h"
#include "cmsis_os.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char CLI_Buffer_TX[100];
void printStringBlocking(const char *str);

UART_HandleTypeDef huart2;
osThreadId LED_TaskHandle;
osThreadId cli_receiveHandle;
osThreadId CLI_UpdateHandle;
QueueHandle_t periodQueue;

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
void StartFirstTask(void const *argument);
void StartReceiveTask(void const *argument);
void StartUpdateTask(void const *argument);

int main(void)
{
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_USART2_UART_Init();

  periodQueue = xQueueCreate(1, sizeof(uint32_t));
  if (periodQueue == NULL)
    Error_Handler();

  osThreadDef(LED_Task, StartFirstTask, osPriorityAboveNormal, 0, 128);
  LED_TaskHandle = osThreadCreate(osThread(LED_Task), NULL);

  osThreadDef(cli_receive, StartReceiveTask, osPriorityNormal, 0, 256);
  cli_receiveHandle = osThreadCreate(osThread(cli_receive), NULL);

  osThreadDef(CLI_Update, StartUpdateTask, osPriorityBelowNormal, 0, 128);
  CLI_UpdateHandle = osThreadCreate(osThread(CLI_Update), NULL);

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
    Error_Handler();
  RCC_ClkInitStruct.ClockType =
      RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
    Error_Handler();
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
    Error_Handler();
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

  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
}

void cli_transmit(const char *message)
{
  HAL_UART_Transmit(&huart2, (uint8_t *)message, strlen(message), HAL_MAX_DELAY);
}

#define RX_BUFFER_SIZE 100
char rx_buffer[RX_BUFFER_SIZE];

int cli_receive(char *buffer, uint16_t buffer_size)
{
  uint16_t index = 0;
  uint8_t rx_char;
  while (index < buffer_size - 1)
  {
    if (HAL_UART_Receive(&huart2, &rx_char, 1, HAL_MAX_DELAY) == HAL_OK)
    {
      HAL_UART_Transmit(&huart2, &rx_char, 1, HAL_MAX_DELAY);
      if (rx_char == '\r' || rx_char == '\n')
      {
        buffer[index] = '\0';
        return 1;
      }
      else
      {
        buffer[index++] = rx_char;
      }
    }
    else
    {
      return 0;
    }
  }
  buffer[index] = '\0';
  return 1;
}

void StartFirstTask(void const *argument)
{
  uint32_t blink_period = 100;
  for (;;)
  {
    if (xQueueReceive(periodQueue, &blink_period, 0) == pdPASS)
    {
      cli_transmit("\x1b[33mLED Period Updated\n\r\x1b[0m");
    }
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
    osDelay(blink_period);
  }
}

void StartReceiveTask(void const *argument)
{
  char command_buffer[RX_BUFFER_SIZE];
  uint32_t period = 100;
  for (;;)
  {
    char header_message[50];
    cli_transmit("\x1b[2J");
    cli_transmit("\x1b[1;1H");
    cli_transmit("\x1b[36mENSE452-Lab5-RTOS!\n\r");
    snprintf(header_message, sizeof(header_message), "The current LED rate is %lu ms\n\r", period);
    cli_transmit(header_message);
    cli_transmit("Enter the new rate below:\n\r");
    cli_transmit("********************************************\r\n\n\n\n\x1b[0m");
    cli_transmit("\x1b[8;1r");
    cli_transmit("\x1b[8;1H");
    cli_transmit("> ");
    if (cli_receive(command_buffer, RX_BUFFER_SIZE))
    {
      uint32_t new_period = atoi(command_buffer);
      if (new_period > 0)
      {
        xQueueSend(periodQueue, &new_period, 0);
        period = new_period;
      }
      else
      {
        cli_transmit("\x1b[31mInvalid rate. Please enter a number.\n\r\x1b[0m");
      }
    }
    osDelay(1500);
  }
}

void StartUpdateTask(void const *argument)
{
  uint32_t period = 100;
  char message[50];
  for (;;)
  {
    if (xQueuePeek(periodQueue, &period, 0) == pdPASS)
    {
      snprintf(message, sizeof(message), "\x1b[33mNew LED Rate: %lu ms\n\r\x1b[0m", period);
      cli_transmit(message);
    }
    osDelay(1500);
  }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == TIM4)
    HAL_IncTick();
}

void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
  }
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line) {}
#endif
