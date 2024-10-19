# project('tryC', 'c')
# executable('testing', 'main.c')
project('tryC', 'c',
  version : '0.1',
  meson_version : '>=0.60.0')

# Include directories
# Source files
src_files = files(
  'src/main.c',
  'src/startup_stm32g071xx.s',
  'src/system_stm32g0xx.c',
  'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_hal.c',
  # 'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_hal_adc.c',
  # 'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_hal_adc_ex.c',
  # 'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_hal_cec.c',
  # 'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_hal_comp.c',
  'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_hal_cortex.c',
  # 'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_hal_crc.c',
  # 'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_hal_crc_ex.c',
  # 'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_hal_cryp.c',
  # 'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_hal_cryp_ex.c',
  'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_hal_dac.c',
  'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_hal_dac_ex.c',
  'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_hal_dma.c',
  'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_hal_dma_ex.c',
  'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_hal_exti.c',
  # 'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_hal_fdcan.c',
  # 'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_hal_flash.c',
  # 'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_hal_flash_ex.c',
  'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_hal_gpio.c',
  # 'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_hal_hcd.c',
  # 'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_hal_i2c.c',
  # 'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_hal_i2c_ex.c',
  # 'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_hal_i2s.c',
  # 'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_hal_irda.c',
  # 'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_hal_iwdg.c',
  # 'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_hal_lptim.c',
  # 'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_hal_msp_template.c',
  # 'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_hal_pcd.c',
  # 'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_hal_pcd_ex.c',
  'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_hal_pwr.c',
  'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_hal_pwr_ex.c',
  'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_hal_rcc.c',
  # 'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_hal_rcc_ex.c',
  # 'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_hal_rng.c',
  # 'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_hal_rtc.c',
  # 'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_hal_rtc_ex.c',
  # 'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_hal_smartcard.c',
  # 'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_hal_smartcard_ex.c',
  # 'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_hal_smbus.c',
  # 'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_hal_smbus_ex.c',
  # 'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_hal_spi.c',
  # 'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_hal_spi_ex.c',
  'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_hal_tim.c',
  # 'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_hal_timebase_rtc_alarm_template.c',
  # 'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_hal_timebase_rtc_wakeup_template.c',
  # 'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_hal_timebase_tim_template.c',
  'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_hal_tim_ex.c',
  # 'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_hal_uart.c',
  # 'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_hal_uart_ex.c',
  # 'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_hal_usart.c',
  # 'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_hal_usart_ex.c',
  # 'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_hal_wwdg.c',
  # 'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_ll_adc.c',
  # 'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_ll_comp.c',
  # 'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_ll_crc.c',
  # 'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_ll_crs.c',
  'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_ll_dac.c',
  'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_ll_dma.c',
  'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_ll_exti.c',
  'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_ll_gpio.c',
  # 'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_ll_i2c.c',
  # 'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_ll_lptim.c',
  # 'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_ll_lpuart.c',
  'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_ll_pwr.c',
  'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_ll_rcc.c',
  # 'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_ll_rng.c',
  # 'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_ll_rtc.c',
  # 'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_ll_spi.c',
  'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_ll_tim.c',
  # 'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_ll_ucpd.c',
  # 'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_ll_usart.c',
  # 'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_ll_usb.c',
  # 'ventor/stm32g0xx_hal_driver/Src/stm32g0xx_ll_utils.c',
)
inc_dir = include_directories('include')
core_inc_dir = include_directories('include/Core/Include')
cmsis_dev_inc = include_directories('ventor/cmsis_device_g0/Include')
hal_dev_inc = include_directories('ventor/stm32g0xx_hal_driver/Inc', 'ventor/stm32g0xx_hal_driver/Inc/Legacy')


# Libraries (e.g., HAL or LL) as git submodules
lib_dirs = [core_inc_dir, hal_dev_inc, cmsis_dev_inc]  # Adjust paths based on your structure
# lib_includes = [core_inc_dir, cmsis_dev_inc, hal_dev_inc]
# foreach d : lib_dirs
#   lib_includes += include_directories(d)
# endforeach

# Executable target
elf = executable('testershit', src_files,
  include_directories : lib_dirs + inc_dir,
  link_args : ['-nostdlib', '-nostartfiles', '-nodefaultlibs'],
  override_options : ['c_args=-Os', 'c_link_args=-Wl,--gc-sections'],
  build_by_default: true
)

# Custom target to generate the binary and hex files
objcopy = find_program('arm-none-eabi-objcopy')
custom_target('stm32_app.bin',
  input : elf,
  output : 'stm32_app.bin',
  command : [objcopy, '-O', 'binary', '@INPUT@', '@OUTPUT@'],
  build_by_default: true
)

custom_target('stm32_app.hex',
  input : elf,
  output : 'stm32_app.hex',
  command : [objcopy, '-O', 'ihex', '@INPUT@', '@OUTPUT@'],
  build_by_default: true
)
