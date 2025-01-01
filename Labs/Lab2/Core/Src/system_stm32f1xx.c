#include "stm32f1xx.h"

#if !defined(HSE_VALUE)
#define HSE_VALUE 8000000U
#endif

#if !defined(HSI_VALUE)
#define HSI_VALUE 8000000U
#endif

uint32_t SystemCoreClock = 8000000;
const uint8_t AHBPrescTable[16U] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 6, 7, 8, 9};
const uint8_t APBPrescTable[8U] = {0, 0, 0, 0, 1, 2, 3, 4};

void SystemInit(void)
{
#if defined(STM32F100xE) || defined(STM32F101xE) || defined(STM32F101xG) || defined(STM32F103xE) || defined(STM32F103xG)
#ifdef DATA_IN_ExtSRAM
    SystemInit_ExtMemCtl();
#endif
#endif

#if defined(USER_VECT_TAB_ADDRESS)
    SCB->VTOR = VECT_TAB_BASE_ADDRESS | VECT_TAB_OFFSET;
#endif
}

void SystemCoreClockUpdate(void)
{
    uint32_t tmp = RCC->CFGR & RCC_CFGR_SWS;
    uint32_t pllmull, pllsource;

    switch (tmp)
    {
    case 0x00U:
        SystemCoreClock = HSI_VALUE;
        break;
    case 0x04U:
        SystemCoreClock = HSE_VALUE;
        break;
    case 0x08U:
        pllmull = (RCC->CFGR & RCC_CFGR_PLLMULL) >> 18U;
        pllsource = RCC->CFGR & RCC_CFGR_PLLSRC;
        pllmull += 2U;

        if (pllsource == 0x00U)
        {
            SystemCoreClock = (HSI_VALUE >> 1U) * pllmull;
        }
        else
        {
            if ((RCC->CFGR & RCC_CFGR_PLLXTPRE) != RESET)
            {
                SystemCoreClock = (HSE_VALUE >> 1U) * pllmull;
            }
            else
            {
                SystemCoreClock = HSE_VALUE * pllmull;
            }
        }
        break;
    default:
        SystemCoreClock = HSI_VALUE;
        break;
    }

    tmp = AHBPrescTable[(RCC->CFGR & RCC_CFGR_HPRE) >> 4U];
    SystemCoreClock >>= tmp;
}

#if defined(STM32F100xE) || defined(STM32F101xE) || defined(STM32F101xG) || defined(STM32F103xE) || defined(STM32F103xG)
#ifdef DATA_IN_ExtSRAM
void SystemInit_ExtMemCtl(void)
{
    __IO uint32_t tmpreg;
    RCC->AHBENR = 0x00000114U;
    tmpreg = READ_BIT(RCC->AHBENR, RCC_AHBENR_FSMCEN);
    RCC->APB2ENR = 0x000001E0U;
    tmpreg = READ_BIT(RCC->APB2ENR, RCC_APB2ENR_IOPDEN);

    GPIOD->CRL = 0x44BB44BBU;
    GPIOD->CRH = 0xBBBBBBBBU;
    GPIOE->CRL = 0xB44444BBU;
    GPIOE->CRH = 0xBBBBBBBBU;
    GPIOF->CRL = 0x44BBBBBBU;
    GPIOF->CRH = 0xBBBB4444U;
    GPIOG->CRL = 0x44BBBBBBU;
    GPIOG->CRH = 0x444B4B44U;

    FSMC_Bank1->BTCR[4U] = 0x00001091U;
    FSMC_Bank1->BTCR[5U] = 0x00110212U;
}
#endif
#endif
