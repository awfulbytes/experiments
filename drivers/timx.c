#include "timx.h"
/* #include "src/msp_init.c" */
#include "stm32g0xx_hal_def.h"
#include "stm32g0xx_hal_tim.h"
/* #include "drivers/msp_init.c" */
extern TIM_HandleTypeDef htim6;
extern TIM_MasterConfigTypeDef master_conf;

void tim6_init(TIM_HandleTypeDef* htim_base) {
  if(htim_base->Instance==TIM6) {
    __HAL_RCC_TIM6_CLK_ENABLE();
  }
}

/* void tim6_deinit(TIM_HandleTypeDef* htim_base) { */
/*   if(htim_base->Instance==TIM6) { */
/*     __HAL_RCC_TIM6_CLK_DISABLE(); */
/*   } */
/* } */
void timx_init
(TIM_HandleTypeDef *tima){
    if (tima->Instance == TIM6)
        __HAL_RCC_TIM6_CLK_ENABLE();
}

void timx_deinit
(TIM_HandleTypeDef *tima){
    if (tima->Instance == TIM6)
        __HAL_RCC_TIM6_CLK_ENABLE();
}

HAL_StatusTypeDef tim_init
(uint32_t freq_divider){
  tim_init_status_t stat = {HAL_OK};
  htim6.Instance = TIM6;
  htim6.Init.Prescaler = freq_divider;      //////////////////////////////(uint32_t)(SystemCoreClock / 1000000)
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim6.Init.Period = 0xffff; ////////////////////////////////////////(1000000 / 4095) - 1;
  htim6.Init.ClockDivision = TIM_CLOCKDIVISION_DIV2;
  htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  /* timx_init(&htim6); */
  if (HAL_TIM_Base_Init(&htim6) != stat.tim_ok){
    return stat.tim_err;
  }
  master_conf.MasterOutputTrigger = TIM_TRGO_UPDATE;
  master_conf.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &master_conf) != stat.tim_ok) {
    return stat.tim_err;
  }
  if (HAL_TIM_Base_Start(&htim6) != HAL_OK) {
    return stat.tim_err;
  }
  return stat.tim_ok;
}
