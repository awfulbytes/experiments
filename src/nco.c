#include "nco.h"

#pragma GCC diagnostic ignored "-Wsign-conversion"
void
generate_half_signal(uint16_t sector_length, volatile struct nco nco[static 1]){
        register uint32_t index, n_index, fract;
        register uint16_t a, b, y = 0;
        register int16_t  diff = 0;
        for (; y < sector_length; ++y) {
                index = compute_lut_index(nco->phase.accum);

                if(index    == nco->dstr.amount
                   && index != 0 && nco->dstr.on) {
                        nco->dstr.value =
                                nco->phase.inc << nco->dstr.modifier;

                        /* hack:: make additive or subtractive it makes nice sounds */
                        nco->phase.inc -= nco->dstr.value;
                }

                n_index = index + 1;
                a = nco->curr_wave_ptr[index];
                b = nco->curr_wave_ptr[n_index & 0x7f];
                diff = (int16_t)(b - a);

#ifdef TEST
                dbg_info_nco(nco, fract, diff);
#endif
                if((diff>2000 || diff<-2000) && !nco->dstr.on)
                        nco->data_buff[y] = a;
                else {
                        fract = ((uint64_t)((nco->phase.accum) * (1 << 7)) >> 16);
                        nco->data_buff[y] = (a + ((uint16_t)(((diff * fract) >> 16))));
                }
                nco->phase.accum += nco->phase.inc;
        }
        nco->phase.done_update = false;
}

uint16_t
map_uint(uint16_t value, volatile struct limits boundaries[static 1]) {
        return (uint16_t)(boundaries->min
                          + U16DIVBY32767((value * (boundaries->max - boundaries->min))));
}

bool stage_pending_inc(volatile uint16_t note, volatile struct nco nco[static 1], const uint32_t sample_rate){
    if(note > nco->bw.free.max)
        return false;

    nco->phase.pending_update_inc = compute_nco_increment(note, sample_rate);
    return true;
}

void fmmcpy32 (const void* src, void* dst, uint16_t length) {
    uint32_t* d = dst;
    const uint32_t* s = src;
    for (uint8_t z=0; z < length; ++z) {
        d[z] = s[z];
    }
}

inline void update_data_buff(const volatile uint32_t data[static 128], uint32_t buffer_sector[static 128], uint16_t sector_length){
    fmmcpy32((const void*)data, buffer_sector, sector_length);
}

void align_phase(volatile struct nco *o[2]){
    o[1]->phase.accum = o[0]->phase.accum;
}

void sync_fcw(volatile struct nco *o[2]){
    o[1]->phase.inc = o[0]->phase.inc;
}
