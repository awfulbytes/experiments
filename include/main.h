/* #if !defined(STM32G071xx) || !defined(USE_HAL_DRIVER) */
/* #define  STM32G071xx */
/* #define  USE_HAL_DRIVER */
/* #endif */
#ifndef INC_MAIN_H_
#define INC_MAIN_H_

/* void handle_it(void); */
/* void handle_it_enter(void); */

void dma_init(void);
void gpio_init(void);
#endif
