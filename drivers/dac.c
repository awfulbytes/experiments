#include "dac.h"
#include "gpio.c"
#include <stdint.h>
/* #include "stm32g0xx.h" */

static void dac_chx_priority(struct dac *dac){
    if (dac->channel == LL_DAC_CHANNEL_1){
        NVIC_SetPriority(dac->timx_dac_irq, 3);
        NVIC_EnableIRQ(dac->timx_dac_irq);
    } else {
        NVIC_SetPriority(dac->timx_dac_irq, 2);
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
// DDS variables (phase accumulator technique)
extern volatile uint32_t phase_accum;
extern volatile uint32_t phase_inc;

// ----- Function to Update a Section of the DMA Buffer -----
/**
 * @brief Updates a section of the DMA buffer with new waveform data.
 * @param bufferSection Pointer to the beginning of the section in dacBuffer.
 * @param sectionLength Length of this section (number of samples).
 */
void UpdateDacBufferSection(const uint16_t *data, uint16_t *bufferSection, uint16_t sectionLength)
{
    for (uint16_t i = 0; i < sectionLength; i++) {
        // For a TABLE_SIZE of 256, use the top 8 bits of the 32-bit phase accumulator.
        uint32_t index = (phase_accum * 120) >> 24;  // Extract upper 8 bits (0..255)
        index %= 120;
        bufferSection[i] = (uint16_t) data[index];        // Get the sample from your lookup table

        // Advance the phase accumulator by the phase increment.
        phase_accum += phase_inc;
    }
}
