#include "errors.c"
#include "stm32g0xx_ll_rcc.h"
#include "stm32g0xx_ll_bus.h"
/* extern RCC_OscInitTypeDef rcc_osc; */
/* extern RCC_ClkInitTypeDef rcc_clk; */

void sys_clock_config
(void){
  LL_RCC_HSI_Enable();
  while (LL_RCC_HSI_IsReady() != 1);

  /* Configure PLL */
  LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSI, LL_RCC_PLLM_DIV_4, 70, LL_RCC_PLLR_DIV_5);

  /* Enable PLL and wait for it to stabilize */
  LL_RCC_PLL_Enable();
  LL_RCC_PLL_EnableDomain_SYS();
  while (LL_RCC_PLL_IsReady() != 1);

  /* Set PLL as the system clock source */
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);
  while (LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL);

  /* Set the AHB, APB1, and APB2 prescalers to 1 (no division) */
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);

  LL_Init1msTick(64000000);
  /* Update the SystemCoreClock variable (required for the LL library to know the system clock frequency) */
  LL_SetSystemCoreClock(64000000);

  /* Configure SysTick for 1 ms interrupts */
}
