#include "main.h"
#include "timx.h"
#include "stm32g071xx.h"
#include "stm32g0xx_hal_rcc.h"
#include <stdint.h>

extern TIM_HandleTypeDef htim6;

void tim_init
(uint32_t freq_divider){
  TIM_MasterConfigTypeDef master_conf = {0};
  htim6.Instance = TIM6;
  htim6.Init.Prescaler = freq_divider;
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim6.Init.Period = 1000;
  htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim6) != HAL_OK) {
    handle_it_enter();
  }
  master_conf.MasterOutputTrigger = TIM_TRGO_UPDATE;
  master_conf.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &master_conf) != HAL_OK) {
    handle_it();
  }
}

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
