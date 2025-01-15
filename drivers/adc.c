#include "adc.h"
#include "stm32g071xx.h"
/* #include "stm32g0xx_hal_cortex.h" */
#include "stm32g0xx_ll_adc.h"
#include "stm32g0xx_ll_dma.h"
#include "stm32g0xx_ll_dmamux.h"
#include "stm32g0xx_ll_rcc.h"
#include "stm32g0xx_ll_utils.h"
extern volatile uint16_t adc_value;

uint16_t map_12bit_osc_freq(uint16_t _value) {
    uint16_t min = 100;
    uint16_t max = 800;
    return min + (_value * (max - min)) / 0xfff;
    /* return min + (_value * (max - min)) / (0xfff - min); */
}

void ADC_DMA_Config(struct adc *adc) {
    LL_ADC_InitTypeDef ADC_InitStruct = {0};
    LL_ADC_REG_InitTypeDef ADC_REG_InitStruct = {0};
    LL_DMA_InitTypeDef DMA_InitStruct = {0};

    /* Enable clocks for ADC and DMA */
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_ADC);
    LL_ADC_REG_SetSequencerConfigurable(ADC1, LL_ADC_REG_SEQ_CONFIGURABLE);
    LL_ADC_EnableInternalRegulator(ADC1);
    LL_mDelay(1);
    LL_ADC_StartCalibration(ADC1);
    while (LL_ADC_IsCalibrationOnGoing(ADC1));

    ADC_InitStruct.Clock = LL_ADC_CLOCK_SYNC_PCLK_DIV1;
    ADC_InitStruct.Resolution = LL_ADC_RESOLUTION_12B;
    ADC_InitStruct.DataAlignment = LL_ADC_DATA_ALIGN_RIGHT;
    ADC_InitStruct.LowPowerMode = LL_ADC_LP_MODE_NONE;
    LL_ADC_Init(ADC1, &ADC_InitStruct);

    ADC_REG_InitStruct.TriggerSource = LL_ADC_REG_TRIG_SOFTWARE;
    ADC_REG_InitStruct.SequencerLength = LL_ADC_REG_SEQ_SCAN_DISABLE;
    ADC_REG_InitStruct.SequencerDiscont = LL_ADC_REG_SEQ_DISCONT_DISABLE;
    ADC_REG_InitStruct.ContinuousMode = LL_ADC_REG_CONV_CONTINUOUS;
    ADC_REG_InitStruct.DMATransfer = LL_ADC_REG_DMA_TRANSFER_UNLIMITED;
    ADC_REG_InitStruct.Overrun = LL_ADC_REG_OVR_DATA_OVERWRITTEN;
    LL_ADC_REG_Init(ADC1, &ADC_REG_InitStruct);

    LL_ADC_ClearFlag_CCRDY(ADC1);
    LL_ADC_REG_SetSequencerChAdd(ADC1, LL_ADC_CHANNEL_0);
    while (!LL_ADC_IsActiveFlag_CCRDY(ADC1)) {}
    LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_1, LL_ADC_CHANNEL_0);
    while (!LL_ADC_IsActiveFlag_CCRDY(ADC1)) {}

    LL_ADC_SetChannelSamplingTime(ADC1, LL_ADC_CHANNEL_0,
                                  LL_ADC_SAMPLINGTIME_160CYCLES_5);

    /* Configure DMA for ADC */
    DMA_InitStruct.PeriphRequest = LL_DMAMUX_REQ_ADC1;
    DMA_InitStruct.Direction = LL_DMA_DIRECTION_PERIPH_TO_MEMORY;
    DMA_InitStruct.PeriphOrM2MSrcAddress = (uint32_t) &ADC1->DR;
    DMA_InitStruct.MemoryOrM2MDstAddress = (uint32_t) &adc_value;  // single buffer
    DMA_InitStruct.NbData = 1;  // single transfer
    DMA_InitStruct.Mode = LL_DMA_MODE_CIRCULAR; // keep the transfering
    DMA_InitStruct.PeriphOrM2MSrcIncMode = LL_DMA_PERIPH_NOINCREMENT;
    DMA_InitStruct.MemoryOrM2MDstIncMode = LL_DMA_MEMORY_NOINCREMENT;
    DMA_InitStruct.PeriphOrM2MSrcDataSize = LL_DMA_PDATAALIGN_HALFWORD;
    DMA_InitStruct.MemoryOrM2MDstDataSize = LL_DMA_MDATAALIGN_HALFWORD;
    /* DMA_InitStruct.Priority = 0x03;  // Lower than DAC DMA */
    LL_DMA_Init(DMA1, LL_DMA_CHANNEL_1, &DMA_InitStruct);

    /* NVIC_SetPriority(DMA1_Channel1_IRQn, 0x03); */
    /* NVIC_EnableIRQ(DMA1_Channel1_IRQn); */

    LL_DMA_EnableIT_TE(DMA1, LL_DMA_CHANNEL_1);
    LL_DMA_EnableIT_TC(DMA1, LL_DMA_CHANNEL_1);
    LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_1);
    while (!LL_DMA_IsEnabledChannel(DMA1, LL_DMA_CHANNEL_1)) {}
    LL_ADC_Enable(ADC1);
    while (!LL_ADC_IsActiveFlag_ADRDY(ADC1));
    LL_ADC_ClearFlag_ADRDY(ADC1);

    LL_ADC_REG_StartConversion(ADC1);
}

void Start_ADC_Conversion(void) {
    // Start ADC conversion
    LL_ADC_REG_StartConversion(ADC1);

    // Wait for DMA transfer to complete
    while (!LL_DMA_IsActiveFlag_TC1(DMA1)); // Wait for transfer complete
    LL_DMA_ClearFlag_TC1(DMA1); // Clear transfer complete flag
}
