#include "dma.h"
#include "stm32g0xx_ll_bus.h"

static void dma_init(void){
    NVIC_SetPriority(DMA1_Channel2_3_IRQn, 0x00);
    NVIC_EnableIRQ(DMA1_Channel2_3_IRQn);

    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);
}

void dma_config(struct dma *dma){
    dma_init();
    switch (dma->channel) {
        case LL_DMA_CHANNEL_2:
            dma->dmax_settings.PeriphOrM2MSrcAddress = (uint32_t) &DAC1->DHR12RD;
            LL_DMA_EnableIT_HT(dma->dmax, LL_DMA_CHANNEL_2);
            LL_DMA_EnableIT_TC(dma->dmax, LL_DMA_CHANNEL_2);
            break;
        default:
            break;
    }
    dma->dmax_settings.MemoryOrM2MDstAddress = (uint32_t) dma->data;
    LL_DMA_Init(dma->dmax, dma->channel, &dma->dmax_settings);

    LL_DMA_EnableIT_TE(dma->dmax, dma->channel);
    LL_DMA_EnableChannel(dma->dmax, dma->channel);
    while (!LL_DMA_IsEnabledChannel(dma->dmax, dma->channel)) {}
}

// LEGACY:: this can be removed later but is a nice way to change the
//          waveform if i dont succeed
ErrorStatus alter_wave_form(const uint16_t *data, struct dma *dma){
    dma->chan->CCR &= ~DMA_CCR_EN;
    dma->chan->CMAR = (uint32_t) data;
    dma->chan->CPAR = (dma->channel == LL_DMA_CHANNEL_3) ?
        (uint32_t) &DAC1->DHR12R1 : (uint32_t) &DAC1->DHR12R2;
    dma->chan->CNDTR = 256;
    // dma->chan->CCR |= DMA_CCR_MINC | DMA_CCR_CIRC | DMA_CCR_DIR | DMA_CCR_PL_1;
    dma->chan->CCR |= DMA_CCR_EN;
    return SUCCESS;
}
