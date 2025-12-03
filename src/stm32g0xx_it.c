#include "stm32g0xx_it.h"
// #define DEBUG
#define encoder_leds
#define abs(x)                 ((x<0) ? -x : x)
#define lock_the_door(x)       __disable_irq();\
                               x;\
                               __enable_irq();

void HardFault_Handler(void){while(1);}
void NMI_Handler(void){}
void SVC_Handler(void);
void PendSV_Handler(void);
void SysTick_Handler(void);

void TIM6_DAC_LPTIM1_IRQHandler(void){
    if (TIM6->SR & TIM_SR_UIF) {
        TIM6->SR &= ~(TIM_SR_UIF);
    }
}

void TIM7_LPTIM2_IRQHandler(void){
    if (TIM7->SR & TIM_SR_UIF){
        TIM7->SR &= ~(TIM_SR_UIF);
    }
}

void DMA1_Channel2_3_IRQHandler(void){

    if (l_osc.phase.done_update) {
        l_osc.phase.inc = l_osc.phase.pending_update_inc;
        generate_half_signal(wave_me_d, 128, &l_osc);
        l_osc.phase.done_update = false;
    }
    if (r_osc.phase.done_update) {
        r_osc.phase.inc = r_osc.phase.pending_update_inc;
        /* todo(nxt) lock the door if helps */
        generate_half_signal(wave_me_d2, 128, &r_osc);
        r_osc.phase.done_update = false;
        /* GPIOB->ODR |= (1 << 5); */
        /* GPIOB->ODR |= (1 << 3); */
    }
    /* i get phase difference because of the sequential reads/writes...
     * π i could use another dac interrupt...*/
    if (((DMA1->ISR & DMA_ISR_HTIF2) == DMA_ISR_HTIF2) &&
        ((DMA1->ISR & DMA_ISR_HTIF3) == DMA_ISR_HTIF3)) {
        (DMA1->IFCR) = (DMA_IFCR_CHTIF2);
        (DMA1->IFCR) = (DMA_IFCR_CHTIF3);
        lock_the_door(
        update_data_buff(l_osc.data_buff.ping_buff, dac_double_buff, 128);
        update_data_buff(r_osc.data_buff.ping_buff, dac_double_buff2, 128))
    }
    if (((DMA1->ISR & DMA_ISR_TCIF2) == DMA_ISR_TCIF2) &&
        ((DMA1->ISR & DMA_ISR_TCIF3) == DMA_ISR_TCIF3)) {
        (DMA1->IFCR) = (DMA_IFCR_CTCIF2);
        (DMA1->IFCR) = (DMA_IFCR_CTCIF3);
        lock_the_door(
        update_data_buff(l_osc.data_buff.ping_buff, dac_double_buff + 128, 128);
        update_data_buff(r_osc.data_buff.ping_buff, dac_double_buff2 + 128, 128))
    }
}

void TIM2_IRQHandler(void) {
    if (TIM2->SR & TIM_SR_UIF) {
        TIM2->SR &= ~(TIM_SR_UIF);

        if ((DMA1->ISR & DMA_ISR_TCIF4) == DMA_ISR_TCIF4){
            (DMA1->IFCR) = (DMA_IFCR_CTCIF4);

            world.current_value_cv_0_pitch = cv_raw_adc_inp[0];
            world.current_value_cv_distortion_amount = cv_raw_adc_inp[1];
            world.distortion_amount_cv = cv_raw_adc_inp[2];
            world.current_value_cv_1_pitch = cv_raw_adc_inp[3];
            l_osc.phase.pending_update = r_osc.phase.pending_update = true;
        }
        /* GPIOB->ODR |= (1 << 3); */
    }
}

static inline void handle_osc_distortion(struct nco nco[static 1]){
    if (!nco->distortion.on) {
#ifdef encoder_leds
        GPIOB->ODR |= (1 << 3);
        GPIOB->ODR &= ~(1 << 5);
#endif // encoder_leds
        nco->distortion.on = true;
    }
    else{
#ifdef encoder_leds
        GPIOB->ODR &= ~(1 << 3);
        GPIOB->ODR |= (1 << 5);
#endif // encoder_leds
        nco->distortion.on = false;
    }
}

void EXTI4_15_IRQHandler(void) {
    if ((EXTI->FPR1 & osc_0_pd_enc.A.it_settings.exti_line) == osc_0_pd_enc.A.it_settings.exti_line){
        (EXTI->FPR1) = (osc_0_pd_enc.A.it_settings.exti_line);
        osc_0_pd_enc.B.value = ((osc_0_pd_enc.B.pin.port_id->IDR) & (1U<<5)) ? 1U : 0;
        osc_0_pd_enc.A.flag = 'i';
        scan_and_apply_current_modulations(&osc_0_pd_enc, &l_osc);
    }
    if ((EXTI->FPR1 & osc_1_pd_enc.A.it_settings.exti_line) == osc_1_pd_enc.A.it_settings.exti_line){
        (EXTI->FPR1) = (osc_1_pd_enc.A.it_settings.exti_line);
        osc_1_pd_enc.B.value = ((osc_1_pd_enc.B.pin.port_id->IDR) & (1U<<8)) ? 1U : 0;
        osc_1_pd_enc.A.flag = 'i';
        scan_and_apply_current_modulations(&osc_1_pd_enc, &r_osc);
    }
    if ((EXTI->RPR1 & wave_choise_dac1.exti.exti_line) == wave_choise_dac1.exti.exti_line){
        (EXTI->RPR1) = (wave_choise_dac1.exti.exti_line);
        wave_button_callback(&wave_choise_dac1);
        wave_choise_dac1.flag = 'i';
    }
    if ((EXTI->RPR1 & wave_choise_dac2.exti.exti_line) == wave_choise_dac2.exti.exti_line) {
        (EXTI->RPR1) = (wave_choise_dac2.exti.exti_line);
        wave_button_callback(&wave_choise_dac2);
        wave_choise_dac2.flag = 'i';
    }
    if ((EXTI->RPR1 & distortion_choice.exti.exti_line) == distortion_choice.exti.exti_line) {
        (EXTI->RPR1) = (distortion_choice.exti.exti_line);

        // todo need to make this more reliable... check timers....
        handle_osc_distortion(&l_osc);
        handle_osc_distortion(&r_osc);
    }
}
