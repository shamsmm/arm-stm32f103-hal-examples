#include <stdio.h>
#include "main.h"

static GPIO_InitTypeDef  GPIO_InitStruct;

void SystemClock_Config(void);
void GPIO_Config(void);

static int delay;
static unsigned short random;

void ADC1_2_IRQHandler() {
    random = ADC1->DR;
}

int main(void)
{
    HAL_Init();

    SystemClock_Config();

    GPIO_Config();

    __HAL_RCC_ADC1_CLK_ENABLE();
    __HAL_RCC_ADC2_CLK_ENABLE();

    DBGMCU->CR |= DBGMCU_CR_DBG_WWDG_STOP;

    ADC1->CR2 |= ADC_CR2_CONT | ADC_CR2_ADON;
    HAL_Delay(1);
    ADC1->CR2 |= ADC_CR2_ADON;

    ADC1->CR2 |=  ADC_CR2_TSVREFE;

    ADC1->CR2 |= ADC_CR2_CAL;
    while (ADC1->CR2 & ADC_CR2_RSTCAL);

    HAL_NVIC_EnableIRQ(18);
    ADC1->CR1 |=  ADC_CR1_EOCIE /*| ADC_CR1_SCAN*/;
    ADC1->SQR3 |= 16; // Set channel for internal temperature sensor

    HAL_Delay(500);

    // delay should be less than 64.5ms, 63ms is a safe margin since there
    // are functions overhead
    delay = (random % 7 == 0) ? 63 : 100; // the random number has to be multiple of 10 or else
    // delay duration will trigger watchdog to reset

    __HAL_RCC_WWDG_CLK_ENABLE();
    WWDG->CFR = 3U << WWDG_CFR_WDGTB_Pos | 127;
    WWDG->CR = WWDG_CR_WDGA | 127;

    while (1){
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
        HAL_Delay(delay);
        WWDG->CR |= 127;
    }
}

void GPIO_Config(void)
{
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();

    /* Output to MCO */
    MODIFY_REG(RCC->CFGR, RCC_CFGR_MCO, RCC_CFGR_MCO_SYSCLK);

    GPIO_InitStruct.Pin       = GPIO_PIN_8;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);


    GPIO_InitStruct.Pin       = GPIO_PIN_13;
    GPIO_InitStruct.Mode      = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
}

void SystemClock_Config(void)
{
  RCC->CFGR = 3U << RCC_CFGR_ADCPRE_Pos;

  RCC_ClkInitTypeDef clkinitstruct = {0};
  RCC_OscInitTypeDef oscinitstruct = {0};
  
  oscinitstruct.OscillatorType  = RCC_OSCILLATORTYPE_HSE;
  oscinitstruct.HSEState        = RCC_HSE_ON;
  oscinitstruct.LSEState        = RCC_LSE_ON;
  oscinitstruct.HSIState        = RCC_HSI_ON;
  oscinitstruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  oscinitstruct.HSEPredivValue    = RCC_HSE_PREDIV_DIV1;
  oscinitstruct.PLL.PLLState    = RCC_PLL_ON;
  oscinitstruct.PLL.PLLSource   = RCC_PLLSOURCE_HSE;
  oscinitstruct.PLL.PLLMUL      = RCC_PLL_MUL16;
  if (HAL_RCC_OscConfig(&oscinitstruct)!= HAL_OK)
  {
    /* Initialization Error */
    while(1); 
  }

  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
     clocks dividers */
  clkinitstruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  clkinitstruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  clkinitstruct.AHBCLKDivider = RCC_SYSCLK_DIV2;    // 128MHz/2 = 64MHz
  clkinitstruct.APB2CLKDivider = RCC_HCLK_DIV1; // 64MHz
  clkinitstruct.APB1CLKDivider = RCC_HCLK_DIV2;  // 32MHz
  if (HAL_RCC_ClockConfig(&clkinitstruct, FLASH_LATENCY_2)!= HAL_OK)
  {
    /* Initialization Error */
    while(1); 
  }
}

#ifdef  USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{
  printf("Wrong parameters value: file %s on line %lu\r\n", file, line);

  /* Infinite loop */
  while (1)
  {
  }
}
#endif