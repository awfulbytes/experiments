project('Testing', 'c',
       version: '0.0.0',
       meson_version: '>=0.60.0')

src_f = files('./test_nco.c', '../src/nco.c')
src_f_enc = files('./test_distortion_encoder.c', '../src/ui.c')
src_f_math = files('./test_math.c', '../src/nco.c')

src_f_g = files('./test_generation_option.c')

inc_f_all = include_directories('../include', '../ventor/stm32g0xx_hal_driver/Inc',
                            '../ventor/cmsis_device_g0/Include/', '../include/Core/Include/')
o = executable('oscillators', src_f,
               include_directories : inc_f_all)
g = executable('generate_stuff', src_f_g,
               include_directories : inc_f_all)
e = executable('encoder', src_f_enc,
               include_directories : inc_f_all)
m = executable('math', src_f_math,
               include_directories : inc_f_all)

test('nco', o, suite: ['core', 'gen'])
test('generators', g, suite: 'gen')
test('distortion-encoder', e, suite: 'core')
test('maths', m, suite: 'core')
