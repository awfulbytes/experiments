#include "stm32g071xx.h"
#include "stm32g0xx_ll_dac.h"
#include "stm32g0xx_ll_dma.h"
#include "stm32g0xx_ll_bus.h"
struct dma {
    DMA_TypeDef *dmax;
    LL_DMA_InitTypeDef dmax_settings;
    uint32_t channel;
    const uint16_t *data;
};

void dma_config(struct dma *dma);
