project('Testing', 'c',
       version: '0.0.0',
       meson_version: '>=0.60.0')
src_f = files('./test_nco.c', '../src/nco.c')
inc_f = include_directories('../include')
e = executable('testing', src_f,
               include_directories : inc_f)
test('NCO', e)
