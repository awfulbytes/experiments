#ifndef __INCLUDE_ADC_H__
#include "stm32g071xx.h"
#include "stm32g0xx_ll_bus.h"
#include "stm32g0xx_ll_adc.h"
#include "stm32g0xx_ll_gpio.h"

struct adc {
    volatile uint16_t *data;
    volatile char roof;
    ADC_TypeDef *adcx;
    LL_ADC_InitTypeDef *adcx_settings;
    volatile uint32_t dbg_ctr;
};

void ADC_DMA_Config(struct adc *adc);
void Start_ADC_Conversion(void);
void adc_init(void);
uint16_t map_12bit_osc_freq(uint16_t adc_value);

#endif
