#include "adc.h"
#include "stm32g0xx_ll_adc.h"
extern volatile uint16_t pitch0_value;
extern struct adc pitch0cv_in;
static ErrorStatus adc_dma_setup(struct adc *adc);

uint16_t map_12bit_osc_freq(uint16_t value) {
    uint16_t min = 440;
    uint16_t max = 830;
    return min + (value * (max - min)) / 0xfff;
}

void adc_init_settings(struct adc *adc){
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_ADC);
    LL_ADC_REG_SetSequencerConfigurable(adc->adcx, LL_ADC_REG_SEQ_CONFIGURABLE);
    LL_ADC_EnableInternalRegulator(adc->adcx);
    LL_mDelay(1);
    LL_ADC_StartCalibration(adc->adcx);
    while (LL_ADC_IsCalibrationOnGoing(adc->adcx));

    LL_ADC_Init(adc->adcx, &adc->settings);
    LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_1, adc->channel);
    while (LL_ADC_REG_Init(adc->adcx, &adc->reg_settings) != SUCCESS){};
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
