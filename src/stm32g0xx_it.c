#include "stm32g0xx_it.h"
#include <stdatomic.h>
#include <stdint.h>
// #define DEBUG
#define abs(x) ((x<0) ? -x : x)
extern volatile uint16_t prev_value;
extern volatile uint16_t pitch0_value;
extern struct nco l_osc, r_osc;
extern volatile bool phase_done_update;
extern volatile const uint16_t *wave_me_d, *wave_me_d2;
extern volatile const uint16_t *dither;
extern atomic_ushort dac_double_buff[256], dac_double_buff2[256];
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

    if (phase_done_update) {
        l_osc.phase_inc = l_osc.phase_pending_update_inc;
        r_osc.phase_inc = l_osc.phase_inc;
        generate_half_signal(wave_me_d, 128, &l_osc);
        generate_half_signal(wave_me_d2, 128, &r_osc);
        phase_done_update = false;
    }

    if ((DMA1->ISR & DMA_ISR_HTIF2) == DMA_ISR_HTIF2) {
        (DMA1->IFCR) = (DMA_IFCR_CHTIF2);
        update_ping_pong_buff(r_osc.data_buff.ping_buff, dac_double_buff2, 128);
        if ((DMA1->ISR & DMA_ISR_HTIF3) == DMA_ISR_HTIF3) {
            (DMA1->IFCR) = (DMA_IFCR_CHTIF3);
            update_ping_pong_buff(l_osc.data_buff.ping_buff, dac_double_buff, 128);
        }
    }
    if ((DMA1->ISR & DMA_ISR_TCIF3) == DMA_ISR_TCIF3){
        (DMA1->IFCR) = (DMA_IFCR_CTCIF3);
        update_ping_pong_buff(l_osc.data_buff.ping_buff, dac_double_buff + 128, 128);
        if ((DMA1->ISR & DMA_ISR_TCIF2) == DMA_ISR_TCIF2){
            (DMA1->IFCR) = (DMA_IFCR_CTCIF2);
            update_ping_pong_buff(r_osc.data_buff.ping_buff, dac_double_buff2 + 128, 128);
        }
    }

}

void TIM2_IRQHandler(void) {
    if (TIM2->SR & TIM_SR_UIF) {
        TIM2->SR &= ~(TIM_SR_UIF);
        prev_value = pitch0_value;
        l_osc.phase_pending_update = true;
#ifdef DEBUG
        GPIOB->ODR ^= (1 << 3);
#endif // DEBUG
    }
}

void EXTI4_15_IRQHandler(void) {
    if ((EXTI->RPR1 & wave_choise_dac1.exti.exti_line) == wave_choise_dac1.exti.exti_line) {
        (EXTI->RPR1) = (wave_choise_dac1.exti.exti_line);
        wave_button_callback(&wave_choise_dac1);
        wave_choise_dac1.flag = 'i';
    }
    if ((EXTI->RPR1 & wave_choise_dac2.exti.exti_line) == wave_choise_dac2.exti.exti_line) {
        (EXTI->RPR1) = (wave_choise_dac2.exti.exti_line);
        wave_button_callback(&wave_choise_dac2);
        wave_choise_dac2.flag = 'i';
    }
}
