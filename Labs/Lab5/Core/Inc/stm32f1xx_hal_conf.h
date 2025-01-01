#ifndef __STM32F1xx_HAL_CONF_H
#define __STM32F1xx_HAL_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

/* HAL Module Selection */
#define HAL_MODULE_ENABLED
#define HAL_GPIO_MODULE_ENABLED
#define HAL_TIM_MODULE_ENABLED
#define HAL_UART_MODULE_ENABLED
#define HAL_CORTEX_MODULE_ENABLED
#define HAL_DMA_MODULE_ENABLED
#define HAL_FLASH_MODULE_ENABLED
#define HAL_EXTI_MODULE_ENABLED
#define HAL_PWR_MODULE_ENABLED
#define HAL_RCC_MODULE_ENABLED

/* Oscillator Values */
#define HSE_VALUE    8000000U /*!< External oscillator value in Hz */
#define HSE_STARTUP_TIMEOUT 100U /*!< HSE startup timeout in ms */
#define HSI_VALUE    8000000U /*!< Internal oscillator value in Hz */
#define LSI_VALUE    40000U /*!< LSI oscillator value in Hz */
#define LSE_VALUE    32768U /*!< LSE oscillator value in Hz */
#define LSE_STARTUP_TIMEOUT 5000U /*!< LSE startup timeout in ms */

/* System Configuration */
#define VDD_VALUE 3300U /*!< VDD value in mv */
#define TICK_INT_PRIORITY 15U /*!< Tick interrupt priority */
#define USE_RTOS 0U /*!< RTOS usage */
#define PREFETCH_ENABLE 1U /*!< Enable prefetch */

/* HAL Callback Settings */
#define USE_HAL_UART_REGISTER_CALLBACKS 0U /*!< Disable UART callbacks */
#define USE_HAL_TIM_REGISTER_CALLBACKS 0U /*!< Disable TIM callbacks */

/* Assert Selection */
#define USE_FULL_ASSERT 0U /*!< Disable full assert */

/* Peripheral Includes */
#ifdef HAL_RCC_MODULE_ENABLED
#include "stm32f1xx_hal_rcc.h"
#endif
#ifdef HAL_GPIO_MODULE_ENABLED
#include "stm32f1xx_hal_gpio.h"
#endif
#ifdef HAL_EXTI_MODULE_ENABLED
#include "stm32f1xx_hal_exti.h"
#endif
#ifdef HAL_DMA_MODULE_ENABLED
#include "stm32f1xx_hal_dma.h"
#endif
#ifdef HAL_TIM_MODULE_ENABLED
#include "stm32f1xx_hal_tim.h"
#endif
#ifdef HAL_UART_MODULE_ENABLED
#include "stm32f1xx_hal_uart.h"
#endif

/* Assert Macro */
#ifdef USE_FULL_ASSERT
#define assert_param(expr) ((expr) ? (void)0U : assert_failed((uint8_t *)__FILE__, __LINE__))
void assert_failed(uint8_t *file, uint32_t line);
#else
#define assert_param(expr) ((void)0U)
#endif

#ifdef __cplusplus
}
#endif

#endif /* __STM32F1xx_HAL_CONF_H */
