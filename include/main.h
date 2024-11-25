/* #if !defined(STM32G071xx) || !defined(USE_HAL_DRIVER) */
/* #define  STM32G071xx */
/* #define  USE_HAL_DRIVER */
/* #endif */
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
/* #include "stm32g0xx_ll_dac.h" */
/* #include "stm32g0xx_ll_rcc.h" */
/* #include "stm32g0xx_ll_bus.h" */
/* #include "stm32g0xx_ll_system.h" */
#include "stm32g0xx_ll_exti.h"
/* #include "stm32g0xx_ll_cortex.h" */
/* #include "stm32g0xx_ll_utils.h" */
/* #include "stm32g0xx_ll_pwr.h" */
/* #include "stm32g0xx_ll_dma.h" */
#include "stm32g0xx_ll_gpio.h"
/* #include "gpio.h" */

#if defined(USE_FULL_ASSERT)
#include "stm32_assert.h"
#endif /* USE_FULL_ASSERT */

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stm32g0xx_ll_tim.h"

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/**
  * @brief LED4
  */
#define USER_BUTTON_EXTI_LINE                   LL_EXTI_LINE_13
#define USER_BUTTON_EXTI_IRQn                   EXTI4_15_IRQn
#define USER_BUTTON_EXTI_LINE_ENABLE()          LL_EXTI_EnableIT_0_31(USER_BUTTON_EXTI_LINE)
#define USER_BUTTON_EXTI_FALLING_TRIG_ENABLE()  LL_EXTI_EnableFallingTrig_0_31(USER_BUTTON_EXTI_LINE)
#define USER_BUTTON_SYSCFG_SET_EXTI()           do {                                                                     \
                                                  LL_EXTI_SetEXTISource(LL_EXTI_CONFIG_PORTC, LL_EXTI_CONFIG_LINE13);  \
                                                } while(0)
#define USER_BUTTON_IRQHANDLER                  EXTI4_15_IRQHandler


void UserButton_Callback(void);
void DacUnderrunError_Callback(void);


/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define USER_BUTTON_Pin LL_GPIO_PIN_13
#define USER_BUTTON_GPIO_Port GPIOC
#define USER_BUTTON_EXTI_IRQn EXTI4_15_IRQn
#define LED4_Pin LL_GPIO_PIN_5
#define LED4_GPIO_Port GPIOA

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */
#endif
