#include "dac.h"
/* #include "stm32g0xx.h" */
#include "stm32g071xx.h"
#include "stm32g0xx_ll_bus.h"
#include "stm32g0xx_ll_dac.h"
#include "system_stm32g0xx.h"
#include <stdint.h>

#include "gpio.c"

static void dac_chx_priority(struct dac *dac){
    if (dac->channel == LL_DAC_CHANNEL_1){
        NVIC_SetPriority(dac->timx_dac_irq, 0);
        NVIC_EnableIRQ(dac->timx_dac_irq);
    } else {
        NVIC_SetPriority(dac->timx_dac_irq, 1);
        NVIC_EnableIRQ(dac->timx_dac_irq);
    }
}

void dac_config
(struct dac *gdac){
    // HACK:: put the priority different...
    // __XXX__:: i think that there is a race condition
    //           for the timers of the dac (6, 7)
    // I may be able to make this via code for the channels.
    // The other wave does not have any issue... so mayby i
    // have to use the lowest priority for left and higher for
    // right channels????
    dac_chx_priority(gdac);
    LL_APB1_GRP1_EnableClock(gdac->bus_clk_abp);

    LL_DAC_SetTriggerSource(gdac->dacx, gdac->channel, gdac->trg_src);

    LL_DAC_ConfigOutput(gdac->dacx, gdac->channel, gdac->dacx_settings.OutputMode,
                        gdac->dacx_settings.OutputBuffer,
                        gdac->dacx_settings.OutputConnection);
    LL_DAC_EnableDMAReq(gdac->dacx, gdac->channel);

    // TODO:: this should fix the broken wave on channel 1
    if (gdac->channel == LL_DAC_CHANNEL_1)
        LL_DAC_EnableIT_DMAUDR1(gdac->dacx);
    else
        LL_DAC_EnableIT_DMAUDR2(gdac->dacx);
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
