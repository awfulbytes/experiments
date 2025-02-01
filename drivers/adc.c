#include "adc.h"
#include "stm32g071xx.h"
/* #include "stm32g0xx_hal_cortex.h" */
#include "stm32g0xx.h"
#include "stm32g0xx_ll_adc.h"
#include "stm32g0xx_ll_dma.h"
#include "stm32g0xx_ll_dmamux.h"
#include "stm32g0xx_ll_rcc.h"
#include "stm32g0xx_ll_utils.h"
extern volatile uint16_t pitch0_value;
extern struct adc pitch0cv_in;
static ErrorStatus adc_dma_setup(struct adc *adc);

uint16_t map_12bit_osc_freq(uint16_t _value) {
    uint16_t min = 440;
    uint16_t max = 1760;
    return min + (_value * (max - min)) / 0xfff;
    /* return min + (_value * (max - min)) / (0xfff - min); */
}

void adc_init_settings(struct adc *adc){
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_ADC);
    LL_ADC_REG_SetSequencerConfigurable(adc->adcx, LL_ADC_REG_SEQ_CONFIGURABLE);
    LL_ADC_EnableInternalRegulator(adc->adcx);
    LL_mDelay(1);
    LL_ADC_StartCalibration(adc->adcx);
    while (LL_ADC_IsCalibrationOnGoing(adc->adcx));

    LL_ADC_Init(adc->adcx, &adc->settings);

    LL_ADC_REG_Init(adc->adcx, &adc->reg_settings);
    LL_ADC_ClearFlag_CCRDY(adc->adcx);

    LL_ADC_SetChannelSamplingTime(adc->adcx, adc->channel,
                                  LL_ADC_SAMPLINGTIME_160CYCLES_5);
    LL_ADC_Enable(adc->adcx);
    while (adc_dma_setup(adc) &&
           !LL_ADC_IsActiveFlag_ADRDY(adc->adcx));
    LL_ADC_ClearFlag_ADRDY(adc->adcx);

    LL_ADC_REG_StartConversion(adc->adcx);
}

static ErrorStatus adc_dma_setup(struct adc *adc) {

    adc->dmax_settings.PeriphOrM2MSrcAddress = (uint32_t) &adc->adcx->DR;
    adc->dmax_settings.MemoryOrM2MDstAddress = (uint32_t) adc->data;  // single buffer
      /* DMA_InitStruct.Priority = 0x03;  // Lower than DAC DMA */
    LL_DMA_Init(adc->dmax, adc->dma_channel, &adc->dmax_settings);

    /* NVIC_SetPriority(DMA1_Channel1_IRQn, 0x03); */
    /* NVIC_EnableIRQ(DMA1_Channel1_IRQn); */

    LL_DMA_EnableIT_TE(adc->dmax, adc->dma_channel);
    LL_DMA_EnableIT_TC(adc->dmax, adc->dma_channel);
    LL_DMA_EnableChannel(adc->dmax, adc->dma_channel);
    while (!LL_DMA_IsEnabledChannel(adc->dmax, adc->dma_channel)) {}
    return SUCCESS;
}

void start_adc_conversion(void) {
    // Start ADC conversion
    LL_ADC_REG_StartConversion(pitch0cv_in.adcx);

    // Wait for DMA transfer to complete
    while (!LL_DMA_IsActiveFlag_TC1(DMA1)); // Wait for transfer complete
    LL_DMA_ClearFlag_TC1(DMA1); // Clear transfer complete flag
    pitch0cv_in.roof = 'i';
    LL_mDelay(3); // HACK:: to be removed
}
