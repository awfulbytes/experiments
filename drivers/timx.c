#include "timx.h"
#include "stm32g0xx_ll_rcc.h"
#include "stm32g0xx_ll_bus.h"

static struct timer* timx_set(struct timer *timer) {
    timer->timx_clk_freq = __LL_RCC_CALC_PCLK1_FREQ(SystemCoreClock, LL_RCC_GetAPB1Prescaler());
    timer->timx_settings.Prescaler = 1;
    timer->timx_settings.Autoreload = 0;
    timer->timx_settings.CounterMode = LL_TIM_COUNTERMODE_UP;
    return timer;
}

void tim_init
(uint32_t output_freq, struct timer *tim){
    struct timer *setted = timx_set(tim);

    if (LL_RCC_GetAPB1Prescaler() == LL_RCC_APB1_DIV_2){
        setted->timx_clk_freq *= 2;
    } else {
    }

    uint32_t period = (SystemCoreClock / output_freq) - 1;
    setted->timx_settings.Autoreload = period;
    if (tim->timx == TIM2) {
        LL_APB1_GRP1_EnableClock(setted->apb_clock_reg);

        NVIC_SetPriority(TIM2_IRQn, 0x40);
        NVIC_EnableIRQ(TIM2_IRQn);
        NVIC_SetPriority(TIM6_DAC_LPTIM1_IRQn, 0x00);
        NVIC_EnableIRQ(TIM6_DAC_LPTIM1_IRQn);
        NVIC_SetPriority(TIM7_LPTIM2_IRQn, 0x00);
        NVIC_EnableIRQ(TIM7_LPTIM2_IRQn);
        LL_TIM_Init(tim->timx, &tim->timx_settings);
        LL_TIM_EnableARRPreload(tim->timx);
        LL_TIM_EnableIT_UPDATE(tim->timx);
        LL_TIM_EnableUpdateEvent(tim->timx);
        LL_TIM_SetPrescaler(tim->timx, 0);
        LL_TIM_SetAutoReload(tim->timx, period);
        LL_TIM_SetTriggerOutput(tim->timx, tim->trigger_output);
        LL_TIM_EnableCounter(tim->timx);
        return;
    }
    LL_APB1_GRP1_EnableClock(setted->apb_clock_reg);
    LL_TIM_SetPrescaler(setted->timx, 0);
    LL_TIM_SetAutoReload(setted->timx,  setted->timx_settings.Autoreload);
    LL_TIM_SetCounterMode(setted->timx, setted->timx_settings.CounterMode);
    LL_TIM_SetTriggerOutput(setted->timx, setted->trigger_output);
    LL_TIM_EnableCounter(setted->timx);
    LL_TIM_EnableUpdateEvent(setted->timx);
}
