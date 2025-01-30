#include <stdint.h>
#define TIMER TIM6
#include "stm32g0xx_ll_bus.h"
#include "stm32g0xx_ll_dac.h"
#include "stm32g0xx_ll_tim.h"
/* HAL_StatusTypeDef dac_init_analog */
/* (DAC_HandleTypeDef *dac1); */

struct dac {
  DAC_TypeDef *dacx;
  LL_DAC_InitTypeDef dacx_settings;
  uint32_t channel;
};


void dma_config(void);
void dac_ch1_init(struct dac *dacx);
void dac_config(struct dac *gdac);
void dac_act(struct dac *gdac);
/* HAL_StatusTypeDef dac_init (void); */
