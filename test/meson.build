project('Testing', 'c',
       version: '0.0.0',
       meson_version: '>=0.60.0')

MCU        = '-DSTM32G071xx'
LL         = '-DUSE_FULL_LL_DRIVER'
testing    = '-DTEST'

src_f = files('./test_nco_increment.c', '../src/nco.c')
src_f_s = files('./test_signal_generation.c', '../src/nco.c')
src_f_enc = files('./test_distortion_encoder.c', '../src/ui.c')
src_f_math = files('./test_math.c', '../src/nco.c')

src_f_d = files('./test_distortion_on_signal.c', '../src/nco.c', '../src/ui.c', '../src/overseer.c')

inc_f_all = include_directories('../include', '../ventor/stm32g0xx_hal_driver/Inc',
                            '../ventor/cmsis_device_g0/Include/', '../include/Core/Include/')

g = executable('oscillator-increment', src_f,
               c_args : [MCU , LL, testing],
               include_directories : inc_f_all)
s = executable('signal-generation', src_f_s,
               c_args : [MCU , LL, testing],
               include_directories : inc_f_all)
e = executable('encoder', src_f_enc,
               c_args : [MCU , LL, testing],
               include_directories : inc_f_all)
m = executable('math', src_f_math,
               c_args : [MCU , LL, testing],
               include_directories : inc_f_all)
d = executable('distorted-signal', src_f_d,
               c_args : [MCU , LL],
               include_directories : inc_f_all)

test('nco-increment', g, suite: ['nco'])
test('signal-generation', s, suite: ['nco'])
test('distorded-signal', d, suite: ['nco'])
test('distortion-encoder', e, suite: 'ui')
test('maths', m, suite: 'math')
