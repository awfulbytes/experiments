#include "stm32g0xx_hal.h"
#include "stm32g0xx_hal_def.h"
#include <stdint.h>
/* HAL_StatusTypeDef tim6_init(TIM_HandleTypeDef* htim_base); */
/* void tim_init(uint32_t); */
typedef struct timx_init_status {
    HAL_StatusTypeDef tim_ok;
    HAL_StatusTypeDef tim_err;
}tim_init_status_t;

void tim6_init(TIM_HandleTypeDef* htim_base);
void tim6_deinit(TIM_HandleTypeDef* htim_base);
HAL_StatusTypeDef tim_init
(uint32_t freq_divider);

void timx_init(TIM_HandleTypeDef *);
void timx_deinit(TIM_HandleTypeDef *);
