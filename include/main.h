/* #if !defined(STM32G071xx) || !defined(USE_HAL_DRIVER) */
/* #define  STM32G071xx */
/* #define  USE_HAL_DRIVER */
/* #endif */
#ifndef INC_MAIN_H_
#define INC_MAIN_H_

#include "stm32g0xx_hal.h"
#include "stm32g0xx_hal_tim.h"
extern TIM_HandleTypeDef htim6;
void handle_it(void);
void handle_it_enter(void);


void Error_Handler(void);
/* void Error_Handler(void); */
#endif
