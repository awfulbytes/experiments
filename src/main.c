/* #include "include/stm32g0xx_hal_driver/Inc/stm32g0xx.h" */
/* #include <stdio.h> */
/* #include "system_stm32g0xx.c" */
#include "main.h"     // Do i need this to be here or i can put all of them in main.h.... mooooore files more kBits...

#include "dac.h"
/* #include "stm32_hal_legacy.h" */
#include "stm32g0xx_hal.h"
#include "stm32g0xx_hal_dac.h"
#include "hal_config.c"
/* #include "stm32g0xx_hal_dma.h" */
/* #include "stm32g0xx_hal_gpio.h" */
#include "stm32g0xx_hal_rcc.h"
#include "stm32g0xx_hal_tim.h"
#include "stm32g0xx_ll_utils.h"
#include "timx.h"
#include "stm32g0xx_nucleo.h"

DAC_HandleTypeDef hdac1_c1;
TIM_HandleTypeDef htim6;

DMA_HandleTypeDef dmac1;

void DAC_Ch1_TriangleConfig(void);

void sys_clock_config(void);

void dac_init
(void);

void tim_init
(uint8_t freq_devider, TIM_HandleTypeDef);

void dma_init(void);
void gpio_init(void);

void SysInit(void) {
  HAL_Init();
  /* hal_config_init(); */

  sys_clock_config();
  gpio_init();
  /* dma_init(); */
  tim_init(2, htim6);
  dac_init();
  /* timx_init(htim6); */
  HAL_TIM_Base_Start(&htim6);

  BSP_LED_Init(LED4);
}

void main() {
  /* dac_init_analog(hdac1_c1, dmac1); */
  SysInit();
  while (1) {
    /* HAL_Delay(1000); // 1-second delay */
    /* HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_4); */
    /* BSP_LED_Toggle(LED4); */
    /* HAL_Delay(4000); */
    /* LL_mDelay(200); */
    DAC_Ch1_TriangleConfig();
  }
}

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

// TODO:: use the dma to save CPU cycles to make more noise !!
// HACK:: could be used to benefit on processing...
void dma_init(void){
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* HAL_NVIC_SetPriority(DMA1_Channel2_3_IRQn, 0, 0); */
  HAL_NVIC_EnableIRQ(DMA1_Channel2_3_IRQn);
}

void dac_init
(void){
  DAC_ChannelConfTypeDef dac_conf = {0};

  hdac1_c1.Instance = DAC1;
  if (HAL_DAC_Init(&hdac1_c1) != HAL_OK) {
    handle_it();
  }
  dac_conf.DAC_SampleAndHold = DAC_SAMPLEANDHOLD_DISABLE;
  dac_conf.DAC_Trigger = DAC_TRIGGER_T6_TRGO;
  dac_conf.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
  dac_conf.DAC_ConnectOnChipPeripheral = DAC_CHIPCONNECT_DISABLE;
  dac_conf.DAC_UserTrimming = DAC_TRIMMING_FACTORY;
  if (HAL_DAC_ConfigChannel(&hdac1_c1, &dac_conf, DAC_CHANNEL_1) != HAL_OK){
    handle_it();
  }
}

void tim_init
(uint8_t freq_divider, TIM_HandleTypeDef htimx){
  TIM_MasterConfigTypeDef master_conf = {0};
  htimx.Instance = TIM6;
  htimx.Init.Prescaler = 0x00;
  htimx.Init.CounterMode = TIM_COUNTERMODE_UP;
  htimx.Init.Period = 0xff;
  htimx.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htimx) != HAL_OK) {
    handle_it_enter();
  }

  master_conf.MasterOutputTrigger = TIM_TRGO_UPDATE;
  master_conf.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htimx, &master_conf) != HAL_OK) {
    handle_it();
  }
}

void gpio_init(void){
  __HAL_RCC_GPIOA_CLK_ENABLE();
}

void handle_it_enter(void){
  while(1) {
    BSP_LED_Toggle(LED4);
    LL_mDelay(3000);
  }
}

void handle_it(void){
  while(1) {
    BSP_LED_Toggle(LED4);
    LL_mDelay(2000);
  }
}

/*
 *******************************************************************************
 * FIXME::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
 *******************************************************************************
*/
void DAC_Ch1_TriangleConfig(void){
  BSP_LED_Off(LED4);

  /*##-3- Enable DAC Channel1 ################################################*/
  if (HAL_DAC_Start(&hdac1_c1, DAC_CHANNEL_1) != HAL_OK)
  {
    /* Start Error */
    Error_Handler();
  }

  /*##-2- DAC channel2 Triangle Wave generation configuration ################*/
  if (HAL_DACEx_TriangleWaveGenerate(&hdac1_c1, DAC_CHANNEL_1, DAC_TRIANGLEAMPLITUDE_4095) != HAL_OK)
  {
    /* Triangle wave generation Error */
    handle_it();
  }

/*   /\*##-4- Set DAC channel1 DHR12RD register ################################################*\/ */
  /* if (HAL_DAC_SetValue(&hdac1_c1, DAC_CHANNEL_1, DAC_ALIGN_12B_R, 0x1000) != HAL_OK) { */
  /*   /\* Setting value Error *\/ */
  /*   handle_it(); */
  /* } */
}

void Error_Handler(void){
  while(1) {
    BSP_LED_Toggle(LED4);
    LL_mDelay(4000);
  }
}
