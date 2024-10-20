#include "stm32g0xx_ll_dac.h"
#include "stm32g0xx_hal_dac.h"
#include "stm32g0xx_hal_dma.h"

void dac_init_analog
(DAC_HandleTypeDef, DMA_HandleTypeDef);

void dac_deinit_analog(DAC_HandleTypeDef);
