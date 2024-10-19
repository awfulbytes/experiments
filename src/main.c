/* #include "include/stm32g0xx_hal_driver/Inc/stm32g0xx.h" */
/* #include <stdio.h> */
/* #include "system_stm32g0xx.c" */
#include "main.h"     // Do i need this to be here or i can put all of them in main.h.... mooooore files more kBits...
/* #include "stm32_hal_legacy.h" */
#include "stm32g071xx.h"
#include "stm32g0xx_hal.h"
#include "stm32g0xx_hal_cortex.h"
#include "stm32g0xx_hal_dac.h"
#include "stm32g0xx_hal_def.h"
#include "stm32g0xx_hal_flash.h"
#include "stm32g0xx_hal_gpio.h"
#include "stm32g0xx_hal_pwr_ex.h"
#include "stm32g0xx_hal_rcc.h"
#include "stm32g0xx_hal_tim.h"
#include "stm32g0xx_hal_tim_ex.h"

DAC_HandleTypeDef *hdac1_c1;
DAC_ChannelConfTypeDef *dac_conf;
TIM_HandleTypeDef *htim6;
TIM_MasterConfigTypeDef *master_timer;

RCC_OscInitTypeDef *rcc_osc = {0};
RCC_ClkInitTypeDef *rcc_clk = {0};

void sys_clock_config
(RCC_OscInitTypeDef *osc, RCC_ClkInitTypeDef *clk);
static void dac_init
(DAC_HandleTypeDef *, DAC_ChannelConfTypeDef *);
static void tim_init
(uint8_t freq_devider, TIM_HandleTypeDef *, TIM_MasterConfigTypeDef *);

static void dma_init(void);
static void gpio_init(void);

void SysInit(void) {
  HAL_Init();

  sys_clock_config(rcc_osc, rcc_clk);
  tim_init(6, htim6, master_timer);
  dac_init(hdac1_c1, dac_conf);
  dma_init();
  gpio_init();
}

int main() {
  /* HAL_GPIO_Init(GPIOA, GPIO_PIN_13); */
   SysInit();
  int my_shit = 5;
  while (my_shit < 10) {
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_13);
    /* my_shit = 10; */
  }
  return my_shit;
}

void sys_clock_config
(RCC_OscInitTypeDef *rcc_osc, RCC_ClkInitTypeDef *clk){

  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

  rcc_osc->OscillatorType = RCC_OSCILLATORTYPE_HSI;
  rcc_osc->HSIState = RCC_HSI_ON;
  rcc_osc->HSIDiv = RCC_HSI_DIV1;
  rcc_osc->HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  rcc_osc->PLL.PLLState = RCC_PLL_ON;
  rcc_osc->PLL.PLLSource = RCC_PLLSOURCE_HSI;
  rcc_osc->PLL.PLLM =RCC_PLLM_DIV4;
  rcc_osc->PLL.PLLN =49;
  rcc_osc->PLL.PLLP =RCC_PLLP_DIV10;
  rcc_osc->PLL.PLLQ =RCC_PLLQ_DIV5;
  rcc_osc->PLL.PLLR =RCC_PLLR_DIV5;
  if (HAL_RCC_OscConfig(rcc_osc) != HAL_OK) {
    handle_it();
  }

  rcc_clk->ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1;
  rcc_clk->SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  rcc_clk->AHBCLKDivider = RCC_SYSCLK_DIV1;
  rcc_clk->APB1CLKDivider = RCC_HCLK_DIV1;
  if (HAL_RCC_ClockConfig(rcc_clk, FLASH_LATENCY_1) != HAL_OK) {
    handle_it();
  }
}

// TODO:: use the dma to save CPU cycles to make more noise !!
// HACK:: could be used to benefit on processing...
static void dma_init(void){
  __HAL_RCC_DMA1_CLK_ENABLE();

  HAL_NVIC_SetPriority(DMA1_Channel2_3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel2_3_IRQn);
}

static void dac_init
(DAC_HandleTypeDef *dacx_chanx, DAC_ChannelConfTypeDef *dacx_chx_conf){

  if (dacx_chanx == NULL) {handle_it();}

  dacx_chanx->Instance = DAC1;
  if (HAL_DAC_Init(dacx_chanx) != HAL_OK) {
    handle_it();
  }
  dacx_chx_conf->DAC_SampleAndHold = DAC_SAMPLEANDHOLD_DISABLE;
  dacx_chx_conf->DAC_Trigger = DAC_TRIGGER_T6_TRGO;
  dacx_chx_conf->DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
  dacx_chx_conf->DAC_ConnectOnChipPeripheral = DAC_CHIPCONNECT_DISABLE;
  dacx_chx_conf->DAC_UserTrimming = DAC_TRIMMING_FACTORY;
  if (HAL_DAC_ConfigChannel(dacx_chanx, dacx_chx_conf, DAC_CHANNEL_1) != HAL_OK){
    handle_it();
  }
}

static void tim_init
(uint8_t freq_divider, TIM_HandleTypeDef *htimx, TIM_MasterConfigTypeDef *master_conf){

  htimx->Instance = TIM6;
  htimx->Init.Prescaler = 0x0000;
  htimx->Init.CounterMode = TIM_COUNTERMODE_UP;
  htimx->Init.Period = 0x5ff;
  htimx->Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(htimx) != HAL_OK) {
    handle_it();
  }

  master_conf->MasterOutputTrigger = TIM_TRGO_UPDATE;
  master_conf->MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(htimx, master_conf) != HAL_OK) {
    handle_it();
  }
}

static void gpio_init(void){
  __HAL_RCC_GPIOA_CLK_ENABLE();
}

void handle_it(void){
  while (1) {
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_13);
  }
}
