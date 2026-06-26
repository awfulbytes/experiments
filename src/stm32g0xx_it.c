#include "stm32g0xx_it.h"
#define abs(x)                 ((x<0) ? -x : x)
#define lock_the_door(x)       __disable_irq();\
                               x;\
                               __enable_irq();

/* hack to avoid waveform change at POR */
enum run{dry, on};
enum run runtime = dry;

void HardFault_Handler(void){while(1);}
void NMI_Handler(void){}
void SVC_Handler(void);
void PendSV_Handler(void);
void SysTick_Handler(void);

static inline void memory_barier(void){
    __asm("dmb sy");
}

static inline void sync_barier(void){
    __asm("dsb sy");
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

        sync_barier();
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

/* optimized for asembler */
enum adc_pins_ordered { adc0, adc1, adc9, adc8, adc6, adc_cnt};
void TIM2_IRQHandler(void) {
    memory_barier();
    if((TIM2->SR & TIM_SR_UIF)){
        TIM2->SR &= ~(TIM_SR_UIF);

        if ((DMA1->ISR & DMA_ISR_TCIF4) == DMA_ISR_TCIF4){
            world._cv_0_pitch = cv_raw_adc_inp[adc0];
            world.osc_0_cv_distortion_amount = cv_raw_adc_inp[adc1];
            world.osc_1_cv_distortion_ammount = cv_raw_adc_inp[adc9];
            world._cv_1_pitch = cv_raw_adc_inp[adc8];
            world.tunner_pitch_raw_d = cv_raw_adc_inp[adc6];
            l_osc.phase.pending_update = r_osc.phase.pending_update = true;

            (DMA1->IFCR) = (DMA_IFCR_CTCIF4);
        }
    }
}

static inline void handle_osc_distortion(struct nco nco[static 1]){
    if (!nco->distortion.on) {
#ifdef encoder_leds
        GPIOB->ODR |= (1 << 3);
        GPIOB->ODR &= ~(1 << 5);
#endif // encoder_leds
        nco->distortion.on = true;
        display.view[nco->in_the_house.report] = dist;
        cosmos.sync = false;
    }
    else{
#ifdef encoder_leds
        GPIOB->ODR &= ~(1 << 3);
        GPIOB->ODR |= (1 << 5);
#endif // encoder_leds
        nco->distortion.on = false;
        display.view[nco->in_the_house.report] = tuning;
        cosmos.sync = true;
    }
}

static inline bool check_pend(struct exti settings, uint32_t flag){
    return (flag & settings.exti_line) == settings.exti_line;
}

#pragma GCC diagnostic ignored "-Wunused-but-set-parameter"
#define clear_pending(settings, flag) (flag) = settings.exti_line;

void EXTI4_15_IRQHandler(void) {

    if(check_pend(osc_0_pd_enc.A.it_settings, EXTI->FPR1)){
        osc_0_pd_enc.B.value[0] = read_gpio(&osc_0_pd_enc.B.pin);
        clear_pending(osc_0_pd_enc.A.it_settings, EXTI->FPR1);
    }
    if(check_pend(osc_0_pd_enc.A.it_settings, EXTI->RPR1)){
        osc_0_pd_enc.B.value[1] = read_gpio(&osc_0_pd_enc.B.pin);
        if(runtime == on){ /* hack */
            apply_modulations_callback(&osc_0_pd_enc);
            l_osc.phase.pending_update = true;
        }
        clear_pending(osc_0_pd_enc.A.it_settings, EXTI->RPR1);
    }

    if(check_pend(osc_1_pd_enc.A.it_settings, EXTI->FPR1)){
        osc_1_pd_enc.B.value[0] = read_gpio(&osc_1_pd_enc.B.pin);
        clear_pending(osc_1_pd_enc.A.it_settings, EXTI->FPR1);
    }
    if(check_pend(osc_1_pd_enc.A.it_settings, EXTI->RPR1)){
        osc_1_pd_enc.B.value[1] = read_gpio(&osc_1_pd_enc.B.pin);
        if (runtime == on){ /* hack */
            apply_modulations_callback(&osc_1_pd_enc);
            r_osc.phase.pending_update = true;
        }
        clear_pending(osc_1_pd_enc.A.it_settings, EXTI->RPR1);
    }

    if(check_pend(octave_switch.it[0], EXTI->RPR1)){
        octave_switch._state[0] = read_gpio(&octave_switch.pins[0]);
        l_osc.tempered.oct.change = true;
        clear_pending(octave_switch.it[0], EXTI->RPR1);
    }

    if(check_pend(octave_switch.it[1], EXTI->RPR1)){
        octave_switch._state[1] = read_gpio(&octave_switch.pins[1]);
        l_osc.tempered.rec = true;
        clear_pending(octave_switch.it[1], EXTI->RPR1);
    }

    if(check_pend(switch2_dev_rev0.it[0], EXTI->RPR1)){
        switch2_dev_rev0._state[0] = read_gpio(&switch2_dev_rev0.pins[0]);
        /*                    test                    */
        clear_pending(switch2_dev_rev0.it[0], EXTI->RPR1);
    }

    if(check_pend(switch2_dev_rev0.it[1], EXTI->RPR1)){
        switch2_dev_rev0._state[1] = read_gpio(&switch2_dev_rev0.pins[1]);
        /*                    test                    */
        clear_pending(switch2_dev_rev0.it[1], EXTI->RPR1);
    }

    if(check_pend(freq_mode_but_dac1.exti, EXTI->FPR1)){
        l_osc.tempered.oct.shift = true;
        l_osc.tempered.oct.jump += 1;
        if(display.octave_shifts[0] < 4)
            display.octave_shifts[0] += (uint8_t) l_osc.tempered.oct.jump;
        else
            display.octave_shifts[0] = 4;
        clear_pending(freq_mode_but_dac1.exti, EXTI->FPR1);
    }

    if(check_pend(osc_0_mode_choice.exti, EXTI->FPR1)){
        //change_pitch_mode(&r_osc);
        //r_osc.on_scale = (r_osc.mode == tracking) ? true : false;

        osc_0_mode_choice.state = read_gpio(&osc_0_mode_choice.pin);
        osc_0_mode_choice.flag = 'i';
        clear_pending(osc_0_mode_choice.exti, EXTI->FPR1);
    }
    if(check_pend(distortion_choice.exti, EXTI->FPR1)){
        handle_osc_distortion(&l_osc);
        handle_osc_distortion(&r_osc);
        clear_pending(distortion_choice.exti, EXTI->FPR1);
    }

    if(runtime == dry)
        runtime = on;

}
