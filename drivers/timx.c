#include "timx.h"
#include "stm32g071xx.h"
#include "stm32g0xx.h"
#include "stm32g0xx_ll_rcc.h"
#include "stm32g0xx_ll_tim.h"
#include "system_stm32g0xx.h"
#include "../src/wave.c"
/* #include "stm32g0xx_ll_tim.h" */
#include "stm32g0xx_ll_bus.h"

/* #define WAVEFORM_TIMER_FREQUENCY                (WAVEFORM_FREQUENCY * WAVEFORM_SAMPLES_SIZE) */
#define WAVEFORM_TIMER_PR_MAX_VAL               ((uint32_t)0xFFFF-1)
#define WAVEFORM_TIMER_FREQUENCY_RANGE_MIN      ((uint32_t)    1)

/* uint32_t wave_tim_freq; */
/* wave_t my_wave; */

struct timer timx_set(struct timer *timer) {
  timer->timx = TIM6;
  timer->timx_clk_freq = __LL_RCC_CALC_PCLK1_FREQ(SystemCoreClock, LL_RCC_GetAPB1Prescaler());
  timer->timx_settings.Prescaler = 0;  // __LL_TIM_CALC_PSC(timer->timx_clk_freq, 1000000);
  timer->timx_settings.Autoreload = 0; // __LL_TIM_CALC_ARR(timer->timx_clk_freq, LL_RCC_GetAPB1Prescaler(), 250);
  timer->timx_settings.CounterMode = LL_TIM_COUNTERMODE_UP;
  return *timer;
}

/* uint32_t amp = 3300;
 * uint32_t fre = 120;
 */

timer_settings_t timer_init_settings
(timer_settings_t *settings){
  settings->timx = TIM6;
  settings->tim_clk_freq = __LL_RCC_CALC_PCLK1_FREQ(SystemCoreClock, LL_RCC_GetAPB1Prescaler());
  settings->prescaler = 0;
  settings->tim_reload = 0;
  return *settings;
}

void tim_init
(struct timer *setted, uint32_t output_freq){

  if (LL_RCC_GetAPB1Prescaler() == LL_RCC_APB1_DIV_2){
    setted->timx_clk_freq *= 2;
  } else {
  }

  /* TODO:: Should make a test with gdb and a calculator for these values and then be sure
   *              how are setted inside the setted `struct' */
  setted->timx_settings.Prescaler = __LL_TIM_CALC_PSC(setted->timx_clk_freq, 1000000) + 1;
  setted->timx_settings.Autoreload =
      __LL_TIM_CALC_ARR(setted->timx_clk_freq, setted->timx_settings.Prescaler, output_freq * DATA_SIZE);

  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM6);
  LL_TIM_SetPrescaler(setted->timx, setted->timx_settings.Prescaler - 1);
  LL_TIM_SetAutoReload(setted->timx,  setted->timx_settings.Autoreload - 1);
  LL_TIM_SetCounterMode(setted->timx, setted->timx_settings.CounterMode);

  LL_TIM_SetTriggerOutput(setted->timx, LL_TIM_TRGO_UPDATE);
  LL_TIM_EnableCounter(setted->timx);
}

void dma_init
(void){
  NVIC_SetPriority(DMA1_Channel2_3_IRQn, 1); /* DMA IRQ lower priority than DAC IRQ */
  NVIC_EnableIRQ(DMA1_Channel2_3_IRQn);

  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);
}

void dma_config
(void){
  dma_init();
  LL_DMA_ConfigTransfer(DMA1, LL_DMA_CHANNEL_3,
                        LL_DMA_DIRECTION_MEMORY_TO_PERIPH | LL_DMA_MODE_CIRCULAR
                        | LL_DMA_PERIPH_NOINCREMENT       | LL_DMA_MEMORY_INCREMENT
                        | LL_DMA_PDATAALIGN_HALFWORD      | LL_DMA_MDATAALIGN_HALFWORD
                        | LL_DMA_PRIORITY_HIGH);

  LL_DMA_SetPeriphRequest(DMA1, LL_DMA_CHANNEL_3, LL_DMAMUX_REQ_DAC1_CH1);
  LL_DMA_ConfigAddresses(DMA1, LL_DMA_CHANNEL_3,
                         (uint32_t) &scaled_sin,
                         LL_DAC_DMA_GetRegAddr(DAC1,
                                               LL_DAC_CHANNEL_1,
                                               LL_DAC_DMA_REG_DATA_12BITS_RIGHT_ALIGNED),
                         LL_DMA_DIRECTION_MEMORY_TO_PERIPH);

  LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_3, DATA_SIZE);

  LL_DMA_EnableIT_TE(DMA1, LL_DMA_CHANNEL_3);
  LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_3);
}
