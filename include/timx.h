#include "stm32g0xx_hal.h"
#include <stdint.h>
void tim_init(uint32_t);
void timx_start_clk(TIM_HandleTypeDef);
void timx_stop_clk(TIM_HandleTypeDef);
