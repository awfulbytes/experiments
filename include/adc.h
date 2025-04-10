#include "stm32g0xx_ll_dma.h"
#ifndef __INCLUDE_ADC_H__
#include "stm32g071xx.h"
#include "stm32g0xx_ll_bus.h"
#include "stm32g0xx_ll_adc.h"
#include "stm32g0xx_ll_gpio.h"

#include "stm32g0xx_ll_utils.h"
struct adc {
    ADC_TypeDef *adcx;
    LL_ADC_InitTypeDef settings;
    LL_ADC_REG_InitTypeDef reg_settings;
    DMA_TypeDef *dmax;
    LL_DMA_InitTypeDef dmax_settings;
    volatile char roof;
    volatile uint16_t *data;
    unsigned int dma_channel;
    unsigned long channel;
};

void adc_init_settings(struct adc *adc);
void adc_init(void);

#endif
