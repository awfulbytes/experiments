#include "stm32g0xx_it.h"
#include "errors.h"
#include "gpio.h"
#include "stm32g0xx_ll_dma.h"
#include <stdint.h>
/* extern volatile uint16_t adc_value; */
/* extern volatile uint16_t prev_value; */
extern struct button wave_choise_but;
// extern struct adc adcx;
extern struct gpio led;

void HardFault_Handler(void) {
    while (1) {
    }
}
void NMI_Handler(void) {/* while(1); */}
void SVC_Handler(void);
void PendSV_Handler(void);
void SysTick_Handler(void);

void DMA1_Channel1_IRQHandler(void) {
    if (LL_DMA_IsActiveFlag_TE1(DMA1) == SET){
        LL_DMA_ClearFlag_TE1(DMA1);
        while(1) {
            LL_GPIO_TogglePin(led.port_id, led.pin_id);
            LL_mDelay(1200);
            /* break; */
        }
    }
}

void EXTI4_15_IRQHandler(void) {
    if (LL_EXTI_IsActiveRisingFlag_0_31(wave_choise_but.exti.exti_line) == SET) {
        LL_EXTI_ClearRisingFlag_0_31(wave_choise_but.exti.exti_line);
        wave_button_callback(&wave_choise_but);
        wave_choise_but.flag = 'i';
    }
}
