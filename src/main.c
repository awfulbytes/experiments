/* #include "include/stm32g0xx_hal_driver/Inc/stm32g0xx.h" */
/* #include <stdio.h> */
/* #include "system_stm32g0xx.c" */
#include "main.h"     // Do i need this to be here or i can put all of them in main.h.... mooooore files more kBits...

#include "dac.h"
#include "hal_config.c"
#include "msp_init.c"

// NOTE:: These should go in automagicaly from macro expantion...
#include "stm32g0xx_nucleo.h"

DAC_HandleTypeDef hdac1_c1;
TIM_HandleTypeDef htim6;

DMA_HandleTypeDef dmac1;

void sys_clock_config (void);
static void DAC_Ch1_TriangleConfig();
static void dac_init (void);
static void tim_init (uint32_t freq_divider);
static void gpio_init(void);

void main() {

  /* dac_init_analog(hdac1_c1, dmac1); */
  HAL_Init();
  hal_config_init();

  sys_clock_config();
  dac_init();

  /* HAL_DAC_MspInit(&hdac1_c1); */
  /* HAL_TIM_Base_MspInit(&htim6); */
  /* HAL_DAC_Init(&hdac1_c1); */
  tim_init(0);
  /* HAL_TIM_Base_Init(&htim6); */
  BSP_LED_Init(LED4);

  /* HAL_DAC_Start(&hdac1_c1, DAC1_CHANNEL_1); */
  HAL_TIM_Base_Start(&htim6);

  if (HAL_DAC_Start(&hdac1_c1, DAC_CHANNEL_1) != HAL_OK)
  {
    /* Start Error */
    Error_Handler();
  }

  while (1) {
     /* if (__HAL_RCC_DMA1_IS_CLK_ENABLED()) { */
     /*   BSP_LED_Toggle(LED4); */
     /*   HAL_Delay(1000); */
     /* } */
    /* timx_start_clk(htim6); */
    if (__HAL_RCC_DAC1_IS_CLK_ENABLED()){
      /* HAL_DAC_DeInit(&hdac1_c1); */
      DAC_Ch1_TriangleConfig();
      /* handle_it_enter(); */
      /* BSP_LED_Toggle(LED4); */
      /* HAL_Delay(1000); // 1-second delay */
    }
    else {
      BSP_LED_Toggle(LED4);
      HAL_Delay(100);
    }
    /* HAL_Delay(100); // 1-second delay */
    /* HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_4); */
    /* LL_mDelay(200); */
    /* timx_stop_clk(htim6); */
    /* HAL_Delay(3000); */
    /* DAC_Ch1_TriangleConfig(); */
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
/* void dma_init(void){ */
/*   __HAL_RCC_DMA1_CLK_ENABLE(); */

/*   HAL_NVIC_SetPriority(DMA1_Channel2_3_IRQn, 0, 0); */
/*   HAL_NVIC_EnableIRQ(DMA1_Channel2_3_IRQn); */
/* } */

static void dac_init
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

static void tim_init
(uint32_t freq_divider){
  TIM_MasterConfigTypeDef master_conf = {0};
  htim6.Instance = TIM6;
  htim6.Init.Prescaler = freq_divider;
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim6.Init.Period = 1000;
  htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim6) != HAL_OK) {
    handle_it_enter();
  }
  master_conf.MasterOutputTrigger = TIM_TRGO_UPDATE;
  master_conf.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &master_conf) != HAL_OK) {
    handle_it();
  }
}

static void gpio_init(void){
  __HAL_RCC_GPIOA_CLK_ENABLE();
}

void handle_it_enter(void){
  while(1) {
    BSP_LED_Toggle(LED4);
    HAL_Delay(3000);
  }
}

void handle_it(void){
  while(1) {
    BSP_LED_Toggle(LED4);
    HAL_Delay(2000);
  }
}

static void DAC_Ch1_TriangleConfig(void){
  BSP_LED_Off(LED4);

  /*##-3- Enable DAC Channel1 ################################################*/
  /* ##-2- DAC channel2 Triangle Wave generation configuration ################ */
  if (HAL_DACEx_TriangleWaveGenerate(&hdac1_c1, DAC_CHANNEL_1, DAC_TRIANGLEAMPLITUDE_4095) != HAL_OK)
  {
    /* Triangle wave generation Error */
    handle_it();
  }

/*   /\*##-4- Set DAC channel1 DHR12RD register ################################################*\/ */
  /* if (HAL_DAC_SetValue(&hdac1_c1, DAC_CHANNEL_1, DAC_ALIGN_12B_R, 0x100) != HAL_OK) { */
  /*   /\* Setting value Error *\/ */
  /*   handle_it(); */
  /* } */
}

void Error_Handler(void){
  while(1) {
    BSP_LED_Toggle(LED4);
    HAL_Delay(4000);
  }
}
