#include "stm32g0xx_it.h"
#include "errors.h"
// #include "dac.h"
#include "dma.h"
#include "stm32g071xx.h"
#include "stm32g0xx_ll_dma.h"
#include "stm32g0xx_ll_gpio.h"
#include "stm32g0xx_ll_tim.h"
#include <stdint.h>
// #define DEBUGDAC
extern volatile uint16_t pitch0_value;
extern const uint16_t *scaled_sin;
extern uint16_t *wave_me_d;
extern uint16_t dac_double_buff[240];
extern struct dma dac_1_dma;
extern struct button wave_choise_dac1;
extern struct button wave_choise_dac2;
extern struct gpio led;
extern struct adc pitch0cv_in;

void HardFault_Handler(void){while(1);}
void NMI_Handler(void){}
void SVC_Handler(void);
void PendSV_Handler(void);
void SysTick_Handler(void);

void TIM6_DAC_LPTIM1_IRQHandler(void){
    if (TIM6->SR & TIM_SR_UIF) {
        TIM6->SR &= ~(TIM_SR_UIF);
#ifdef DEBUGDAC
        GPIOB->ODR ^= (1 << 3);
#endif // DEBUG
    }
   // else if (DAC1->SR & DAC_SR_DMAUDR2){
   //     DAC1->SR &= DAC_SR_DMAUDR2;
   // }
}

void DMA1_Channel2_3_IRQHandler(void){
    if (LL_DMA_IsActiveFlag_HT3(DMA1) == SET){
        // GPIOB->ODR ^= (1 << 3);
        LL_DMA_ClearFlag_HT3(DMA1);
        UpdateDacBufferSection(wave_me_d, dac_double_buff, 120);
        // alter_wave_form(dac_double_buff, &dac_1_dma);
    }
    if (LL_DMA_IsActiveFlag_TC3(DMA1) == SET){
        // GPIOB->ODR ^= (1 << 3);
        LL_DMA_ClearFlag_TC3(DMA1);
        UpdateDacBufferSection(wave_me_d, &dac_double_buff[120], 120);
        // alter_wave_form(dac_double_buff, &dac_1_dma);
    }
    if (LL_DMA_IsActiveFlag_TE3(DMA1) == SET){
        LL_DMA_ClearFlag_TE2(DMA1);
        while (1) {}
    }
}

void TIM2_IRQHandler(void) {
    if (TIM2->SR & TIM_SR_UIF) {
        TIM2->SR &= ~(TIM_SR_UIF);
        // while (!(ADC1->ISR & ADC_ISR_EOC)) {}
        pitch0_value = pitch0cv_in.adcx->DR;
        pitch0cv_in.roof = 'i';
    }
#ifdef DEBUG
    GPIOB->ODR ^= (1 << 3);
#endif // DEBUG
}

static inline void button_handler(struct button *b){
    __disable_irq();
    wave_button_callback(b);
    b->flag = 'i';
    __enable_irq();
}

void EXTI4_15_IRQHandler(void) {
    if ((EXTI->RPR1 & wave_choise_dac1.exti.exti_line) == wave_choise_dac1.exti.exti_line) {
        (EXTI->RPR1) = (wave_choise_dac1.exti.exti_line);
        button_handler(&wave_choise_dac1);
    }
    if ((EXTI->RPR1 & wave_choise_dac2.exti.exti_line) == wave_choise_dac2.exti.exti_line) {
        (EXTI->RPR1) = (wave_choise_dac2.exti.exti_line);
        button_handler(&wave_choise_dac2);
    }
}
