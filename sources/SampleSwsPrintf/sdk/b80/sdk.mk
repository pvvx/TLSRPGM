################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 

OUT_DIR += /b80

OBJS += \
$(OUT_PATH)/b80/adc.o \
$(OUT_PATH)/b80/aes.o \
$(OUT_PATH)/b80/analog.o \
$(OUT_PATH)/b80/bsp.o \
$(OUT_PATH)/b80/clock.o \
$(OUT_PATH)/b80/flash.o \
$(OUT_PATH)/b80/gpio.o \
$(OUT_PATH)/b80/i2c.o \
$(OUT_PATH)/b80/ir_learn.o \
$(OUT_PATH)/b80/keyscan.o \
$(OUT_PATH)/b80/qdec.o \
$(OUT_PATH)/b80/s7816.o \
$(OUT_PATH)/b80/spi.o \
$(OUT_PATH)/b80/timer.o \
$(OUT_PATH)/b80/uart.o \
$(OUT_PATH)/b80/watchdog.o \
$(OUT_PATH)/b80/usbhw.o \
$(OUT_PATH)/b80/div_mod.o \
$(OUT_PATH)/b80/cstartup_flash.o


BOOT_FLAG := $(TEL_CHIP)
#-DMCU_STARTUP_FLASH -DMCU_CORE_B80=1 -DDUT_TEST -DALL_SRAM_CODE=1 -DSRAM_OTP_FLASH_HANDLE=1

# Each subdirectory must supply rules for building sources it contributes
$(OUT_PATH)/b80/%.o: $(TEL_PATH)/%.c
	@echo 'Building file: $<'
	@$(TC32_PATH)tc32-elf-gcc $(GCC_FLAGS) $(INCLUDE_PATHS) -c -o"$@" "$<"

$(OUT_PATH)/b80/%.o: $(TEL_PATH)/%.S
	@echo 'Building file: $<'
	@$(TC32_PATH)tc32-elf-gcc $(INCLUDE_PATHS) $(BOOT_FLAG) -c -o"$@" "$<"
