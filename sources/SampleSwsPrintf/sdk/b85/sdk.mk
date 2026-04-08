################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 

OUT_DIR += /b85

OBJS += \
$(OUT_PATH)/b85/adc.o \
$(OUT_PATH)/b85/aes.o \
$(OUT_PATH)/b85/analog.o \
$(OUT_PATH)/b85/audio.o \
$(OUT_PATH)/b85/bsp.o \
$(OUT_PATH)/b85/clock.o \
$(OUT_PATH)/b85/flash.o \
$(OUT_PATH)/b85/gpio.o \
$(OUT_PATH)/b85/i2c.o \
$(OUT_PATH)/b85/lpc.o \
$(OUT_PATH)/b85/qdec.o \
$(OUT_PATH)/b85/s7816.o \
$(OUT_PATH)/b85/spi.o \
$(OUT_PATH)/b85/timer.o \
$(OUT_PATH)/b85/uart.o \
$(OUT_PATH)/b85/usbhw.o \
$(OUT_PATH)/b85/watchdog.o \
$(OUT_PATH)/b85/div_mod.o \
$(OUT_PATH)/b85/cstartup_flash.o

BOOT_FLAG := $(TEL_CHIP)

# Each subdirectory must supply rules for building sources it contributes
$(OUT_PATH)/b85/%.o: $(TEL_PATH)/%.c
	@echo 'Building file: $<'
	@$(TC32_PATH)tc32-elf-gcc $(GCC_FLAGS) $(INCLUDE_PATHS) -c -o"$@" "$<"

$(OUT_PATH)/b85/%.o: $(TEL_PATH)/%.S
	@echo 'Building file: $<'
	@$(TC32_PATH)tc32-elf-gcc $(INCLUDE_PATHS) $(BOOT_FLAG) -c -o"$@" "$<"
