#include <stdint.h>
#include "stm32g0xx_ll_dac.h"
#include <stdint.h>

struct dac {
    DAC_TypeDef *dacx;
    LL_DAC_InitTypeDef dacx_settings;
    uint32_t channel;
    unsigned long bus_clk_abp;
    unsigned long trg_src;
    int timx_dac_irq;
};


// void dma_config(void);
void dac_config(struct dac *gdac);
void dac_act(struct dac *gdac);

