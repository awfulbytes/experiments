#include "stm32g071xx.h"
#include "stm32g0xx.h"

#ifdef USE_FULL_LL_DRIVER
#define TIMER TIM6
#include "stm32g0xx_ll_bus.h"
#include "stm32g0xx_ll_tim.h"
#include "stm32g0xx_ll_dma.h"
#include "stm32g0xx_ll_dmamux.h"
#include "stm32g0xx_ll_rcc.h"
#include "system_stm32g0xx.h"
#endif

/**
  * A struct to represent all timer nessessary shit.
  *
  * @param *timx          Timer identifier.
  * @param timx_settings Low level settings struct.
  * @param timx_clk_freq Timer clock frequency
  *
  */
struct timer {
  TIM_TypeDef *timx;
  LL_TIM_InitTypeDef timx_settings;
  uint32_t timx_clk_freq;
};


/**
 * @brief Initialize a timer.
 * @param *timer A struct to represent all timer related info.
 * */
struct timer* timx_set(struct timer *timer);

/*
 * One should pass this from the initiator function to initialize default configs...
 * @ref: timer_init_settings
*/
void tim_init (struct timer *setted, uint32_t out_freq, const uint16_t *data);
ErrorStatus dma_change_wave (const uint16_t *data, const uint16_t output_freq, struct timer *timer);
