#include <stdio.h>
#include "main.h"

static GPIO_InitTypeDef  GPIO_InitStruct;
I2C_HandleTypeDef I2C1Handle;
I2C_HandleTypeDef I2C2Handle;

#define I2C_7Bit_SLAVE_ADDRESS 0x3C

void SystemClock_Config(void);
int main(void)
{
    HAL_Init();

    SystemClock_Config();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_I2C1_CLK_ENABLE();
    __HAL_RCC_I2C2_CLK_ENABLE();

    /* Output to MCO */
//    MODIFY_REG(RCC->CFGR, RCC_CFGR_MCO, RCC_CFGR_MCO_SYSCLK);

    GPIO_InitStruct.Pin       = GPIO_PIN_8;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin       = GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_10 | GPIO_PIN_11;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);


    GPIO_InitStruct.Pin       = GPIO_PIN_13;
    GPIO_InitStruct.Mode      = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);

    I2C1Handle.Instance = I2C1;
    I2C1Handle.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    I2C1Handle.Init.OwnAddress1 = I2C_7Bit_SLAVE_ADDRESS; // Slave Address
    I2C1Handle.Init.ClockSpeed = 100000;
    I2C1Handle.Init.DutyCycle = I2C_DUTYCYCLE_2;
    HAL_I2C_Init(&I2C1Handle);

    I2C2Handle.Instance = I2C2;
    I2C2Handle.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    I2C2Handle.Init.ClockSpeed = 100000;
    I2C2Handle.Init.DutyCycle = I2C_DUTYCYCLE_2;
    HAL_I2C_Init(&I2C2Handle);


    while (HAL_I2C_Init(&I2C1Handle) != HAL_OK || HAL_I2C_Init(&I2C2Handle));

    uint8_t rx_buffer[10];
    uint8_t *tx_buffer = "A";

    while (1)
    {
        HAL_I2C_Slave_Receive_IT(&I2C1Handle, (uint8_t *)rx_buffer, 1);

        HAL_I2C_Master_Transmit(&I2C2Handle, I2C_7Bit_SLAVE_ADDRESS << 1, tx_buffer, 1, HAL_MAX_DELAY);

        HAL_Delay(200);
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
    }
}

void SystemClock_Config(void)
{
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
  clkinitstruct.AHBCLKDivider = RCC_SYSCLK_DIV2;
  clkinitstruct.APB2CLKDivider = RCC_HCLK_DIV1;
  clkinitstruct.APB1CLKDivider = RCC_HCLK_DIV2;  
  if (HAL_RCC_ClockConfig(&clkinitstruct, FLASH_LATENCY_2)!= HAL_OK)
  {
    /* Initialization Error */
    while(1); 
  }
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  printf("Wrong parameters value: file %s on line %lu\r\n", file, line);

  /* Infinite loop */
  while (1)
  {
  }
}
#endif