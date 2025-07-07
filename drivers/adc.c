#include "adc.h"
#include "stm32g0xx_ll_adc.h"
#include "stm32g0xx_ll_utils.h"
#include "stm32g0xx_ll_bus.h"

extern struct adc adc_settings;
extern volatile uint16_t cv_raw_adc_inp;

static ErrorStatus adc_dma_setup(struct adc *adc);

void adc_init_settings(struct adc *adc){
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_ADC);

    LL_ADC_EnableInternalRegulator(adc->adcx);
    LL_mDelay(1);
    LL_ADC_StartCalibration(adc->adcx);
    while (LL_ADC_IsCalibrationOnGoing(adc->adcx));

    LL_ADC_Init(adc->adcx, &adc->settings);


    LL_ADC_REG_SetSequencerConfigurable(adc->adcx, LL_ADC_REG_SEQ_CONFIGURABLE);
    while (!LL_ADC_IsActiveFlag_CCRDY(adc->adcx)) {}
    LL_ADC_ClearFlag_CCRDY(adc->adcx);

    LL_ADC_REG_SetSequencerChannels(adc->adcx,
                                    LL_ADC_CHANNEL_0 |
                                    LL_ADC_CHANNEL_1 |
                                    LL_ADC_CHANNEL_9);
    while (!LL_ADC_IsActiveFlag_CCRDY(adc->adcx)) {}
    LL_ADC_ClearFlag_CCRDY(adc->adcx);


    LL_ADC_SetSamplingTimeCommonChannels(adc->adcx,
                                         LL_ADC_SAMPLINGTIME_COMMON_1,
                                         LL_ADC_SAMPLINGTIME_12CYCLES_5);

    LL_ADC_SetChannelSamplingTime(adc->adcx,
                                  LL_ADC_CHANNEL_0 |
                                  LL_ADC_CHANNEL_1 |
                                  LL_ADC_CHANNEL_9,
                                  LL_ADC_SAMPLINGTIME_COMMON_1);

    LL_ADC_REG_SetSequencerRanks(adc->adcx, LL_ADC_REG_RANK_1, LL_ADC_CHANNEL_0);
    LL_ADC_REG_SetSequencerRanks(adc->adcx, LL_ADC_REG_RANK_2, LL_ADC_CHANNEL_1);
    LL_ADC_REG_SetSequencerRanks(adc->adcx, LL_ADC_REG_RANK_3, LL_ADC_CHANNEL_9);
    while (!LL_ADC_IsActiveFlag_CCRDY(adc->adcx)) {}
    LL_ADC_ClearFlag_CCRDY(adc->adcx);

    while (LL_ADC_REG_Init(adc->adcx, &adc->reg_settings) != SUCCESS){};
    // LL_ADC_ClearFlag_CCRDY(adc->adcx);
    LL_ADC_Enable(adc->adcx);
    while (adc_dma_setup(adc) &&
           !LL_ADC_IsActiveFlag_ADRDY(adc->adcx));

    LL_ADC_ClearFlag_ADRDY(adc->adcx);

    LL_ADC_REG_StartConversion(adc->adcx);
}

static ErrorStatus adc_dma_setup(struct adc *adc) {

    adc->dmax_settings.PeriphOrM2MSrcAddress = (uint32_t) &adc->adcx->DR;
    adc->dmax_settings.MemoryOrM2MDstAddress = (uint32_t) &cv_raw_adc_inp;

    LL_DMA_Init(adc->dmax, adc->dma_channel, &adc->dmax_settings);

    LL_DMA_EnableIT_TE(adc->dmax, adc->dma_channel);
    LL_DMA_EnableIT_TC(adc->dmax, adc->dma_channel);
    LL_DMA_EnableChannel(adc->dmax, adc->dma_channel);
    while (!LL_DMA_IsEnabledChannel(adc->dmax, adc->dma_channel)) {}

    return SUCCESS;
}
