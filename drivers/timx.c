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
(uint32_t output_freq){
    struct timer *setted = timx_set(&tim6_settings);

    if (LL_RCC_GetAPB1Prescaler() == LL_RCC_APB1_DIV_2){
        setted->timx_clk_freq *= 2;
    } else {
    }

    setted->timx_settings.Autoreload =
        __LL_TIM_CALC_ARR(setted->timx_clk_freq,
                          setted->timx_settings.Prescaler,
                          output_freq * DATA_SIZE(scaled_sin));

    LL_APB1_GRP1_EnableClock(setted->apb_clock_reg);
    LL_TIM_SetPrescaler(setted->timx, setted->timx_settings.Prescaler);
    LL_TIM_SetAutoReload(setted->timx,  setted->timx_settings.Autoreload);
    LL_TIM_SetCounterMode(setted->timx, setted->timx_settings.CounterMode);

    LL_TIM_SetTriggerOutput(setted->timx, setted->trigger_output);
    LL_TIM_EnableCounter(setted->timx);
    LL_TIM_EnableUpdateEvent(setted->timx);
}

static void dma_init
(void){
    NVIC_SetPriority(DMA1_Channel2_3_IRQn, 2); /* DMA IRQ lower priority than DAC IRQ */
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
                          | LL_DMA_PRIORITY_MEDIUM);

    LL_DMA_SetPeriphRequest(DMA1, LL_DMA_CHANNEL_3, LL_DMAMUX_REQ_DAC1_CH1);
    LL_DMA_ConfigAddresses(DMA1, LL_DMA_CHANNEL_3,
                           (uint32_t) &scaled_sin,
                           LL_DAC_DMA_GetRegAddr(DAC1,
                                                 LL_DAC_CHANNEL_1,
                                                 LL_DAC_DMA_REG_DATA_12BITS_RIGHT_ALIGNED),
                           LL_DMA_DIRECTION_MEMORY_TO_PERIPH);

    LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_3, DATA_SIZE(scaled_sin));

    LL_DMA_EnableIT_TE(DMA1, LL_DMA_CHANNEL_3);
    LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_3);
}

ErrorStatus alter_wave_frequency
(const uint16_t output_freq, struct timer *timer){
    timer->timx_settings.Autoreload =
        __LL_TIM_CALC_ARR(timer->timx_clk_freq, timer->timx_settings.Prescaler, output_freq * DATA_SIZE(scaled_sin));
    LL_TIM_SetAutoReload(timer->timx, timer->timx_settings.Autoreload);
    return SUCCESS;
}
ErrorStatus dma_change_wave
(const uint16_t *data){
    LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_3);
    LL_DMA_ConfigAddresses(DMA1, LL_DMA_CHANNEL_3,
                           (uint32_t) data,
                           LL_DAC_DMA_GetRegAddr(DAC1,
                                                 LL_DAC_CHANNEL_1,
                                                 LL_DAC_DMA_REG_DATA_12BITS_RIGHT_ALIGNED),
                           LL_DMA_DIRECTION_MEMORY_TO_PERIPH);
    LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_3);
    return SUCCESS;
}
