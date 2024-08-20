##
# Trying C for modules
#
# @file
# @version 0.1

# -include Drivers/STM32G0xx_HAL_Driver/subdir.mk
# -include Drivers/CMSIS/subdir.mk
# -include Drivers/BSP/STM32G0xx_Nucleo/subdir.mk

TARGET = bin/test
LINKER = STM32G071RBTX_FLASH.ld

SRC = $(wildcard *.c)
HDR = $(wildcard *.h)

OBJ = $(patsubst %.c, obj/%.o, $(SRC))

#
# TODO: build the files properly and make sure all files are getting build..
# for now the truth is that this aint in a good state (linker errors) but i go
# vacatione!!
#
#
# DEP_DIRS 	+= include/cmsis_device_g0/Include \
# 			include/stm32g0xx_hal_driver/Inc
DEP_DIRS	:= ventor/stm32g0xx_hal_driver/Inc
DEP_SRCS	:= ventor/stm32g0xx_hal_driver/Src



RM 		:= rm -f
echo 	:= echo -e
RED_BLD := '\033[1;31m'
NOC_BLD := '\033[1;0m'
CC 		:= arm-none-eabi-gcc
CPU		:= cortex-m0plus
ISA 	:= thumb

FLAGS 	+= \
		--specs=nosys.specs \
		-Wl,--gc-sections \
		-static \
		--specs=nano.specs \
		-mfloat-abi=soft \
		-m$(ISA) \
		-Wl,--start-group \
		-lc -lm \
		-Wl,--end-group

all: default all_libs

default: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) -o $@ $? -I$(DEP_DIRS) -mcpu=$(CPU) -T"$(LINKER)" $(FLAGS)

obj/%.o: src/%.c $(all_libs)
	$(CC) -c $< -I$(DEP_DIRS) -mcpu=$(CPU) -T"$(LINKER)" $(FLAGS) -g -o $@

all_libs: $(DEP_SRCS)/*.c
	$(CC) -c $< -I$(DEP_DIRS) -mcpu=$(CPU) -T"$(LINKER)" $(FLAGS) -g -o $@

clean:
	@$(RM) 	bin/* obj/*.o
	@$(echo) "${RED_BLD}Deleted old .o and binary files!!${NOC_BLD}"
	make -j8
# end
