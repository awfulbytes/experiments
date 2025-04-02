project('Testing', 'c',
       version: '0.0.0',
       meson_version: '>=0.60.0')

src_f = files('./test_nco.c', '../src/nco.c')
src_f_g = files('./test_generation_option.c')

inc_f = include_directories('../include','../ventor/stm32g0xx_hal_driver/Inc' )
o = executable('oscillators', src_f,
               include_directories : inc_f)
g = executable('generate_stuff', src_f_g,
               include_directories : inc_f)
test('nco', o, suite: 'core')
test('generators', g, suite: 'core')
