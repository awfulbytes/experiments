#define TIMER TIM6
#include "stm32g0xx_ll_bus.h"
#include "stm32g0xx_ll_dac.h"
#include "stm32g0xx_ll_tim.h"
/* HAL_StatusTypeDef dac_init_analog */
/* (DAC_HandleTypeDef *dac1); */

#define WAVEFORM_SAMPLES_SIZE       (sizeof (WaveformSine_12bits_32samples) / sizeof (uint16_t))
void dma_config(void);
void dac_config(void);
void dac_act(void);
/* HAL_StatusTypeDef dac_init (void); */
