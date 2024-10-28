/* #include "include/stm32g0xx_hal_driver/Inc/stm32g0xx.h" */
/* #include <stdio.h> */
/* #include "system_stm32g0xx.c" */
#include "main.h"     // Do i need this to be here or i can put all of them in main.h.... mooooore files more kBits...
#include "stm32g0xx_hal.h"
#include "stm32g0xx_hal_dac.h"
#include "stm32g0xx_hal_dac_ex.h"
#include "stm32g0xx_hal_rcc.h"
#include "stm32g0xx_nucleo.h"

/* #include "drivers/dac.c" */
/* #include "drivers/timx.c" */
#include "system_stm32g0xx.h"
#include "timx.h"
#include "dac.h"
#include "hal_config.c"
#include "sysclk.c"
#include <stdint.h>

RCC_OscInitTypeDef rcc_osc;
RCC_ClkInitTypeDef rcc_clk;
DAC_HandleTypeDef hdac1_c1;
TIM_HandleTypeDef htim6;

DMA_HandleTypeDef dmac1;

// NOTE:: These should go in automagicaly from macro expantion...
TIM_MasterConfigTypeDef master_conf = {0};
#define TRIANGLE_RESOLUTION 0xfff               /* Define an appropriate resolution based on your requirement */

static uint32_t triangle_wave[TRIANGLE_RESOLUTION];
#define SQUARE_RESOLUTION 0xfff

static uint32_t square_wave[SQUARE_RESOLUTION];


void sys_clock_config (void);
static uint32_t* Generate_TriangleWave(void);
static uint32_t* Generate_SquareWave(void);

void main() {

  HAL_Init();
  hal_config_init();

  sys_clock_config();
  /* dma_init(); */
  /* dac_init_analog(&hdac1_c1); */
  gpio_init();

  BSP_LED_Init(LED4);
  if ((tim_init(64) || dac_init()) == HAL_ERROR){
    BSP_LED_Toggle(LED4);
    /* HAL_Delay(300); */
    BSP_LED_Toggle(LED4);
    /* HAL_Delay(600); */
    BSP_LED_Toggle(LED4);
    /* HAL_Delay(300); */
  }

  /* HAL_DAC_Start(&hdac1_c1, DAC1_CHANNEL_1); */
  /* if (HAL_DAC_Start(&hdac1_c1, DAC_CHANNEL_1) != HAL_OK) { */
  /*   /\* Start Error *\/ */
  /*   handle_it(); */
  /* } */
  /* Generate_TriangleWave(); */
  Generate_SquareWave();
  // NOTE:: make this more clear... i dont know if this starts something or not..
  //         gdb says yes to this but no to the dma... i need to test the dma handler
  //         and start the appropriate functions... see linked notes for more info..
  if (HAL_DAC_Start_DMA(&hdac1_c1, DAC_CHANNEL_1,
                        square_wave,
                        TRIANGLE_RESOLUTION,
                        DAC_ALIGN_12B_R) == HAL_OK) {
    BSP_LED_Toggle(LED4);
    /* HAL_Delay(600); */
  }
  /* Start_DAC_DMA_TriangleWave(); */
  while (1) {
    if (__HAL_RCC_DMA1_IS_CLK_ENABLED() && __HAL_RCC_DAC1_IS_CLK_ENABLED()) {
      /* DAC_Ch1_TriangleConfig(); */
      BSP_LED_Toggle(LED4);
      HAL_Delay(100);
    }
    /* timx_start_clk(htim6); */
    /* handle_it_enter(); */
    /* BSP_LED_Toggle(LED4); */
    /* HAL_Delay(3000); */
  }
}

// TODO:: use the dma to save CPU cycles to make more noise !!
// HACK:: could be used to benefit on processing...
/* void dma_init(void){ */
/*   __HAL_RCC_DMA1_CLK_ENABLE(); */

/*   HAL_NVIC_SetPriority(DMA1_Channel2_3_IRQn, 0, 0); */
/*   HAL_NVIC_EnableIRQ(DMA1_Channel2_3_IRQn); */
/* } */

void gpio_init(void){
  __HAL_RCC_GPIOA_CLK_ENABLE();
}

static uint32_t* Generate_TriangleWave(void) {
    uint32_t i;
    uint32_t half_res = TRIANGLE_RESOLUTION / 2;
    uint32_t increment = half_res; // Increment per step
    // Rising slope of the triangle
    for (i = 0; i < half_res; i++) {
        triangle_wave[i] = i * increment;
    }
    // Falling slope of the triangle
    for (; i < TRIANGLE_RESOLUTION - 1; i++) {
        triangle_wave[i] = TRIANGLE_RESOLUTION - ((i - half_res) * increment);
    }
   // Ensure the last value is exactly zero to avoid minor rounding errors
    triangle_wave[TRIANGLE_RESOLUTION - 1] = 0;

    return triangle_wave;
}
static uint32_t* Generate_SquareWave(void) {
    uint32_t i;
    uint32_t half_res = SQUARE_RESOLUTION / 2;

    // Fill the first half of the array with the maximum value (1)
    for (i = 0; i < half_res; i++) {
        square_wave[i] = 1; // Representing high state
    }

    // Fill the second half of the array with the minimum value (0)
    for (; i < SQUARE_RESOLUTION - 1; i++) {
        square_wave[i] = 0; // Representing low state
    }

    return square_wave; // Return the pointer to the square wave array
}
