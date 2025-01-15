#include "dac.h"
/* #include "stm32g0xx.h" */
#include "stm32g071xx.h"
#include "stm32g0xx_ll_dac.h"
#include "system_stm32g0xx.h"
#include <stdint.h>

#include "gpio.c"

void dac_default_init
(struct dac *dac) {
    dac->dacx = DAC1;
    dac->channel = LL_DAC_CHANNEL_1;
    dac->dacx_settings.OutputMode = LL_DAC_OUTPUT_MODE_NORMAL;
    dac->dacx_settings.OutputBuffer = LL_DAC_OUTPUT_BUFFER_ENABLE;
    dac->dacx_settings.OutputConnection = LL_DAC_OUTPUT_CONNECT_GPIO;
}

void dac_config
(struct dac *gdac){
    NVIC_SetPriority(TIM6_DAC_LPTIM1_IRQn, 0);
    NVIC_EnableIRQ(TIM6_DAC_LPTIM1_IRQn);

    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_DAC1);

    LL_DAC_SetTriggerSource(gdac->dacx, gdac->channel, LL_DAC_TRIG_EXT_TIM6_TRGO);

    LL_DAC_ConfigOutput(gdac->dacx, gdac->channel, gdac->dacx_settings.OutputMode,
                        gdac->dacx_settings.OutputBuffer,
                        gdac->dacx_settings.OutputConnection);
    LL_DAC_EnableDMAReq(gdac->dacx, gdac->channel);
    LL_DAC_EnableIT_DMAUDR1(gdac->dacx);
}

void dac_act
(struct dac *gdac){
    __IO uint32_t wait_idx = 0;
    LL_DAC_Enable(gdac->dacx, gdac->channel);
    wait_idx = (LL_DAC_DELAY_STARTUP_VOLTAGE_SETTLING_US * (SystemCoreClock / 200000)) / 10;
    while (wait_idx != 0) {
        wait_idx--;
    }
    LL_DAC_EnableTrigger(gdac->dacx, gdac->channel);
}
