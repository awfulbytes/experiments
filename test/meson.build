project('Testing', 'c',
       version: '0.0.0',
       meson_version: '>=0.60.0')

src_f = files('./test_nco_increment.c', '../src/nco.c')
src_f_s = files('./test_signal_generation.c', '../src/nco.c')
src_f_enc = files('./test_distortion_encoder.c', '../src/ui.c')
src_f_math = files('./test_math.c', '../src/nco.c')

src_f_d = files('./test_generation_option.c')

inc_f_all = include_directories('../include', '../ventor/stm32g0xx_hal_driver/Inc',
                            '../ventor/cmsis_device_g0/Include/', '../include/Core/Include/')

g = executable('oscillator-increment', src_f,
               include_directories : inc_f_all)
s = executable('signal-generation', src_f_s,
               include_directories : inc_f_all)
e = executable('encoder', src_f_enc,
               include_directories : inc_f_all)
m = executable('math', src_f_math,
               include_directories : inc_f_all)

d = executable('generate_stuff', src_f_d,
               include_directories : inc_f_all)

test('nco-increment', g, suite: ['core', 'nco'])
test('signal-generation', s, suite: ['core', 'nco'])
test('generators', d, suite: 'gen')
test('distortion-encoder', e, suite: 'core')
test('maths', m, suite: 'core')
