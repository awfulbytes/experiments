#include "ui.h"
#include "adc.h"
#include "dma.h"
#include "nco.h"
#include "overseer.h"

extern struct overworld world;
extern volatile uint16_t        cv_raw_adc_inp[5];
extern volatile const uint16_t *wave_me_d, *wave_me_d2;
extern uint32_t dac_double_buff2[256];

extern volatile uint8_t cnt_adc_cycles;

extern struct nco               l_osc, r_osc;
extern struct button            freq_mode_but_dac1;
extern struct button            freq_mode_but_dac2;
extern struct button            distortion_choice;
extern struct encoder           osc_0_pd_enc;
extern struct encoder           osc_1_pd_enc;

#define prime_table_size                                       128
#define buffer_size(buf)       ((sizeof buf) / (sizeof buf[0]))
uint32_t merged_dual_buffer[prime_table_size];
uint8_t const data_size = buffer_size(merged_dual_buffer);

void NMI_Handler(void);
void HardFault_Handler(void);
void SVC_Handler(void);
void PendSV_Handler(void);
void SysTick_Handler(void);
void DMA1_Channel2_3_IRQHandler(void);
void EXTI4_15_IRQHandler(void);

void DMA1_Channel1_IRQHandler(void);

__attribute((always_inline))
inline void change_pitch_mode(struct nco oscillator[static 1]){
    /* todo (test that) should not give issues but i do-not have the time!! */
    oscillator->mode =
        (oscillator->mode == high)
        ? low : high;
}
