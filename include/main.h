#if !defined(STM32G071xx) || !defined(USE_HAL_DRIVER)
#define  STM32G071xx
#define  USE_HAL_DRIVER
#endif

#include "../ventor/cmsis_device_g0/Include/stm32g0xx.h"
#include "../ventor/stm32g0xx_hal_driver/Inc/stm32g0xx_hal.h"
#include "../ventor/stm32g0xx_hal_driver/Inc/stm32g0xx_hal_cortex.h"
// NOTE:: this is very important to configure the chip... otherwise noone knows
//         what the hell is happening
#include "../ventor/stm32g0xx_hal_driver/Inc/stm32g0xx_hal_conf_template.h"
#include "../ventor/stm32g0xx_hal_driver/Inc/stm32g0xx_ll_dma.h"
#include "../ventor/stm32g0xx_hal_driver/Inc/stm32g0xx_ll_dmamux.h"
#include "../ventor/stm32g0xx_hal_driver/Inc/stm32g0xx_hal_dma.h"
#include "../ventor/stm32g0xx_hal_driver/Inc/stm32g0xx_hal_gpio.h"
#include "../ventor/stm32g0xx_hal_driver/Inc/stm32g0xx_hal_tim.h"

void Error_Handler(void);
