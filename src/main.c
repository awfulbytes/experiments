/* #include "include/stm32g0xx_hal_driver/Inc/stm32g0xx.h" */
/* #include <stdio.h> */
/* #include "system_stm32g0xx.c" */
#include "main.h"
#include "dac.h"
#include "timx.h"
#include "sysclk.c"
#include <stddef.h>
#include <stdint.h>


void main() {
  struct timer tim6_settings;
  sys_clock_config();
  tim6_settings = timx_set(&tim6_settings);
  tim_init(&tim6_settings, 250);
  dma_config();
  dac_config();
  dac_act();
  while (1) {
  }
}
