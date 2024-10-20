#include "timx.h"
#include "stm32g071xx.h"
#include "stm32g0xx_hal_rcc.h"
#include <stdint.h>

void timx_init
(TIM_HandleTypeDef tima){
    switch ((uintptr_t)tima.Instance) {
        case (uintptr_t)TIM6:
            __HAL_RCC_TIM6_CLK_ENABLE();
            break;
    }
}

void timx_deinit
(TIM_HandleTypeDef tima){
    switch ((uintptr_t)tima.Instance) {
        case (uintptr_t)TIM6:
            __HAL_RCC_TIM6_CLK_DISABLE();
            break;
    }
}
