/* #include "include/stm32g0xx_hal_driver/Inc/stm32g0xx.h" */
/* #include <stdio.h> */
/* #include "system_stm32g0xx.c" */
#include "main.h"     // Do i need this to be here or i can put all of them in main.h.... mooooore files more kBits...

#include "dac.h"
#include "timx.h"
#include "hal_config.c"
#include "msp_init.c"
#include "sysclk.c"

// NOTE:: These should go in automagicaly from macro expantion...

DAC_HandleTypeDef hdac1_c1;
TIM_HandleTypeDef htim6;

DMA_HandleTypeDef dmac1;

void sys_clock_config (void);
static void DAC_Ch1_TriangleConfig();

void main() {

  /* dac_init_analog(hdac1_c1, dmac1); */
  HAL_Init();
  /* hal_config_init(); */

  sys_clock_config();
  dac_init();

  tim_init(0);
  /* HAL_TIM_Base_Init(&htim6); */
  BSP_LED_Init(LED4);

  /* HAL_DAC_Start(&hdac1_c1, DAC1_CHANNEL_1); */
  ;

  if ((HAL_DAC_Start(&hdac1_c1, DAC_CHANNEL_1) ||
       HAL_TIM_Base_Start(&htim6)) != HAL_OK) {
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

// TODO:: use the dma to save CPU cycles to make more noise !!
// HACK:: could be used to benefit on processing...
/* void dma_init(void){ */
/*   __HAL_RCC_DMA1_CLK_ENABLE(); */

/*   HAL_NVIC_SetPriority(DMA1_Channel2_3_IRQn, 0, 0); */
/*   HAL_NVIC_EnableIRQ(DMA1_Channel2_3_IRQn); */
/* } */

void gpio_init(void){
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
