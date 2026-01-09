#include "stm32g0xx_it.h"
// #define DEBUG
/* #define encoder_leds */
#define abs(x)                 ((x<0) ? -x : x)
#define lock_the_door(x)       __disable_irq();\
                               x;\
                               __enable_irq();

void HardFault_Handler(void){while(1);}
void NMI_Handler(void){}
void SVC_Handler(void);
void PendSV_Handler(void);
void SysTick_Handler(void);

void TIM7_LPTIM2_IRQHandler(void){
    /* todo(nxt) make this a profiling timer */
    if (TIM7->SR & TIM_SR_UIF){
        TIM7->SR &= ~(TIM_SR_UIF);
    }
}

void DMA1_Channel2_3_IRQHandler(void){
    /* optimized access for compiler massage */
    if (l_osc.phase.done_update && r_osc.phase.done_update) {
        l_osc.phase.inc = l_osc.phase.pending_update_inc;
        r_osc.phase.inc = r_osc.phase.pending_update_inc;

        if(cosmos.sync){
            /* sync_fcw(cosmos.oscillators); */
            align_phase(cosmos.oscillators);
        }

        for(uint8_t o=0; o < 2; ++o){
            generate_half_signal(data_size, cosmos.oscillators[o]);
        }
        merge_signals_dual_dac_mode(cosmos.oscillators, merged_dual_buffer, data_size);
    }

    if (((DMA1->ISR & DMA_ISR_HTIF2) == DMA_ISR_HTIF2)) {
        update_data_buff(merged_dual_buffer,
                         dac_double_buff2,
                         data_size);
        (DMA1->IFCR) = (DMA_IFCR_CHTIF2);
    }

    if (((DMA1->ISR & DMA_ISR_TCIF2) == DMA_ISR_TCIF2)) {
        update_data_buff(merged_dual_buffer,
                         dac_double_buff2 + data_size,
                         data_size);
        (DMA1->IFCR) = (DMA_IFCR_CTCIF2);
    }
}

void TIM2_IRQHandler(void) {
    if (TIM2->SR & TIM_SR_UIF) {

        if ((DMA1->ISR & DMA_ISR_TCIF4) == DMA_ISR_TCIF4){
            world.current_value_cv_0_pitch = cv_raw_adc_inp[0];
            world.osc_0_cv_distortion_amount = cv_raw_adc_inp[1];
            world.osc_1_cv_distortion_ammount = cv_raw_adc_inp[2];
            world.current_value_cv_1_pitch = cv_raw_adc_inp[3];
            world.osc_0_cv_2_distortion_amount = cv_raw_adc_inp[4];
            l_osc.phase.pending_update = r_osc.phase.pending_update = true;

            (DMA1->IFCR) = (DMA_IFCR_CTCIF4);
        }

        TIM2->SR &= ~(TIM_SR_UIF);
    }
}

static inline void handle_osc_distortion(struct nco nco[static 1]){
    if (!nco->distortion.on) {
#ifdef encoder_leds
        GPIOB->ODR |= (1 << 3);
        GPIOB->ODR &= ~(1 << 5);
#endif // encoder_leds
        nco->distortion.on = true;
        cosmos.sync = false;
    }
    else{
#ifdef encoder_leds
        GPIOB->ODR &= ~(1 << 3);
        GPIOB->ODR |= (1 << 5);
#endif // encoder_leds
        nco->distortion.on = false;
        cosmos.sync = true;
    }
}

void EXTI4_15_IRQHandler(void) {

    if ((EXTI->FPR1 & osc_0_pd_enc.A.it_settings.exti_line) == osc_0_pd_enc.A.it_settings.exti_line){
        osc_0_pd_enc.B.value[0] = read_gpio(&osc_0_pd_enc.B.pin);
        (EXTI->FPR1) = (osc_0_pd_enc.A.it_settings.exti_line);
    }
    if ((EXTI->RPR1 & osc_0_pd_enc.A.it_settings.exti_line) == osc_0_pd_enc.A.it_settings.exti_line){
        osc_0_pd_enc.B.value[1] = read_gpio(&osc_0_pd_enc.B.pin);
        apply_modulations_callback(&osc_0_pd_enc, &l_osc);
        (EXTI->RPR1) = (osc_0_pd_enc.A.it_settings.exti_line);
    }

    if ((EXTI->FPR1 & osc_1_pd_enc.A.it_settings.exti_line) == osc_1_pd_enc.A.it_settings.exti_line){
        osc_1_pd_enc.B.value[0] = read_gpio(&osc_1_pd_enc.B.pin);
        (EXTI->FPR1) = (osc_1_pd_enc.A.it_settings.exti_line);
    }
    if ((EXTI->RPR1 & osc_1_pd_enc.A.it_settings.exti_line) == osc_1_pd_enc.A.it_settings.exti_line){
        osc_1_pd_enc.B.value[1] = read_gpio(&osc_1_pd_enc.B.pin);
        apply_modulations_callback(&osc_1_pd_enc, &r_osc);
        (EXTI->RPR1) = (osc_1_pd_enc.A.it_settings.exti_line);
    }

    if ((EXTI->RPR1 & freq_mode_but_dac1.exti.exti_line) == freq_mode_but_dac1.exti.exti_line){
        change_pitch_mode(&l_osc);
        (EXTI->RPR1) = (freq_mode_but_dac1.exti.exti_line);
    }
    if ((EXTI->RPR1 & freq_mode_but_dac2.exti.exti_line) == freq_mode_but_dac2.exti.exti_line) {
        change_pitch_mode(&r_osc);
        (EXTI->RPR1) = (freq_mode_but_dac2.exti.exti_line);
    }
    if ((EXTI->RPR1 & distortion_choice.exti.exti_line) == distortion_choice.exti.exti_line) {
        handle_osc_distortion(&l_osc);
        handle_osc_distortion(&r_osc);
        (EXTI->RPR1) = (distortion_choice.exti.exti_line);
    }
    return;
}
