#include "dac.h"
/* #include "stm32g0xx.h" */
#include "stm32g071xx.h"
#include "system_stm32g0xx.h"
#include <stdint.h>

#include "gpio.c"


void dac_config
(void){
  gpio_init();
  NVIC_SetPriority(TIM6_DAC_LPTIM1_IRQn, 0);
  NVIC_EnableIRQ(TIM6_DAC_LPTIM1_IRQn);

  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_DAC1);

  /* Set the mode for the selected DAC channel */
  // LL_DAC_SetMode(DAC1, LL_DAC_CHANNEL_1, LL_DAC_MODE_NORMAL_OPERATION);

  LL_DAC_SetTriggerSource(DAC1, LL_DAC_CHANNEL_1, LL_DAC_TRIG_EXT_TIM6_TRGO);

  LL_DAC_ConfigOutput(DAC1, LL_DAC_CHANNEL_1, LL_DAC_OUTPUT_MODE_NORMAL,
                      LL_DAC_OUTPUT_BUFFER_ENABLE, LL_DAC_OUTPUT_CONNECT_GPIO);

  /* NOTE: DAC channel output configuration can also be done using            */
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

void dac_act
(void){
  __IO uint32_t wait_idx = 0;
  LL_DAC_Enable(DAC1, LL_DAC_CHANNEL_1);
  wait_idx = (LL_DAC_DELAY_STARTUP_VOLTAGE_SETTLING_US * (SystemCoreClock / 200000)) / 10;
  while (wait_idx != 0) {
    wait_idx--;
  }
  LL_DAC_EnableTrigger(DAC1, LL_DAC_CHANNEL_1);
}
