#include "nco.h"

void apply_pd_alg(struct nco nco[static 1]){
    nco->distortion.distortion_value = nco->phase.inc << nco->distortion.dante;
}

#pragma GCC diagnostic ignored "-Wsign-conversion"
void generate_half_signal(volatile const uint16_t data[static 128],
                          uint16_t                sector_length,
                          struct   nco            nco[static 1]){
    register uint32_t index, n_index, fract;
    register uint16_t a, b, y;
    register int16_t  diff;

    for (y = 0; y < sector_length; ++y) {
        index   = compute_lut_index(nco);
        n_index = index + 1;
        a       = data[index];
        b       = data[n_index & 0x7f];
        diff    = (int16_t) (b-a);
        fract   = ((uint64_t)((nco->phase.accum) * (1<<7)) >> 16) & 0xffff;

#ifdef TEST
        dbg_info_nco(nco, fract, diff);
#endif // TEST
        if (y == nco->distortion.amount
            && nco->distortion.on) {
            apply_pd_alg(nco);
            // hack:: make additive or subtractive it makes nice sounds
            nco->phase.inc -= (nco->distortion.distortion_value);
        }
        /*
         * needs too much work...
         */
        /* if (y >= nco->distortion.amount_2 */
        /*     && nco->distortion.on) { */
        /*     apply_pd_alg(nco); */
        /*     // hack:: make additive or subtractive it makes nice sounds */
        /*     nco->phase.inc += (nco->distortion.distortion_value); */
        /* } */
        nco->data_buff.ping_buff[y] = (a + ((uint16_t)(((diff * fract) >> 16))));
        nco->phase.accum += nco->phase.inc;
    }
}

__attribute__((pure))
uint16_t map_uint(uint16_t      value,
                  volatile struct limits boundaries[static 1]) {
    uint16_t range = boundaries->max - boundaries->min;
    return (uint16_t)(boundaries->min + (value * range) / boundaries->cv_raw_max);
}

__attribute__((pure))
bool stage_pending_inc(volatile uint16_t      note,
                       volatile struct nco    nco[static 1],
                       const    uint_fast32_t sample_rate){
    if(note > 15100)
        return false;

    nco->phase.pending_update_inc = compute_nco_increment(note, sample_rate);
    return true;
}

static inline void mmcpy ( void* dst, const void* src, uint16_t length) {
    char* d = dst;
    const char* s = src;
    for (int z=0; z < length; ++z) {
        d[z] = s[z];
    }
}

__attribute__((always_inline))
inline void update_data_buff(const volatile  uint32_t data[static 128],
                             uint32_t       buffer_sector[static 128],
                             uint16_t       sector_length) {
    /* memcpy(buffer_sector, (const void* restrict) data, sizeof(uint16_t) * sector_length); */
    mmcpy(buffer_sector, (const void*) data, sizeof(uint32_t) * sector_length);
}
