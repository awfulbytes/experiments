/* #if !defined(STM32G071xx) || !defined(USE_HAL_DRIVER) */
/* #define  STM32G071xx */
/* #define  USE_HAL_DRIVER */
/* #endif */
#ifndef __MAIN_H
#define __MAIN_H
#include "gpio.h"
#include "dac.h"
#include "forms.h"
#include "stm32g0xx.h"
/* #include "stm32g0xx_ll_gpio.h" */
/* #include "stm32g0xx_ll_utils.h" */
#include "timx.h"

/* Includes ------------------------------------------------------------------*/
/* #include "stm32g0xx_ll_dac.h" */
/* #include "stm32g0xx_ll_rcc.h" */
/* #include "stm32g0xx_ll_bus.h" */
/* #include "stm32g0xx_ll_system.h" */
/* #include "stm32g0xx_ll_exti.h" */
/* #include "stm32g0xx_ll_cortex.h" */
/* #include "stm32g0xx_ll_utils.h" */
/* #include "stm32g0xx_ll_pwr.h" */
/* #include "stm32g0xx_ll_dma.h" */
/* #include "stm32g0xx_ll_gpio.h" */
/* #include "gpio.h" */

#if defined(USE_FULL_ASSERT)
#include "stm32_assert.h"
#endif /* USE_FULL_ASSERT */

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* #include "stm32g0xx_ll_tim.h" */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/**
  * @brief LED4
  */
void WaitForUserButtonPress(struct button *button);
void DacUnderrunError_Callback(void);


/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */
#endif
