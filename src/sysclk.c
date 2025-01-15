#include "sysclk.h"
#include "stm32g0xx_ll_rcc.h"

void clk_config_settings(sysclk_t *clk){
    clk->clk_source = LL_RCC_PLLSOURCE_HSI;
    clk->pllm = LL_RCC_PLLM_DIV_4;
    clk->plln = 64;
    clk->pllr = LL_RCC_PLLR_DIV_5;
}

sysclk_t my_clk;
void sys_clock_config
(void){
    LL_FLASH_SetLatency(LL_FLASH_LATENCY_0);
    LL_RCC_HSI_Enable();
    while (LL_RCC_HSI_IsReady() != 1);

    clk_config_settings(&my_clk);

    LL_RCC_PLL_ConfigDomain_SYS(my_clk.clk_source, my_clk.pllm, my_clk.plln, my_clk.pllr);

    LL_RCC_PLL_Enable();
    LL_RCC_PLL_EnableDomain_SYS();
    while (LL_RCC_PLL_IsReady() != 1);

    LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);
    while (LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL);

    LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
    LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);

    LL_Init1msTick(56000000);
    LL_SetSystemCoreClock(56000000);
    __enable_irq();
}
