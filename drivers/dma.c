#include "dma.h"
#include "stm32g0xx_ll_dma.h"
#include <stdint.h>
static void dma_init
(void){
    NVIC_SetPriority(DMA1_Channel2_3_IRQn, 2); /* DMA IRQ lower priority than DAC IRQ */
    NVIC_EnableIRQ(DMA1_Channel2_3_IRQn);

    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);
}

void dma_config
(struct dma *dma){
    dma_init();
    switch (dma->channel) {
        case LL_DMA_CHANNEL_3:
            dma->dmax_settings.PeriphOrM2MSrcAddress = (uint32_t) &DAC1->DHR12R1;
            break;
        case LL_DMA_CHANNEL_2:
            dma->dmax_settings.PeriphOrM2MSrcAddress = (uint32_t) &DAC1->DHR12R2;
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
