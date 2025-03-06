#define idx(x) ((x>>1) + 1)
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
// double octave[16] = {440, 16.352, 17.324, 17.324, 18.354,
//                      19.445, 19.445, 20.602, 20.602, 21.827,
//                      21.827, 23.125, 23.125, 24.5,   25.957,
//                      25.957};

double octave[21] = {440,
                     466.16, 466.16, 493.88, 493.88,
                     523.25, 523.25, 554.37, 554.37, 587.33,
                     622.25, 622.25, 659.26, 659.26,
                     698.46, 698.46, 739.99, 739.99, 783.99,
                     830.61, 830.61};
uint64_t freqs[21]={};
uint64_t osc_oct_incs[21]={};
uint64_t *test_increment_generator(){
    for (uint16_t i=0; i<21; i++) {
        osc_oct_incs[i] = (uint64_t)((octave[i] * (1ULL<<32))/44000);
        assert(sizeof(osc_oct_incs[i]) * 8 == 64);
    }
    printf("size of all increments: %lu bytes\n", sizeof(osc_oct_incs));
    return osc_oct_incs;
}

int main(){
    uint64_t *local_increment_var = test_increment_generator();
    for (int i=0; i<21; i++) {
        freqs[i] = ((local_increment_var[i] * 44000)>>32) + 1;

        printf("%d\t%lu\t\t0x%lx\n", i+1, freqs[i], osc_oct_incs[i]);
    }
    printf("\n~~~~~~~~~ Needed from OG table ~~~~~~~~~\n\n");
    for (uint64_t i=0; i<21; i++) {
        int count = 0;
        ((freqs[i] == freqs[i+1]) ? i++ : i);
        (idx(i) == 5 ?  count++ : count);
        if (count == 2) {i = 8;}
        printf("%lu\t%lu\t\t0x%lx\n", idx(i), freqs[i], osc_oct_incs[i]);
    }
    assert(freqs[0] == 440);
    assert(osc_oct_incs[0] == freqs[0] * (1UL<<32) / 44000);
}
