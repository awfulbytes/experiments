#include "nco.h"
#include "string.h"

void apply_pd_alg(struct nco nco[static 1]){
    nco->distortion.distortion_value = nco->phase_inc << nco->distortion.dante;
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
        fract   = ((uint64_t)((nco->phase_accum) * (1<<7)) >> 16) & 0xffff;

#ifdef TEST
        dbg_info_nco(nco, fract, diff);
#endif // TEST
        if (y == nco->distortion.amount
            && nco->distortion.on) {
            apply_pd_alg(nco);
            // hack:: make additive or subtractive it makes nice sounds
            nco->phase_inc -= (nco->distortion.distortion_value);
        }
        nco->data_buff.ping_buff[y] = (a + ((uint16_t)(((diff * fract) >> 16))));
        nco->phase_accum += nco->phase_inc;
    }
}

__attribute__((pure))
uint16_t map_12b_to_range(uint16_t      value,
                          struct limits boundaries[static 1]) {
    uint16_t range = boundaries->max - boundaries->min;
    return (uint16_t)(boundaries->min + (value * range) / boundaries->cv_raw_max);
}

__attribute__((pure))
bool stage_pending_inc(volatile uint16_t      adc_raw_value,
                       struct   nco           nco[static 1],
                       const    uint_fast32_t sample_rate){

    /* todo(nxt) Here we need some work.
     * - π what if i disable interrupts while generating the signal above?
     *   this is much easier to test and should test it first.
     *
     * - another thing is i could put the hole dac data in a double double buffer and store the hole data in one big array.
     *   with this i can then make the generation of the buffer in one go for both oscillators and then serve the dac but i have
     *   to make sure that all the signal goes back and i have to change testing also.. i may need this later
     *
     * - the idea is to sample 2 channels and then use the upper bits from the first one e.g. something like (first >> 2) << 2
     *   and also have a (second << 10) >> 10 to discard the upper 10 bits and keep these 2 that we discarded from the first.
     * - A better way could be to make a mask and xor them together. This is what other people do!!
     * - Having two channels cooperating to give one more stable value is spearable i thing...
     * - π ... what if i used the distortion amount knob and put a latch for the alternate usage for the fine knob.
     *
     */

    uint16_t note =
        (nco->mode == free) ?
        map_12b_to_range(adc_raw_value, &nco->bandwidth.free) :
        map_12b_to_range(adc_raw_value, &nco->bandwidth.tracking);
    nco->phase_pending_update_inc = compute_nco_increment(note, sample_rate);

    return true;
}

__attribute__((always_inline))
inline void update_data_buff(const uint16_t data[static 128],
                             uint16_t       buffer_sector[static 128],
                             uint16_t       sector_length) {
    memcpy(buffer_sector, data, sizeof(uint16_t) * sector_length);
}

void stage_modulated_signal_values(struct   nco      osc[static 1],
                                   uint16_t          distortion_cv,
                                   volatile uint16_t pitch_cv,
                                   uint32_t          master_clock){
    if(osc->phase_pending_update){
        osc->distortion.amount = map_12b_to_range(distortion_cv,
                                                  &osc->distortion.level_range);
        bool staged = stage_pending_inc(pitch_cv, osc, master_clock);
        osc->phase_done_update = staged;
        osc->phase_pending_update = !osc->phase_done_update;
    }
}
