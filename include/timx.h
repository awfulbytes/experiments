#include "stm32g0xx.h"
/* #include "stm32g0xx_hal_def.h" */
/* #include "stm32g0xx_hal_tim.h" */

#define USE_FULL_LL_DRIVER
#ifdef USE_FULL_LL_DRIVER
#define TIMER TIM6
#include "stm32g0xx_ll_bus.h"
#include "stm32g0xx_ll_tim.h"
#include "system_stm32g0xx.h"
#endif
/* HAL_StatusTypeDef tim6_init(TIM_HandleTypeDef* htim_base); */
/* void tim_init(uint32_t); */
typedef struct timx_init_status {
    ErrorStatus tim_ok;
    ErrorStatus tim_err;
}tim_init_status_t;

/* HAL_StatusTypeDef tim6_init(TIM_HandleTypeDef* htim_base); */
/* HAL_StatusTypeDef tim6_deinit(TIM_HandleTypeDef* htim_base); */
void tim_init (void);

/* void timx_init(TIM_HandleTypeDef *); */
/* void timx_deinit(TIM_HandleTypeDef *); */
