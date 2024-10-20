/* #include "include/stm32g0xx_hal_driver/Inc/stm32g0xx.h" */
/* #include <stdio.h> */
/* #include "system_stm32g0xx.c" */
#include "main.h"     // Do i need this to be here or i can put all of them in main.h.... mooooore files more kBits...

#include "dac.h"
#include "hal_config.c"
#include "stm32g0xx_hal_dac.h"
#include "stm32g0xx_hal_dma.h"
#include "stm32g0xx_ll_utils.h"
#include "timx.h"
#include "stm32g0xx_nucleo.h"

DAC_HandleTypeDef hdac1_c1;
DAC_ChannelConfTypeDef dac_conf;
TIM_HandleTypeDef htim6;
TIM_MasterConfigTypeDef master_timer;

DMA_HandleTypeDef dmac1;

RCC_OscInitTypeDef rcc_osc = {0};
RCC_ClkInitTypeDef rcc_clk = {0};

void sys_clock_config
(RCC_OscInitTypeDef osc, RCC_ClkInitTypeDef clk);

static void dac_init
(DAC_HandleTypeDef, DAC_ChannelConfTypeDef);

static void tim_init
(uint8_t freq_devider, TIM_HandleTypeDef, TIM_MasterConfigTypeDef);

static void dma_init(void);
static void gpio_init(void);

uint8_t isInterruptEnabled(void)
{
    return (__get_PRIMASK() == 0);  // Returns 1 if interrupts are enabled, 0 if disabled
}

void SysInit(void) {

  HAL_Init();
  hal_config_init();
  sys_clock_config(rcc_osc, rcc_clk);
  dma_init();
  gpio_init();

  /* timx_init(htim6); */
  tim_init(2, htim6, master_timer);
  dac_init(hdac1_c1, dac_conf);
  /* dac_init_analog(hdac1_c1, dmac1); */

  // NOTE:: debug portal
  BSP_LED_Init(LED4);
  /* BSP_PB_Init(BUTTON_USER, BUTTON_MODE_EXTI); */
}

void main() {
  SysInit();
  // NOTE:: debug portal
  /* BSP_PB_Init(BUTTON_USER, BUTTON_MODE_EXTI); */
  HAL_TIM_Base_Start(&htim6);

  /* dac_init_analog(hdac1_c1, dmac1); */
  while (1) {
    if(isInterruptEnabled() == 0){
    /* HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_13); */
      handle_it();
    }else{
      /* HAL_DAC_DeInit(&hdac1_c1); */
      DAC_Ch1_TriangleConfig();
      BSP_LED_Toggle(LED4);
      LL_mDelay(20);
    }
    /* BSP_LED_Off(LED4); */
    /* my_shit = 10; */
    /* HAL_DAC_DeInit(&hdac1_c1); */
  }
}

void sys_clock_config
(RCC_OscInitTypeDef rcc_osc, RCC_ClkInitTypeDef clk){

  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

  rcc_osc.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  rcc_osc.HSIState = RCC_HSI_ON;
  rcc_osc.HSIDiv = RCC_HSI_DIV1;
  rcc_osc.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  rcc_osc.PLL.PLLState = RCC_PLL_ON;
  rcc_osc.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  rcc_osc.PLL.PLLM =RCC_PLLM_DIV4;
  rcc_osc.PLL.PLLN =49;
  rcc_osc.PLL.PLLP =RCC_PLLP_DIV10;
  rcc_osc.PLL.PLLQ =RCC_PLLQ_DIV5;
  rcc_osc.PLL.PLLR =RCC_PLLR_DIV5;
  if (HAL_RCC_OscConfig(&rcc_osc) != HAL_OK) {
    handle_it();
  }

  rcc_clk.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1;
  rcc_clk.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  rcc_clk.AHBCLKDivider = RCC_SYSCLK_DIV1;
  rcc_clk.APB1CLKDivider = RCC_HCLK_DIV1;
  if (HAL_RCC_ClockConfig(&rcc_clk, FLASH_LATENCY_1) != HAL_OK) {
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
(DAC_HandleTypeDef dacx_chanx, DAC_ChannelConfTypeDef dacx_chx_conf){

  dacx_chanx.Instance = DAC1;
  if (HAL_DAC_Init(&dacx_chanx) != HAL_OK) {
    handle_it();
  }
  dacx_chx_conf.DAC_SampleAndHold = DAC_SAMPLEANDHOLD_DISABLE;
  dacx_chx_conf.DAC_Trigger = DAC_TRIGGER_T6_TRGO;
  dacx_chx_conf.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
  dacx_chx_conf.DAC_ConnectOnChipPeripheral = DAC_CHIPCONNECT_DISABLE;
  dacx_chx_conf.DAC_UserTrimming = DAC_TRIMMING_FACTORY;
  if (HAL_DAC_ConfigChannel(&dacx_chanx, &dacx_chx_conf, DAC_CHANNEL_1) != HAL_OK){
    handle_it();
  }
}

static void tim_init
(uint8_t freq_divider, TIM_HandleTypeDef htimx, TIM_MasterConfigTypeDef master_conf){

  htimx.Instance = TIM6;
  htimx.Init.Prescaler = freq_divider;
  htimx.Init.CounterMode = TIM_COUNTERMODE_UP;
  htimx.Init.Period = 0x5ff;
  htimx.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htimx) != HAL_OK) {
    handle_it();
  }

  master_conf.MasterOutputTrigger = TIM_TRGO_UPDATE;
  master_conf.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htimx, &master_conf) != HAL_OK) {
    handle_it();
  }
}

static void gpio_init(void){
  __HAL_RCC_GPIOA_CLK_ENABLE();
}

void handle_it(void){
  while(1)
  {
    BSP_LED_Toggle(LED4);
    LL_mDelay(2000);
    /* HAL_Delay(1000); */
    /* BSP_LED_Off(LED4); */
  }
}

// :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
// FIXME::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
// :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void DAC_Ch1_TriangleConfig(void){
  BSP_LED_Off(LED4);
  /*##-1- Initialize the DAC peripheral ######################################*/
  if (HAL_DAC_Init(&hdac1_c1) != HAL_OK)
  {
    /* DAC initialization Error */
    handle_it();
  }

  /*##-2- DAC channel2 Triangle Wave generation configuration ################*/
  if (HAL_DACEx_TriangleWaveGenerate(&hdac1_c1, DAC_CHANNEL_1, DAC_TRIANGLEAMPLITUDE_4095) != HAL_OK)
  {
    /* Triangle wave generation Error */
    handle_it();
  }

  /*##-3- Enable DAC Channel1 ################################################*/
  if (HAL_DAC_Start(&hdac1_c1, DAC_CHANNEL_1) != HAL_OK)
  {
    /* Start Error */
    handle_it();
  }

  /*##-4- Set DAC channel1 DHR12RD register ################################################*/
  if (HAL_DAC_SetValue(&hdac1_c1, DAC_CHANNEL_1, DAC_ALIGN_12B_R, 0x100) != HAL_OK)
  {
    /* Setting value Error */
    handle_it();
  }
}
