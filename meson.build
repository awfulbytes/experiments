project('tryC', 'c',
  version : '0.1.0',
  meson_version : '>=0.60.0')

src_files = files(
  'drivers/dac.c',
  'drivers/timx.c',
  'drivers/adc.c',
  'drivers/dma.c',
  'src/interrupter.c',
  'src/startup_stm32g071xx.s',
  'src/system_stm32g0xx.c',
  'src/nco.c',
  'src/stm32g0xx_it.c',
  'src/overseer.c',
  )

src_deps = files(
  'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_ll_adc.c',
  'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_ll_dac.c',
  'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_ll_dma.c',
  'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_ll_exti.c',
  'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_ll_gpio.c',
  'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_ll_lptim.c',
  'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_ll_pwr.c',
  'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_ll_rcc.c',
  'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_ll_tim.c',
  'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_ll_usart.c',
  'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_ll_usb.c',
  'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_ll_utils.c',
  # ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  # those features do not exist or are not used `atm
  # ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  # 'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_ll_i2c.c',
  # 'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_ll_lpuart.c',
  # 'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_ll_rtc.c',
  # 'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_ll_spi.c',
  # 'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_hal_gpio.c',
  # 'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_ll_ucpd.c',
  # 'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_hal_cortex.c',
  # 'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_ll_comp.c',
  # 'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_ll_crc.c',
  # 'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_ll_crs.c',
)
inc_dir = include_directories('include')
core_inc_dir = include_directories('include/Core/Include',
                                  '/usr/include')
cmsis_dev_inc = include_directories('ventor/cmsis_device_g0/Include')
hal_dev_inc = include_directories('ventor/stm32g0xx_hal_driver/Inc',
                                  'ventor/stm32g0xx_hal_driver/Inc/Legacy')

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  # Adjust paths based on your structure 
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
lib_dirs = [core_inc_dir, hal_dev_inc, cmsis_dev_inc]


# Flags
MCU        = '-DSTM32G071xx'
LL         = '-DUSE_FULL_LL_DRIVER'
fast       = '-Ofast'
small      = '-Os'
full       = '-O3'
optimize   = small

objcopy                 = find_program('arm-none-eabi-objcopy')
strip                   = find_program('arm-none-eabi-strip')
size                    = find_program('arm-none-eabi-size')

# Executable target
elf = executable('testershit', src_files + src_deps,
                 # link_with: hal,
                 include_directories : lib_dirs + inc_dir,
                 override_options: [],
                 c_args : [MCU , LL, optimize],
                 link_args : ['-nostdlib', '-nostartfiles', '-nodefaultlibs',
                              '--data-sections', '-std=c23', optimize,
                              '--specs=nano.specs', '-Wl,-Map=output.map'],
                 build_by_default: true)
small = custom_target('Small sized binary',
                      input : elf,
                      output : 'small.bin',
                      command : [objcopy, '-O', 'binary', '@INPUT@', '@OUTPUT@'],
                      build_by_default: true)
striper = custom_target('stripped-binary',
                        input: elf,
                        output: 'stripped.elf',
                        command: [strip, '--strip-all', '@INPUT@', '-o', '@OUTPUT@'])

release = executable('release', src_files + src_deps,
                     include_directories : lib_dirs + inc_dir,
                     override_options: [],
                     c_args : [MCU , LL, fast],
                     link_args : ['-nostdlib', '-nostartfiles', '-nodefaultlibs',
                                  '--data-sections', '-std=c23', fast,
                                  '--specs=nano.specs', '-Wl,-Map=output.map'],
                     build_by_default: false)
strip_rel = custom_target('stripped-rel',
                          input: release,
                          output: 'stripped-rel.elf',
                          command: [strip, '--strip-all', '@INPUT@', '-o', '@OUTPUT@'],
                          build_by_default: false)
rel_bin = custom_target('release',
                        input : release,
                        output : 'rel.bin',
                        command : [objcopy, '-O', 'binary', '@INPUT@', '@OUTPUT@'],
                        build_by_default: false)

run_target(
  'colapse',
  command: ['ninja', 'clean', '&&',
            'ninja', '&&',
            'ninja', 'clean', '-C', '../test/build/', '&&',
            'ninja', 'test', '-v', '-C', '../test/build/'],)

run_target(
  'bin_size',
  command: [size, '--format=sysv', '--common', 'stripped.elf', 'stripped-rel.elf'],
  depends: [striper, strip_rel])

flash = find_program('st-flash')
run_target(
  'nuke',
  command: [flash, 'erase', '&&', flash, 'reset'],)

run_target(
  'sendit',
  command: [flash, 'write', 'small.bin', '0x08000000', '&&', flash, 'reset'],
  depends: [small])
run_target(
  'fast',
  command: [flash, 'write', 'rel.bin', '0x08000000', '&&', flash, 'reset'],
  depends: [rel_bin])
