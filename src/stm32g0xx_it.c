#include "stm32g0xx_it.h"
#include "overseer.h"
// #define DEBUG
/* #define encoder_leds */
#define abs(x)                 ((x<0) ? -x : x)
#define lock_the_door(x)       __disable_irq();\
                               x;\
                               __enable_irq();
#define buffer_size(buf)       ((sizeof buf) / (sizeof buf[0]))

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

uint32_t construct_dual_dac_reg(uint16_t data_ch_2, uint16_t data_ch_1) {
    return ((uint32_t) data_ch_2 << 16U) | data_ch_1;
}

extern const uint16_t sine_wave[128];
uint32_t merged_dual_buffer[buffer_size(sine_wave)];
uint8_t const data_size = buffer_size(merged_dual_buffer);
void DMA1_Channel2_3_IRQHandler(void){

    if (l_osc.phase.done_update && r_osc.phase.done_update) {
        l_osc.phase.inc = l_osc.phase.pending_update_inc;
        /* this was just for testing now i have to equalize adc pins */
        r_osc.phase.inc = l_osc.phase.pending_update_inc;
        /* r_osc.phase.inc = r_osc.phase.pending_update_inc; */

        generate_half_signal(wave_me_d, data_size, &l_osc);
        generate_half_signal(wave_me_d2, data_size, &r_osc);
        l_osc.phase.done_update = false;
        r_osc.phase.done_update = false;
    }
    for(uint8_t u=0; u < data_size; ++u){
        merged_dual_buffer[u] =
            construct_dual_dac_reg(r_osc.data_buff.ping_buff[u],
                                   l_osc.data_buff.ping_buff[u]);
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
    /* todo it was fun as it lasted...
     * we have an issue where when the distortion is enabled makes signals out
     * of sync. No idea why!!! most of the times you should be able to
     * invalidate the data and start over but here this does not work. nxt:: the
     * phase.inc makes the phase not the data in the table those are one cycle
     * always!!
     */
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
        osc_0_pd_enc.B.value[0] = read_gpio(&osc_0_pd_enc.B.pin);
        (EXTI->FPR1) = (osc_0_pd_enc.A.it_settings.exti_line);
    }
    if ((EXTI->RPR1 & osc_0_pd_enc.A.it_settings.exti_line) == osc_0_pd_enc.A.it_settings.exti_line){
        osc_0_pd_enc.B.value[1] = read_gpio(&osc_0_pd_enc.B.pin);
        /* r_osc.distortion.distortion_value = l_osc.distortion.distortion_value; */
        apply_modulations_callback(&osc_0_pd_enc, &l_osc);
        (EXTI->RPR1) = (osc_0_pd_enc.A.it_settings.exti_line);
    }

    if ((EXTI->FPR1 & osc_1_pd_enc.A.it_settings.exti_line) == osc_1_pd_enc.A.it_settings.exti_line){
        osc_1_pd_enc.B.value[0] = read_gpio(&osc_1_pd_enc.B.pin);
        (EXTI->FPR1) = (osc_1_pd_enc.A.it_settings.exti_line);
    }
    if ((EXTI->RPR1 & osc_1_pd_enc.A.it_settings.exti_line) == osc_1_pd_enc.A.it_settings.exti_line){
        osc_1_pd_enc.B.value[1] = read_gpio(&osc_1_pd_enc.B.pin);
        /* r_osc.distortion.distortion_value = l_osc.distortion.distortion_value; */
        apply_modulations_callback(&osc_1_pd_enc, &r_osc);
        (EXTI->RPR1) = (osc_1_pd_enc.A.it_settings.exti_line);
    }

    if ((EXTI->RPR1 & freq_mode_but_dac1.exti.exti_line) == freq_mode_but_dac1.exti.exti_line){
        (EXTI->RPR1) = (freq_mode_but_dac1.exti.exti_line);
        l_osc.mode = change_pitch_mode(&l_osc);
    }
    if ((EXTI->RPR1 & freq_mode_but_dac2.exti.exti_line) == freq_mode_but_dac2.exti.exti_line) {
        (EXTI->RPR1) = (freq_mode_but_dac2.exti.exti_line);
        r_osc.mode = change_pitch_mode(&r_osc);
    }
    if ((EXTI->RPR1 & distortion_choice.exti.exti_line) == distortion_choice.exti.exti_line) {
        (EXTI->RPR1) = (distortion_choice.exti.exti_line);

        handle_osc_distortion(&l_osc);
        handle_osc_distortion(&r_osc);
    }
    return;
}
