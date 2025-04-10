#include "stm32g071xx.h"
#include <stdint.h>
#ifdef USE_FULL_LL_DRIVER
#include "stm32g0xx_ll_tim.h"
#endif
#include "stm32g071xx.h"

struct timer {
    TIM_TypeDef *timx;
    LL_TIM_InitTypeDef timx_settings;
    uint32_t timx_clk_freq;
    unsigned long apb_clock_reg;
    unsigned long trigger_output;
};

void tim_init (uint32_t out_freq, struct timer *timx);
