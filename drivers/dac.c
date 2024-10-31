#include "dac.h"
#include "stm32g0xx_ll_gpio.h"
#include "system_stm32g0xx.h"
#include "stm32g0xx.h"
#include "stm32g0xx_ll_bus.h"
#include "stm32g0xx_ll_dac.h"
#include <stdint.h>
/* #include "stm32g0xx_hal_dac.h" */
/* #include "stm32g0xx_hal_def.h" */
/* #include "stm32g0xx_hal_dma.h" */
/* #include "stm32g0xx_hal_tim.h" */
/* #include "src/errors.c" */

extern uint32_t *square_wave;


void dac_config(void){
  LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);

  /* Configure GPIO in analog mode to be used as DAC output */
  LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_4, LL_GPIO_MODE_ANALOG);

  /*## Configuration of NVIC #################################################*/
  /* Configure NVIC to enable DAC1 interruptions */
  NVIC_SetPriority(TIM6_DAC_LPTIM1_IRQn, 0);
  NVIC_EnableIRQ(TIM6_DAC_LPTIM1_IRQn);

  /*## Configuration of DAC ##################################################*/

  /* Enable DAC clock */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_DAC1);

  /* Set the mode for the selected DAC channel */
  // LL_DAC_SetMode(DAC1, LL_DAC_CHANNEL_1, LL_DAC_MODE_NORMAL_OPERATION);

  /* Select trigger source */
  LL_DAC_SetTriggerSource(DAC1, LL_DAC_CHANNEL_1, LL_DAC_TRIG_EXT_TIM6_TRGO);

  /* Set the output for the selected DAC channel */
  LL_DAC_ConfigOutput(DAC1, LL_DAC_CHANNEL_1, LL_DAC_OUTPUT_MODE_NORMAL, LL_DAC_OUTPUT_BUFFER_ENABLE, LL_DAC_OUTPUT_CONNECT_GPIO);

  /* Note: DAC channel output configuration can also be done using            */
  /*       LL unitary functions:                                              */
  // LL_DAC_SetOutputMode(DAC1, LL_DAC_CHANNEL_1, LL_DAC_OUTPUT_MODE_NORMAL);
  // LL_DAC_SetOutputBuffer(DAC1, LL_DAC_CHANNEL_1, LL_DAC_OUTPUT_BUFFER_ENABLE);
  // LL_DAC_SetOutputConnection(DAC1, LL_DAC_CHANNEL_1, LL_DAC_OUTPUT_CONNECT_GPIO);

  /* Set DAC mode sample-and-hold timings */
  // LL_DAC_SetSampleAndHoldSampleTime (DAC1, LL_DAC_CHANNEL_1, 0x3FF);
  // LL_DAC_SetSampleAndHoldHoldTime   (DAC1, LL_DAC_CHANNEL_1, 0x3FF);
  // LL_DAC_SetSampleAndHoldRefreshTime(DAC1, LL_DAC_CHANNEL_1, 0xFF);

  /* Enable DAC channel DMA request */
  LL_DAC_EnableDMAReq(DAC1, LL_DAC_CHANNEL_1);

  /* Enable interruption DAC channel1 underrun */
  LL_DAC_EnableIT_DMAUDR1(DAC1);

}

void dac_act(void){
  __IO uint32_t wait_idx = 0;
  LL_DAC_Enable(DAC1, LL_DAC_CHANNEL_1);
  wait_idx = (LL_DAC_DELAY_STARTUP_VOLTAGE_SETTLING_US * (SystemCoreClock / 200000)) / 10;
  while (wait_idx != 0) {
    wait_idx--;
  }
  LL_DAC_EnableTrigger(DAC1, LL_DAC_CHANNEL_1);
}

/* extern DAC_HandleTypeDef hdac1_c1; */
/* extern DMA_HandleTypeDef dmac1; */
// NOTE:: This should not be here i think... overrites the
//         value of the channel config while there is no other place
//         needing this... need to check in more depth
/* extern DAC_ChannelConfTypeDef dac_conf; */

// TODO:: Configuration issues...
//         i need to check how the other binary is configured
//         and check.. i know that this with pointer passing is not working...
/* HAL_StatusTypeDef dac_init_analog */
/* (DAC_HandleTypeDef *dac1){ */

/*   if (dac1->Instance == DAC1) { */

/*     __HAL_RCC_DMA1_CLK_ENABLE(); */
/*     dmac1.Instance = DMA1_Channel3; */
/*     dmac1.Init.Request = DMA_REQUEST_DAC1_CH1; */
/*     dmac1.Init.Direction = DMA_MEMORY_TO_PERIPH; */

/*     dmac1.Init.PeriphInc = DMA_PINC_DISABLE; */
/*     dmac1.Init.MemInc = DMA_MINC_ENABLE; */

/*     dmac1.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD; */
/*     dmac1.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD; */

/*     dmac1.Init.Mode = DMA_CIRCULAR; */
/*     dmac1.Init.Priority = DMA_PRIORITY_HIGH; */

/*     // NOTE:: i think i have to make this somehow starting the dma also... */
/*     //         there is nothing indicating that i start the dma before the dac */
/*     //         in main... */
/*     if (HAL_DMA_Init(&dmac1) != HAL_OK) { */
/*       goto error; */
/*     } else { */
/*       __HAL_LINKDMA(dac1, DMA_Handle1, dmac1); */
/*       HAL_NVIC_SetPriority(DMA1_Channel2_3_IRQn, 0, 0); */
/*       HAL_NVIC_EnableIRQ(DMA1_Channel2_3_IRQn); */
/*       return HAL_OK; */
/*     } */
/*   } else { */
/*     goto error; */
/*   } */
/*   error: */
/*   return HAL_ERROR; */
/* } */

/* HAL_StatusTypeDef dac_init */
/* (void){ */
/*   DAC_ChannelConfTypeDef dac_conf = {0}; */
/*   hdac1_c1.Instance = DAC1; */

/*   /\* hdac1_c1. *\/ */
/*   __HAL_RCC_DAC1_CLK_ENABLE(); */
/*   if (dac_init_analog(&hdac1_c1) == HAL_OK) { */
/*     dac_conf.DAC_SampleAndHold = DAC_SAMPLEANDHOLD_DISABLE; */
/*     // NOTE::  DAC_TRIGGER_T6_TRGO; i have to see what happens with no trigger */
/*     dac_conf.DAC_Trigger = DAC_TRIGGER_T6_TRGO; */
/*     dac_conf.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE; */
/*     /\* dac_conf.DAC_ConnectOnChipPeripheral = DAC_CHIPCONNECT_ENABLE; *\/ */
/*     /\* dac_conf.DAC_UserTrimming = DAC_TRIMMING_FACTORY; *\/ */
/*     HAL_DAC_ConfigChannel(&hdac1_c1, &dac_conf, DAC_CHANNEL_1); */
/*     return HAL_OK; */
/*   } else { */
/*     return HAL_ERROR; */
/*   } */
/* } */


/* /\* // TODO:: learn not to freeze the periherals *\/ */
/* void dac_deinit_analog(DAC_HandleTypeDef dac1){ */
/*     if(dac1.Instance == DAC1){ */
/*         __HAL_RCC_DAC1_CLK_DISABLE(); */
/*         HAL_GPIO_DeInit(GPIOA, GPIO_PIN_4); */
/*         HAL_DMA_DeInit(dac1.DMA_Handle1); */
/*     } */
/* } */
