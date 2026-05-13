#include "nco.h"

void apply_pd_alg(volatile struct nco nco[static 1]){
    nco->distortion.distortion_value = nco->phase.inc << 1;
}

#pragma GCC diagnostic ignored "-Wsign-conversion"
void generate_half_signal(uint16_t sector_length,
                          volatile struct nco     nco[static 1]){
    register uint32_t index, n_index, fract;
    register uint16_t a, b, y;
    register int16_t  diff;
    for (y = 0; y < sector_length; ++y) {

        index   = compute_lut_index(nco->phase.accum);

        if (index==nco->distortion.amount && index!=0 && nco->distortion.on) {
            apply_pd_alg(nco);
            // hack:: make additive or subtractive it makes nice sounds
            nco->phase.inc -= nco->distortion.distortion_value;
        }

        n_index = index + 1;
        a       = nco->curr_wave_ptr[index];
        b       = nco->curr_wave_ptr[n_index & 0x7f];
        diff    = (int16_t) (b-a);

#ifdef TEST
        dbg_info_nco(nco, fract, diff);
#endif // TEST
        if(diff>2000 || diff<-2000)
            nco->data_buff[y] = a;
        else{
            fract   = ((uint64_t)((nco->phase.accum) * (1<<7)) >> 16);
            nco->data_buff[y] = (a + ((uint16_t)(((diff * fract) >> 16))));
        }
        nco->phase.accum += nco->phase.inc;
    }
    nco->phase.done_update = false;
}

uint16_t map_uint(uint16_t value,
                  volatile struct limits boundaries[static 1]) {
    uint16_t range = boundaries->max - boundaries->min;
    return (uint16_t)(boundaries->min + U16DIVBY32767((value * range))) /* / boundaries->cv_raw_max) */;
}

bool stage_pending_inc(volatile        uint16_t      note,
                       volatile struct nco           nco[static 1],
                       const           uint32_t      sample_rate){
    if(note > nco->bandwidth.free.max)
        return false;

    nco->phase.pending_update_inc = compute_nco_increment(note, sample_rate);
    return true;
}

void fmmcpy32 (void* dst, const void* src, uint16_t length) {
    uint32_t* d = dst;
    const uint32_t* s = src;
    for (uint8_t z=0; z < length; ++z) {
        d[z] = s[z];
    }
}

inline void update_data_buff(const volatile  uint32_t data[static 128],
                             uint32_t                 buffer_sector[static 128],
                             uint16_t                 sector_length) {
    fmmcpy32(buffer_sector, (const void*) data, sector_length);
}

void align_phase(volatile struct nco *o[2]){
    o[1]->phase.accum = o[0]->phase.accum;
}

void sync_fcw(volatile struct nco *o[2]){
    o[1]->phase.inc = o[0]->phase.inc;
}
