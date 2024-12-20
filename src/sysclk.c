#include "errors.c"
#include "stm32g0xx_ll_rcc.h"
#include "stm32g0xx_ll_bus.h"
#include "stm32g0xx_ll_system.h"
#include <stdint.h>

typedef struct sysclk {
  uint32_t clk_source;
  uint32_t pllm;
  uint32_t plln;
  uint32_t pllr;
}sysclk_t;

void clk_config_settings(sysclk_t *clk){
  clk->clk_source = LL_RCC_PLLSOURCE_HSI;
  clk->pllm = LL_RCC_PLLM_DIV_5;
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

  /* Enable PLL and wait for it to stabilize */
  LL_RCC_PLL_Enable();
  LL_RCC_PLL_EnableDomain_SYS();
  while (LL_RCC_PLL_IsReady() != 1);

  /* Set PLL as the system clock source */
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);
  while (LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL);

  /* Set the AHB, APB1, and APB2 prescalers to 1 (no division) */
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_2);

  LL_Init1msTick(64000000);
  /* Update the SystemCoreClock variable (required for the LL library to know the system clock frequency) */
  LL_SetSystemCoreClock(64000000);

  /* Configure SysTick for 1 ms interrupts */
}
