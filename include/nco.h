#include "stm32g0xx_ll_utils.h"
#include <stdint.h>
#include <stdbool.h>
void update_ping_pong_buff(const volatile uint16_t *data, uint16_t *bufferSection, uint16_t sectionLength);
static uint16_t map_12b_to_hz(uint16_t adc_value);
ErrorStatus alter_wave_frequency (uint32_t output_freq);
