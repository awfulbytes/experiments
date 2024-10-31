#include "timx.h"
#include "stm32g0xx_ll_bus.h"
#include "stm32g0xx_ll_tim.h"
#include "stm32g0xx_ll_dma.h"
#include "stm32g0xx_ll_dmamux.h"
#include "wave.h"
#include "stm32g0xx_ll_rcc.h"
#include "system_stm32g0xx.h"
/* #include "src/msp_init.c" */
#define WAVEFORM_FREQUENCY          ((uint32_t)200)
/* Size of array containing DAC waveform samples */
#define WAVEFORM_SAMPLES_SIZE       (sizeof (WaveformSine_12bits_32samples) / sizeof (uint16_t))
#define WAVEFORM_TIMER_FREQUENCY                (WAVEFORM_FREQUENCY * WAVEFORM_SAMPLES_SIZE)

#define WAVEFORM_TIMER_PR_MAX_VAL      ((uint32_t)0xFFFF-1)
#define WAVEFORM_TIMER_FREQUENCY_RANGE_MIN      ((uint32_t)    1)
LL_TIM_InitTypeDef htim6;


void tim_init
(void){
  uint32_t tim_clk_freq = __LL_RCC_CALC_PCLK1_FREQ(SystemCoreClock, LL_RCC_GetAPB1Prescaler());
  uint32_t tim_presc = 0;
  uint32_t tim_reload = 0;

  if (LL_RCC_GetAPB1Prescaler() == LL_RCC_APB1_DIV_1){
  } else {
    tim_clk_freq *= 2;
  }

  tim_presc = (tim_clk_freq / (WAVEFORM_TIMER_PR_MAX_VAL * WAVEFORM_TIMER_FREQUENCY_RANGE_MIN)) + 1;
  tim_reload = (tim_clk_freq/ (tim_presc * WAVEFORM_TIMER_FREQUENCY));

  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM6);
  LL_TIM_SetPrescaler(TIMER, tim_presc - 1);
  LL_TIM_SetAutoReload(TIMER,  tim_reload - 1);
  LL_TIM_SetCounterMode(TIMER, LL_TIM_COUNTERMODE_UP);

  LL_TIM_SetTriggerOutput(TIMER, LL_TIM_TRGO_UPDATE);
  LL_TIM_EnableCounter(TIMER);
}

void dma_config(void){
  NVIC_SetPriority(DMA1_Channel2_3_IRQn, 1); /* DMA IRQ lower priority than DAC IRQ */
  NVIC_EnableIRQ(DMA1_Channel2_3_IRQn);

  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);
  LL_DMA_ConfigTransfer(DMA1, LL_DMA_CHANNEL_3,
                        LL_DMA_DIRECTION_MEMORY_TO_PERIPH | LL_DMA_MODE_CIRCULAR
                        | LL_DMA_PERIPH_NOINCREMENT | LL_DMA_MEMORY_INCREMENT
                        | LL_DMA_PERIPH_NOINCREMENT | LL_DMA_PDATAALIGN_HALFWORD
                        | LL_DMA_MDATAALIGN_HALFWORD | LL_DMA_PRIORITY_HIGH );

  LL_DMA_SetPeriphRequest(DMA1, LL_DMA_CHANNEL_3, LL_DMAMUX_REQ_DAC1_CH1);
  LL_DMA_ConfigAddresses(DMA1, LL_DMA_CHANNEL_3, (uint32_t) &WaveformSine_12bits_32samples,
                         LL_DAC_DMA_GetRegAddr(DAC1,
                                               LL_DAC_CHANNEL_1,
                                               LL_DAC_DMA_REG_DATA_12BITS_RIGHT_ALIGNED),
                         LL_DMA_DIRECTION_MEMORY_TO_PERIPH);
  LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_3, WAVEFORM_SAMPLES_SIZE);

  LL_DMA_EnableIT_TE(DMA1, LL_DMA_CHANNEL_3);
  LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_3);
}
