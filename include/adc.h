#include "stm32g0xx_ll_dma.h"
#include "stm32g0xx_ll_adc.h"
#ifndef __INCLUDE_ADC_H__
#include "stm32g071xx.h"

struct adc {
    ADC_TypeDef            *adcx;
    LL_ADC_InitTypeDef      settings;
    LL_ADC_REG_InitTypeDef  reg_settings;
    DMA_TypeDef            *dmax;
    LL_DMA_InitTypeDef      dmax_settings;
    volatile uint16_t      *data;
    unsigned int            dma_channel;
    unsigned long           channel;
    volatile char           roof;
};

void adc_init_settings(struct adc *adc);
void adc_init(void);
void enable_oversampling(struct adc *a);

#endif
