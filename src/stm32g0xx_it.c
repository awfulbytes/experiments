#include "stm32g0xx_it.h"
#include "errors.h"
#include "gpio.h"
#include "stm32g071xx.h"
#include "stm32g0xx_ll_dma.h"
#include <stdint.h>
extern struct button wave_choise_dac1;
extern struct button wave_choise_dac2;
extern struct gpio led;

void HardFault_Handler(void) {
    while (1) {
    }
}
void NMI_Handler(void) {/* while(1); */}
void SVC_Handler(void);
void PendSV_Handler(void);
void SysTick_Handler(void);

void DMA1_Channel2_3_IRQHandler(void){
    if (LL_DMA_IsActiveFlag_TE2(DMA1) == SET){
        LL_DMA_ClearFlag_TE2(DMA1);
        while (1) {}
    }
}

void DMA1_Channel1_IRQHandler(void) {
    if (LL_DMA_IsActiveFlag_TE1(DMA1) == SET){
        LL_DMA_ClearFlag_TE1(DMA1);
        while(1) {}
    }
}

static inline void button_handler(struct button *b){
    __disable_irq();
    wave_button_callback(b);
    b->flag = 'i';
    __enable_irq();
}

void EXTI4_15_IRQHandler(void) {
    if (LL_EXTI_IsActiveRisingFlag_0_31(wave_choise_dac1.exti.exti_line) == SET) {
        LL_EXTI_ClearRisingFlag_0_31(wave_choise_dac1.exti.exti_line);
        button_handler(&wave_choise_dac1);
    }
    if (LL_EXTI_IsActiveRisingFlag_0_31(wave_choise_dac2.exti.exti_line) == SET) {
        LL_EXTI_ClearRisingFlag_0_31(wave_choise_dac2.exti.exti_line);
        button_handler(&wave_choise_dac2);
    }
}
