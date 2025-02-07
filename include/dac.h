#include <stdint.h>
#define TIMER TIM6
#include "stm32g0xx_ll_bus.h"
#include "stm32g0xx_ll_dac.h"
#include "stm32g0xx_ll_tim.h"
#include "stm32g071xx.h"
#include "stm32g0xx_ll_bus.h"
#include "stm32g0xx_ll_dac.h"
#include "system_stm32g0xx.h"
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

void update_ping_pong_buff(volatile const uint16_t *data, uint16_t *bufferSection, uint16_t sectionLength);
