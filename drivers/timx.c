#include "timx.h"
#include "stm32g0xx_ll_rcc.h"
#include "stm32g0xx_ll_bus.h"

static struct timer* timx_set(struct timer *t) {
    t->timx_clk_freq             = __LL_RCC_CALC_PCLK1_FREQ(SystemCoreClock,
                                                                LL_RCC_GetAPB1Prescaler());
    t->settings.Prescaler   = 0;
    t->settings.Autoreload  = 0;
    t->settings.CounterMode = LL_TIM_COUNTERMODE_UP;
    return t;
}

static void compute_arr(struct timer *t, uint32_t desired_frequency){
    if (LL_RCC_GetAPB1Prescaler() == LL_RCC_APB1_DIV_2){
        t->timx_clk_freq *= 2;
    }
    t->settings.Autoreload = (SystemCoreClock / desired_frequency) - 1;
}

static void tim_nvic_config(struct irq_info *t_info){
    NVIC_SetPriority(t_info->nvic_id, t_info->priority);
    NVIC_EnableIRQ(t_info->nvic_id);
}

static void tim_reg_config(struct timer *t){
    LL_APB1_GRP1_EnableClock(t->apb_clock_reg);
    LL_TIM_SetPrescaler(t->id, t->settings.Prescaler);
    LL_TIM_SetAutoReload(t->id,  t->settings.Autoreload);
    LL_TIM_SetCounterMode(t->id, t->settings.CounterMode);
    LL_TIM_SetTriggerOutput(t->id, t->trigger_output);
    LL_TIM_EnableARRPreload(t->id);
}

static void counter_and_update_events(TIM_TypeDef *t){
    LL_TIM_EnableCounter(t);
    LL_TIM_EnableUpdateEvent(t);
}

/* bug:: we initialize only the timer 2...?? how is this possible??
 *       but we have all clocks... we could have internally lock them...
 *       we may not have the update interrupts...
 *       this could be the reason i have only tim 2 working interrupt.*/
void tim_init(uint32_t      output_freq,
              struct timer *tim){
    struct timer *setted = timx_set(tim);

    compute_arr(setted, output_freq);

    tim_reg_config(setted);
    tim_nvic_config(&setted->irq_settings);

    if (tim->id == TIM2) {
        /* LL_TIM_Init(setted->id, &setted->settings); */
        LL_TIM_EnableIT_UPDATE(setted->id);
    }
    counter_and_update_events(setted->id);
}
