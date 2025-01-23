#include "adc.h"
#include "stm32g071xx.h"
/* #include "stm32g0xx_hal_cortex.h" */
#include "stm32g0xx.h"
#include "stm32g0xx_ll_adc.h"
#include "stm32g0xx_ll_dma.h"
#include "stm32g0xx_ll_dmamux.h"
#include "stm32g0xx_ll_rcc.h"
#include "stm32g0xx_ll_utils.h"
extern volatile uint16_t adc_value;
extern struct adc adcx;
static ErrorStatus adc_dma_setup(struct adc *adc);

uint16_t map_12bit_osc_freq(uint16_t _value) {
    uint16_t min = 109;
    uint16_t max = 414;
    return min + (_value * (max - min)) / 0xfff;
    /* return min + (_value * (max - min)) / (0xfff - min); */
}

void adc_init_settings(struct adc *adc){
    adc->adcx = ADC1;
    adc->channel = LL_ADC_CHANNEL_0;
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_ADC);
    LL_ADC_REG_SetSequencerConfigurable(ADC1, LL_ADC_REG_SEQ_CONFIGURABLE);
    LL_ADC_EnableInternalRegulator(adc->adcx);
    LL_mDelay(1);
    LL_ADC_StartCalibration(adc->adcx);
    while (LL_ADC_IsCalibrationOnGoing(adc->adcx));

    adc->settings.Clock = LL_ADC_CLOCK_SYNC_PCLK_DIV1;
    adc->settings.Resolution = LL_ADC_RESOLUTION_12B;
    adc->settings.DataAlignment = LL_ADC_DATA_ALIGN_RIGHT;
    adc->settings.LowPowerMode = LL_ADC_LP_MODE_NONE;
    LL_ADC_Init(adc->adcx, &adc->settings);

    adc->reg_settings.TriggerSource = LL_ADC_REG_TRIG_SOFTWARE;
    adc->reg_settings.ContinuousMode = LL_ADC_REG_CONV_CONTINUOUS;
    adc->reg_settings.SequencerLength = LL_ADC_REG_SEQ_SCAN_DISABLE;
    adc->reg_settings.SequencerDiscont = LL_ADC_REG_SEQ_DISCONT_DISABLE;
    adc->reg_settings.DMATransfer = LL_ADC_REG_DMA_TRANSFER_UNLIMITED;
    adc->reg_settings.Overrun = LL_ADC_REG_OVR_DATA_OVERWRITTEN;

    LL_ADC_REG_Init(adc->adcx, &adc->reg_settings);

    LL_ADC_ClearFlag_CCRDY(adc->adcx);
    LL_ADC_REG_SetSequencerChAdd(adc->adcx, adc->channel);
    while (!LL_ADC_IsActiveFlag_CCRDY(adc->adcx)) {}
    LL_ADC_REG_SetSequencerRanks(adc->adcx, LL_ADC_REG_RANK_1, adc->channel);
    while (!LL_ADC_IsActiveFlag_CCRDY(adc->adcx)) {}

    LL_ADC_SetChannelSamplingTime(adc->adcx, adc->channel,
                                  LL_ADC_SAMPLINGTIME_39CYCLES_5);
    LL_ADC_Enable(adc->adcx);
    while (adc_dma_setup(adc) &&
           !LL_ADC_IsActiveFlag_ADRDY(adc->adcx));
    LL_ADC_ClearFlag_ADRDY(adc->adcx);

    LL_ADC_REG_StartConversion(adc->adcx);
}

static ErrorStatus adc_dma_setup(struct adc *adc) {
    adc->dmax = DMA1;

    adc->dmax_settings.PeriphRequest = LL_DMAMUX_REQ_ADC1;
    adc->dmax_settings.Direction = LL_DMA_DIRECTION_PERIPH_TO_MEMORY;
    adc->dmax_settings.PeriphOrM2MSrcAddress = (uint32_t) &adc->adcx->DR;
    adc->dmax_settings.MemoryOrM2MDstAddress = (uint32_t) &adc_value;  // single buffer
    adc->dmax_settings.NbData = 1;  // single transfer
    adc->dmax_settings.Mode = LL_DMA_MODE_CIRCULAR; // keep the transfering
    adc->dmax_settings.PeriphOrM2MSrcIncMode = LL_DMA_PERIPH_NOINCREMENT;
    adc->dmax_settings.MemoryOrM2MDstIncMode = LL_DMA_MEMORY_NOINCREMENT;
    adc->dmax_settings.PeriphOrM2MSrcDataSize = LL_DMA_PDATAALIGN_HALFWORD;
    adc->dmax_settings.MemoryOrM2MDstDataSize = LL_DMA_MDATAALIGN_HALFWORD;
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
    LL_ADC_REG_StartConversion(adcx.adcx);

    // Wait for DMA transfer to complete
    while (!LL_DMA_IsActiveFlag_TC1(DMA1)); // Wait for transfer complete
    LL_DMA_ClearFlag_TC1(DMA1); // Clear transfer complete flag
    adcx.roof = 'i';
    LL_mDelay(3); // HACK:: to be removed
}
