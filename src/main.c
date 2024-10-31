/* #include "include/stm32g0xx_hal_driver/Inc/stm32g0xx.h" */
/* #include <stdio.h> */
/* #include "system_stm32g0xx.c" */
#include "main.h"     // Do i need this to be here or i can put all of them in main.h.... mooooore files more kBits...
#include "dac.h"
#include "timx.h"
#include "sysclk.c"
#include <stddef.h>
#include <stdint.h>


void main() {
  sys_clock_config();
  dma_config();
  tim_init();
  dac_config();
  dac_act();
  while (1) {
  }
}

// TODO:: use the dma to save CPU cycles to make more noise !!
// HACK:: could be used to benefit on processing...
/* void dma_init(void){ */
/*   __HAL_RCC_DMA1_CLK_ENABLE(); */

/*   HAL_NVIC_SetPriority(DMA1_Channel2_3_IRQn, 0, 0); */
/*   HAL_NVIC_EnableIRQ(DMA1_Channel2_3_IRQn); */
/* } */

/* void gpio_init(void){ */
/*   __HAL_RCC_GPIOA_CLK_ENABLE(); */

/*   GPIO_InitTypeDef gpio_dac1_c1 = {0}; */
/*   gpio_dac1_c1.Pin = GPIO_PIN_4; */
/*   gpio_dac1_c1.Mode = GPIO_MODE_ANALOG; */
/*   gpio_dac1_c1.Pull = GPIO_NOPULL; */
/*   HAL_GPIO_Init(GPIOA, &gpio_dac1_c1); */
/* } */

