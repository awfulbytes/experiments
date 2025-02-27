#include "stm32g0xx_it.h"
#include "errors.h"
// #include "dac.h"
#include "dma.h"
#include "nco.h"
#include "stm32g071xx.h"
#include "stm32g0xx_ll_dma.h"
#include "stm32g0xx_ll_gpio.h"
#include "stm32g0xx_ll_tim.h"
#include <stdint.h>
// #define DEBUG
extern volatile uint64_t phase_inc;
extern volatile uint16_t prev_value;
extern volatile uint16_t pitch0_value;
extern volatile bool phase_pending_update;
extern volatile uint64_t phase_pending_update_inc;
extern volatile const uint16_t *wave_me_d, *wave_me_d2;
extern uint16_t dac_double_buff[256], dac_double_buff2[256];
// extern struct dma dac_1_dma;
extern struct button wave_choise_dac1;
extern struct button wave_choise_dac2;
// extern struct gpio led;
extern struct adc pitch0cv_in;

void HardFault_Handler(void){while(1);}
void NMI_Handler(void){}
void SVC_Handler(void);
void PendSV_Handler(void);
void SysTick_Handler(void);

void TIM6_DAC_LPTIM1_IRQHandler(void){
    if (TIM6->SR & TIM_SR_UIF) {
        TIM6->SR &= ~(TIM_SR_UIF);
    }
   // else if (DAC1->SR & DAC_SR_DMAUDR2){
   //     DAC1->SR &= DAC_SR_DMAUDR2;
   // }
}

void TIM7_LPTIM2_IRQHandler(void){
    if (TIM7->SR & TIM_SR_UIF){
        TIM7->SR &= ~(TIM_SR_UIF);
    }
}
void DMA1_Channel2_3_IRQHandler(void){
    if ((DMA1->ISR & DMA_ISR_HTIF3) == DMA_ISR_HTIF3) {
        (DMA1->IFCR) = (DMA_IFCR_CHTIF3);
        update_ping_pong_buff(wave_me_d, dac_double_buff, 128);
    }
    if ((DMA1->ISR & DMA_ISR_TCIF3) == DMA_ISR_TCIF3){
        (DMA1->IFCR) = (DMA_IFCR_CTCIF3);
        update_ping_pong_buff(wave_me_d, &dac_double_buff[128], 128);
    }
    if (LL_DMA_IsActiveFlag_TE2(DMA1) == SET){
        LL_DMA_ClearFlag_TE2(DMA1);
        while (1) {}
    }
}

void TIM2_IRQHandler(void) {
    if (TIM2->SR & TIM_SR_UIF) {
        TIM2->SR &= ~(TIM_SR_UIF);
        uint16_t current = pitch0_value;
        int32_t diff = current - prev_value;
        if ((((diff < 0) ? -diff : diff) > 100)) {
            prev_value = current;
            // GPIOB->ODR ^= (1 << 3);
            // __disable_irq();
            // __enable_irq();
            phase_pending_update = true;
        }
#ifdef DEBUG
        GPIOB->ODR ^= (1 << 3);
#endif // DEBUG
    }
}

void EXTI4_15_IRQHandler(void) {
    if ((EXTI->RPR1 & wave_choise_dac1.exti.exti_line) == wave_choise_dac1.exti.exti_line) {
        (EXTI->RPR1) = (wave_choise_dac1.exti.exti_line);
        wave_choise_dac1.flag = 'i';
    }
    if ((EXTI->RPR1 & wave_choise_dac2.exti.exti_line) == wave_choise_dac2.exti.exti_line) {
        (EXTI->RPR1) = (wave_choise_dac2.exti.exti_line);
        wave_choise_dac2.flag = 'i';
    }
}
