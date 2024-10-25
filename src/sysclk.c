#include "main.h"

void sys_clock_config
(void){
  RCC_OscInitTypeDef rcc_osc = {0};
  RCC_ClkInitTypeDef rcc_clk = {0};

  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

  rcc_osc.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  rcc_osc.HSIState = RCC_HSI_ON;
  rcc_osc.HSIDiv = RCC_HSI_DIV1;
  rcc_osc.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  rcc_osc.PLL.PLLState = RCC_PLL_ON;
  rcc_osc.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  rcc_osc.PLL.PLLM = RCC_PLLM_DIV4;
  rcc_osc.PLL.PLLN = 70;
  rcc_osc.PLL.PLLP = RCC_PLLP_DIV10;
  rcc_osc.PLL.PLLQ = RCC_PLLQ_DIV5;
  rcc_osc.PLL.PLLR = RCC_PLLR_DIV5;
  if (HAL_RCC_OscConfig(&rcc_osc) != HAL_OK) {
    handle_it_enter();
  }

  rcc_clk.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1;
  rcc_clk.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  rcc_clk.AHBCLKDivider = RCC_SYSCLK_DIV1;
  rcc_clk.APB1CLKDivider = RCC_HCLK_DIV1;
  if (HAL_RCC_ClockConfig(&rcc_clk, FLASH_LATENCY_2) != HAL_OK) {
    handle_it();
  }
}
