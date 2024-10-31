#include "stm32g0xx_hal.h"
#include "stm32g0xx_hal_rcc.h"

/*
 * This is for the low level hal to enable clocks.
 * All is used here are __HAL_RCC_XXX_CLK_Enable...
 * TO OVERWRITE THE FUCKING MSP WEAK FUN_XXX_y
 ** @param None
 ** @return None
 */
void hal_config_init (void){
    __HAL_RCC_SYSCFG_CLK_ENABLE();
    __HAL_RCC_PWR_CLK_ENABLE();

    HAL_SYSCFG_StrobeDBattpinsConfig(SYSCFG_CFGR1_UCPD1_STROBE |
                                     SYSCFG_CFGR1_UCPD2_STROBE);
}
