#include "stm32g0xx_ll_dac.h"
/* #include "forms.h" */
#define VDDA_APPLI                       ((uint32_t)3300)
#define DIGITAL_SCALE_12BITS             (__LL_DAC_DIGITAL_SCALE(LL_DAC_RESOLUTION_12B))
#define WAVEFORM_AMPLITUDE               (VDDA_APPLI)
/* #define WAVEFORM_FREQUENCY               ((uint32_t)440) /\* NOTE:: 2.5 Hz acctualy *\/ */
/* #define WAVEFORM_SAMPLES_SIZE            (sizeof (base.form) / sizeof (uint16_t)) /\* HACK:: may work*\/ */

#define DATA_SIZE(wave)         (uint32_t) sizeof(wave)/sizeof(wave[0])
#define __WAVEFORM_AMPLITUDE_SCALING(__DATA_12BITS__)                                     \
  (__DATA_12BITS__                                                                        \
   * __LL_DAC_CALC_VOLTAGE_TO_DATA(VDDA_APPLI, WAVEFORM_AMPLITUDE, LL_DAC_RESOLUTION_12B) \
   / __LL_DAC_DIGITAL_SCALE(LL_DAC_RESOLUTION_12B)                                        \
  )
typedef struct wave {
    uint32_t frequency;
    uint32_t amplitude;
    const uint16_t *form;
    uint32_t size;
}wave_t;

wave_t create_wave(wave_t *empty, uint32_t freq, uint32_t amp, const uint16_t *data);

uint32_t calc_wave_size(wave_t *wave);

/** Generated using Dr LUT - Free Lookup Table Generator
  * https://github.com/ppelikan/drlut
  **/
// Formula: sin(2*pi*t/T)

/** Generated using Dr LUT - Free Lookup Table Generator
  * https://github.com/ppelikan/drlut
  **/
// Formula: sin(2*pi*t/T)
const uint16_t scaled_sin[] = {
2047, 2154, 2261, 2367, 2473, 2577, 2680, 2781,
 2880, 2976, 3070, 3162, 3250, 3335, 3417, 3494,
 3568, 3638, 3703, 3764, 3820, 3871, 3917, 3958,
 3994, 4024, 4049, 4069, 4083, 4091, 4094, 4091,
 4083, 4069, 4049, 4024, 3994, 3958, 3917, 3871,
 3820, 3764, 3703, 3638, 3568, 3494, 3417, 3335,
 3250, 3162, 3070, 2976, 2880, 2781, 2680, 2577,
 2473, 2367, 2261, 2154, 2047, 1940, 1833, 1727,
 1621, 1517, 1414, 1313, 1214, 1118, 1024,  932,
  844,  759,  677,  600,  526,  456,  391,  330,
  274,  223,  177,  136,  100,   70,   45,   25,
   11,    3,    0,    3,   11,   25,   45,   70,
  100,  136,  177,  223,  274,  330,  391,  456,
  526,  600,  677,  759,  844,  932, 1024, 1118,
 1214, 1313, 1414, 1517, 1621, 1727, 1833, 1940
   /*  __WAVEFORM_AMPLITUDE_SCALING(2048), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(2149), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(2250), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(2350), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(2450), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(2549), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(2646), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(2742), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(2837), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(2929), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(3020), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(3108), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(3193), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(3275), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(3355), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(3431), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(3504), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(3574), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(3639), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(3701), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(3759), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(3812), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(3861), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(3906), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(3946), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(3982), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(4013), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(4039), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(4060), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(4076), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(4087), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(4094), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(4095), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(4091), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(4082), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(4069), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(4050), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(4026), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(3998), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(3965), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(3927), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(3884), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(3837), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(3786), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(3730), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(3671), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(3607), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(3539), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(3468), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(3394), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(3316), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(3235), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(3151), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(3064), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(2975), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(2883), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(2790), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(2695), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(2598), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(2500), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(2400), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(2300), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(2199), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(2098), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(1997), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(1896), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(1795), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(1695), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(1595), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(1497), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(1400), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(1305), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(1212), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(1120), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(1031), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(944), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(860), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(779), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(701), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(627), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(556), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(488), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(424), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(365), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(309), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(258), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(211), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(168), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(130), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(97), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(69), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(45), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(26), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(13), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(4), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(0), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(1), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(8), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(19), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(35), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(56), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(82), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(113), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(149), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(189), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(234), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(283), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(336), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(394), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(456), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(521), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(591), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(664), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(740), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(820), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(902), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(987), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(1075), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(1166), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(1258), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(1353), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(1449), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(1546), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(1645), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(1745), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(1845), */
   /*  __WAVEFORM_AMPLITUDE_SCALING(1946), */
   /* __WAVEFORM_AMPLITUDE_SCALING(2047) */
};

/* const uint16_t sine_12bits_32[] = */
/* { */
/* __WAVEFORM_AMPLITUDE_SCALING(2048), */
/* __WAVEFORM_AMPLITUDE_SCALING(2447), */
/* __WAVEFORM_AMPLITUDE_SCALING(2831), */
/* __WAVEFORM_AMPLITUDE_SCALING(3185), */
/* __WAVEFORM_AMPLITUDE_SCALING(3495), */
/* __WAVEFORM_AMPLITUDE_SCALING(3750), */
/* __WAVEFORM_AMPLITUDE_SCALING(3939), */
/* __WAVEFORM_AMPLITUDE_SCALING(4056), */
/* __WAVEFORM_AMPLITUDE_SCALING(4095), */
/* __WAVEFORM_AMPLITUDE_SCALING(4056), */
/* __WAVEFORM_AMPLITUDE_SCALING(3939), */
/* __WAVEFORM_AMPLITUDE_SCALING(3750), */
/* __WAVEFORM_AMPLITUDE_SCALING(3495), */
/* __WAVEFORM_AMPLITUDE_SCALING(3185), */
/* __WAVEFORM_AMPLITUDE_SCALING(2831), */
/* __WAVEFORM_AMPLITUDE_SCALING(2447), */
/* __WAVEFORM_AMPLITUDE_SCALING(2048), */
/* __WAVEFORM_AMPLITUDE_SCALING(1649), */
/* __WAVEFORM_AMPLITUDE_SCALING(1265), */
/* __WAVEFORM_AMPLITUDE_SCALING(911), */
/* __WAVEFORM_AMPLITUDE_SCALING(601), */
/* __WAVEFORM_AMPLITUDE_SCALING(346), */
/* __WAVEFORM_AMPLITUDE_SCALING(157), */
/* __WAVEFORM_AMPLITUDE_SCALING(40), */
/* __WAVEFORM_AMPLITUDE_SCALING(0), */
/* __WAVEFORM_AMPLITUDE_SCALING(40), */
/* __WAVEFORM_AMPLITUDE_SCALING(157), */
/* __WAVEFORM_AMPLITUDE_SCALING(346), */
/* __WAVEFORM_AMPLITUDE_SCALING(601), */
/* __WAVEFORM_AMPLITUDE_SCALING(911), */
/* __WAVEFORM_AMPLITUDE_SCALING(1265), */
/* __WAVEFORM_AMPLITUDE_SCALING(1649) */
/* }; */
