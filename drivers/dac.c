#include "dac.h"
#include "stm32g0xx_hal_dac.h"
#include "stm32g0xx_hal_def.h"
#include "stm32g0xx_hal_dma.h"
/* #include "src/errors.c" */

extern DAC_HandleTypeDef hdac1_c1;
extern DMA_HandleTypeDef dmac1;
extern DAC_ChannelConfTypeDef dac_conf;

// TODO:: Configuration issues...
//         i need to check how the other binary is configured
//         and check.. i know that this with pointer passing is not working...
HAL_StatusTypeDef dac_init_analog
(DAC_HandleTypeDef *dac1){
    /* gpio_dac1_c1 = {0}; */
  GPIO_InitTypeDef gpio_dac1_c1 = {0};
  if (dac1->Instance == DAC1) {
    /* __HAL_RCC_DAC1_CLK_ENABLE(); */
    /* __HAL_RCC_GPIOA_CLK_ENABLE(); */

    gpio_dac1_c1.Pin = GPIO_PIN_4;
    gpio_dac1_c1.Mode = GPIO_MODE_ANALOG;
    gpio_dac1_c1.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &gpio_dac1_c1);

    __HAL_RCC_DMA1_CLK_ENABLE();
    dmac1.Instance = DMA1_Channel3;
    dmac1.Init.Request = DMA_REQUEST_DAC1_CH1;
    dmac1.Init.Direction = DMA_MEMORY_TO_PERIPH;

    dmac1.Init.PeriphInc = DMA_PINC_DISABLE;
    dmac1.Init.MemInc = DMA_MINC_ENABLE;

    dmac1.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    dmac1.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;

    dmac1.Init.Mode = DMA_CIRCULAR;
    dmac1.Init.Priority = DMA_PRIORITY_HIGH;

    if (HAL_DMA_Init(&dmac1) != HAL_OK) {
      return HAL_ERROR;
    } else {
      __HAL_LINKDMA(dac1, DMA_Handle1, dmac1);
      HAL_NVIC_SetPriority(DMA1_Channel2_3_IRQn, 0, 0);
      HAL_NVIC_EnableIRQ(DMA1_Channel2_3_IRQn);
      return HAL_OK;
    }
  }
}

HAL_StatusTypeDef dac_init
(void){
  DAC_ChannelConfTypeDef dac_conf = {0};
  hdac1_c1.Instance = DAC1;

  __HAL_RCC_DAC1_CLK_ENABLE();
  if (dac_init_analog(&hdac1_c1) == HAL_OK) {
    dac_conf.DAC_SampleAndHold = DAC_SAMPLEANDHOLD_DISABLE;
    // NOTE::  DAC_TRIGGER_T6_TRGO; i have to see what happens with no trigger
    dac_conf.DAC_Trigger = DAC_TRIGGER_T6_TRGO;
    dac_conf.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
    /* dac_conf.DAC_UserTrimming = DAC_TRIMMING_FACTORY; */
    HAL_DAC_ConfigChannel(&hdac1_c1, &dac_conf, DAC_CHANNEL_1);
    return HAL_OK;
  } else {
    return HAL_ERROR;
  }
}


/* // TODO:: learn not to freeze the periherals */
void dac_deinit_analog(DAC_HandleTypeDef dac1){
    if(dac1.Instance == DAC1){
        __HAL_RCC_DAC1_CLK_DISABLE();
        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_4);
        HAL_DMA_DeInit(dac1.DMA_Handle1);
    }
}
