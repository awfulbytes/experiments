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
/* HAL_StatusTypeDef tim6_init(TIM_HandleTypeDef* htim_base); */
/* void tim_init(uint32_t); */
typedef struct timx_init_status {
    ErrorStatus tim_ok;
    ErrorStatus tim_err;
}tim_init_status_t;

typedef struct timer_settings {
    TIM_TypeDef *timx;
    uint32_t tim_clk_freq;
    volatile uint32_t prescaler;
    volatile uint32_t tim_reload;
}timer_settings_t;

typedef struct timer {
  TIM_TypeDef *timx;
  LL_TIM_InitTypeDef timx_settings;
  uint32_t timx_clk_freq;
}timer_t;


timer_settings_t timer_init_settings (timer_settings_t *settings);
timer_t timx_init(timer_t *timer);

/*
 * One should pass this from the initiator function to initialize default configs...
 * @ref: timer_init_settings
*/
void tim_init (timer_t *setted, uint32_t out_freq);
