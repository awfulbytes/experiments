#include "stm32g071xx.h"
#include "dac.h"
#include "stm32g0xx_ll_dac.h"
#include "stm32g0xx_ll_dma.h"
#include "stm32g0xx_ll_bus.h"
struct dma {
    const uint16_t *data;
    uint32_t channel;
    DMA_TypeDef *dmax;
    LL_DMA_InitTypeDef dmax_settings;
    DMA_Channel_TypeDef *chan;
};
ErrorStatus alter_wave_form(const uint16_t *data, struct dma *d);
// DEPRECATED:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
// should be removed there is no need i change the wave properly now!!
ErrorStatus dma_change_wave(const uint16_t *data, struct dma *d, struct dac *dac);
void dma_config(struct dma *dma);
