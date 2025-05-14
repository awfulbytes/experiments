project('tryC', 'c',
  version : '0.1.0',
  meson_version : '>=0.60.0')

# Include directories
# Source files
src_files = files(
  'drivers/dac.c',
  'drivers/timx.c',
  'drivers/adc.c',
  'drivers/dma.c',
  'src/interrupter.c',
  'src/startup_stm32g071xx.s',
  'src/system_stm32g0xx.c',
  'src/ui.c',
  'src/nco.c',
  'src/stm32g0xx_it.c',
  # 'src/msp_init.c'
  )
src_deps = files(
  'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_ll_adc.c',
  # 'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_ll_comp.c',
  # 'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_ll_crc.c',
  # 'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_ll_crs.c',
  'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_ll_dac.c',
  'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_ll_dma.c',
  'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_ll_exti.c',
  'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_ll_gpio.c',
  # 'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_ll_i2c.c',
  'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_ll_lptim.c',
  # 'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_ll_lpuart.c',
  'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_ll_pwr.c',
  'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_ll_rcc.c',
  # 'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_ll_rtc.c',
  # 'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_ll_spi.c',
  'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_ll_tim.c',
  # 'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_ll_ucpd.c',
  'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_ll_usart.c',
  'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_ll_usb.c',
  'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_ll_utils.c',
  # 'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_hal_gpio.c',
  # 'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_hal_cortex.c',
)
arm_sources = []
arm_sources += files(
  'ventor/cmsis-core/DSP/Source/FastMathFunctions/FastMathFunctions.c',
  'ventor/cmsis-core/DSP/Source/CommonTables/arm_common_tables.c',
)
inc_dir = include_directories('include')
core_inc_dir = include_directories('include/Core/Include',
                                  '/usr/include')
cmsis_dev_inc = include_directories('ventor/cmsis_device_g0/Include',
                                   'ventor/cmsis-core/DSP/Include',
                                   )
hal_dev_inc = include_directories('ventor/stm32g0xx_hal_driver/Inc',
                                  'ventor/stm32g0xx_hal_driver/Inc/Legacy')

lib_dirs = [core_inc_dir, hal_dev_inc, cmsis_dev_inc]  # Adjust paths based on your structure


# Libraries (e.g., HAL or LL) as git submodules
# lib_includes = [core_inc_dir, cmsis_dev_inc, hal_dev_inc]
# foreach d : lib_dirs
#   lib_includes += include_directories(d)
# endforeach

# Executable target
elf = executable('testershit', src_files + src_deps,
                 # link_with: hal,
                 include_directories : lib_dirs + inc_dir,
                 override_options: [],
                 link_args : ['-nostdlib', '-nostartfiles', '-nodefaultlibs',
                              '--data-sections', '-std=c23',
                              '--specs=nano.specs', '-Wl,-Map=output.map'],
                 build_by_default: true
                )

# Custom target to generate the binary and hex files
objcopy = find_program('arm-none-eabi-objcopy')
strip = find_program('arm-none-eabi-strip')
size = find_program('arm-none-eabi-size')
custom_target('stm32_app.bin',
              input : elf,
              output : 'stm32_app.bin',
              command : [objcopy, '-O', 'binary', '@INPUT@', '@OUTPUT@'],
              build_by_default: true
             )
striper = custom_target('stripped-binary',
              input: elf,
              output: 'stripped.elf',
              command: [strip, '--strip-all', '@INPUT@', '-o', '@OUTPUT@'],
              # build_by_default: true
             )
custom_target('stm32_app.hex',
              input : elf,
              output : 'stm32_app.hex',
              command : [objcopy, '-O', 'ihex', '@INPUT@', '@OUTPUT@'],
              build_by_default: true
             )
custom_target('stm32.bin',
              input: striper,
              output: 'striper.bin',
              command : [objcopy, '-O', 'binary', '@INPUT@', '@OUTPUT@'],
              build_by_default: true
             )

run_target(
  'bin_size',
  command: [size, '--format=sysv', '--common', 'testershit', 'stripped.elf'],
)
flash = find_program('st-flash')
run_target(
  'nuke',
  command: [flash, 'erase', '&&', flash, 'reset'],
)
run_target(
  'sendit',
  command: [flash, 'write', 'stm32_app.bin', '0x08000000'],
)
