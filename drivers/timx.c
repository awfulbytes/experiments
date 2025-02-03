#include "timx.h"
#include "stm32g071xx.h"
#include "stm32g0xx.h"
#include "stm32g0xx_ll_dac.h"
#include "stm32g0xx_ll_dma.h"
#include "stm32g0xx_ll_rcc.h"
#include "stm32g0xx_ll_tim.h"
#include "system_stm32g0xx.h"
#include "wave.h"
/* #include "stm32g0xx_ll_tim.h" */
#include "stm32g0xx_ll_bus.h"
#include <stdint.h>
extern struct timer tim6_settings;

/* #define WAVEFORM_TIMER_FREQUENCY                (WAVEFORM_FREQUENCY * WAVEFORM_SAMPLES_SIZE) */
#define WAVEFORM_TIMER_PR_MAX_VAL               ((uint32_t)0xFFFF-1)
#define WAVEFORM_TIMER_FREQUENCY_RANGE_MIN      ((uint32_t)    1)

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
        NVIC_SetPriority(TIM2_IRQn, 0);
        NVIC_EnableIRQ(TIM2_IRQn);
        LL_TIM_Init(tim->timx, &tim->timx_settings);
        LL_TIM_EnableARRPreload(tim->timx);
        LL_TIM_EnableIT_UPDATE(tim->timx);
        LL_TIM_EnableUpdateEvent(tim->timx);
        LL_TIM_SetPrescaler(tim->timx, 0);
        LL_TIM_SetAutoReload(tim->timx, period);
        LL_TIM_SetTriggerOutput(tim->timx, tim->trigger_output); // TRGO on update event
        LL_TIM_EnableCounter(tim->timx);
        return;
    } else{
        // setted->timx_settings.Autoreload =
        //     __LL_TIM_CALC_ARR(setted->timx_clk_freq,
        //                       setted->timx_settings.Prescaler,
        //                       output_freq * DATA_SIZE(scaled_sin));
    }
    LL_APB1_GRP1_EnableClock(setted->apb_clock_reg);
    LL_TIM_SetPrescaler(setted->timx, setted->timx_settings.Prescaler);
    LL_TIM_SetAutoReload(setted->timx,  setted->timx_settings.Autoreload);
    LL_TIM_SetCounterMode(setted->timx, setted->timx_settings.CounterMode);

    LL_TIM_SetTriggerOutput(setted->timx, setted->trigger_output);
    LL_TIM_EnableCounter(setted->timx);
    LL_TIM_EnableUpdateEvent(setted->timx);
}

ErrorStatus alter_wave_frequency
(const uint16_t output_freq, struct timer *timer){
    timer->timx_settings.Autoreload =
        __LL_TIM_CALC_ARR(timer->timx_clk_freq, timer->timx_settings.Prescaler, output_freq * DATA_SIZE(scaled_sin) * 2);
    LL_TIM_SetAutoReload(timer->timx, timer->timx_settings.Autoreload);
    return SUCCESS;
}
