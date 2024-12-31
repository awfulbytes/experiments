#include "wave.h"

wave_t create_wave(wave_t *empty, uint32_t freq, uint32_t amp, const uint16_t *data){
    empty->frequency = freq;
    empty->amplitude = amp;
    empty->form = data;
    empty->size = DATA_SIZE(scaled_sin);
    return *empty;
}
