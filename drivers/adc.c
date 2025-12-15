#include "adc.h"
#include "stm32g0xx_ll_utils.h"
#include "stm32g0xx_ll_bus.h"

extern volatile uint16_t cv_raw_adc_inp;

static ErrorStatus adc_dma_setup(struct adc *adc);
static void enable_oversampling(struct adc *a);
static void seq_configuration(struct adc *a);
static void seq_set_channels(struct adc *a, uint32_t channels);
static void seq_enable_ranks(struct adc *a, uint32_t channels);
static void enable_regulator_and_calibrate(struct adc *a, uint32_t clk);
static void dma_trans_config(struct adc *a);
static void config_reg_and_start_converting(struct adc *a);

void adc_init_settings(struct adc *adc){
    uint32_t channels = LL_ADC_CHANNEL_0 | LL_ADC_CHANNEL_1 |
                        LL_ADC_CHANNEL_9 | LL_ADC_CHANNEL_8;
    uint32_t adc_apb2_clk = LL_APB2_GRP1_PERIPH_ADC;

    enable_regulator_and_calibrate(adc, adc_apb2_clk);

    seq_configuration(adc);
    seq_set_channels(adc, channels);
    seq_enable_ranks(adc, channels);

    enable_oversampling(adc);
    dma_trans_config(adc);

    LL_ADC_DisableIT_EOC(adc->adcx);
    LL_ADC_DisableIT_EOS(adc->adcx);

    config_reg_and_start_converting(adc);
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

static void enable_oversampling(struct adc *a){
    LL_ADC_SetOverSamplingScope(a->adcx, LL_ADC_OVS_GRP_REGULAR_CONTINUED);
    LL_ADC_ConfigOverSamplingRatioShift(a->adcx, LL_ADC_OVS_RATIO_16, LL_ADC_OVS_SHIFT_RIGHT_1);
    LL_ADC_SetOverSamplingDiscont(a->adcx, LL_ADC_OVS_REG_CONT);
    LL_ADC_SetTriggerFrequencyMode(a->adcx, LL_ADC_CLOCK_FREQ_MODE_HIGH);
}

static void seq_configuration(struct adc *a){
    LL_ADC_REG_SetSequencerConfigurable(a->adcx, LL_ADC_REG_SEQ_CONFIGURABLE);
    while (!LL_ADC_IsActiveFlag_CCRDY(a->adcx)) {}
    LL_ADC_ClearFlag_CCRDY(a->adcx);
}

static void seq_set_channels(struct adc *a, uint32_t channels){
    LL_ADC_REG_SetSequencerChannels(a->adcx,
                                    channels);
    while (!LL_ADC_IsActiveFlag_CCRDY(a->adcx)) {}
    LL_ADC_ClearFlag_CCRDY(a->adcx);
} 

static void seq_enable_ranks(struct adc *a, uint32_t channels){
    LL_ADC_SetSamplingTimeCommonChannels(a->adcx, LL_ADC_SAMPLINGTIME_COMMON_1,
                                         LL_ADC_SAMPLINGTIME_39CYCLES_5);
    LL_ADC_SetChannelSamplingTime(a->adcx,
                                  channels,
                                  LL_ADC_SAMPLINGTIME_COMMON_1);

    LL_ADC_REG_SetSequencerRanks(a->adcx, LL_ADC_REG_RANK_1, LL_ADC_CHANNEL_0);
    LL_ADC_REG_SetSequencerRanks(a->adcx, LL_ADC_REG_RANK_2, LL_ADC_CHANNEL_1);
    LL_ADC_REG_SetSequencerRanks(a->adcx, LL_ADC_REG_RANK_3, LL_ADC_CHANNEL_9);
    LL_ADC_REG_SetSequencerRanks(a->adcx, LL_ADC_REG_RANK_4, LL_ADC_CHANNEL_8);
    while (!LL_ADC_IsActiveFlag_CCRDY(a->adcx)) {}
    LL_ADC_ClearFlag_CCRDY(a->adcx);
}

static void enable_regulator_and_calibrate(struct adc *a, uint32_t clk){
    LL_APB2_GRP1_EnableClock(clk);
    LL_ADC_EnableInternalRegulator(a->adcx);
    LL_mDelay(1);
    LL_ADC_StartCalibration(a->adcx);
    while (LL_ADC_IsCalibrationOnGoing(a->adcx));

    LL_ADC_Init(a->adcx, &a->settings);
}

static void dma_trans_config(struct adc *a){
    while (adc_dma_setup(a) &&
           !LL_ADC_IsActiveFlag_ADRDY(a->adcx));

    LL_ADC_ClearFlag_ADRDY(a->adcx);
}

static void config_reg_and_start_converting(struct adc *a){
    while (LL_ADC_REG_Init(a->adcx, &a->reg_settings) != SUCCESS){};
    LL_ADC_Enable(a->adcx);

    
    LL_ADC_REG_StartConversion(a->adcx);
}
