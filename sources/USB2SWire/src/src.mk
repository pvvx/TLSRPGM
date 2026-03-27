
INC_PATH += $(SRC_PATH)/inc

OUT_DIR += $(SRC_DIR) $(SRC_DIR)/usb/ $(SRC_DIR)/mcu/  

OBJS += \
$(OUT_PATH)$(SRC_DIR)/main.o \
$(OUT_PATH)$(SRC_DIR)/swire.o \
$(OUT_PATH)$(SRC_DIR)/crc.o \
$(OUT_PATH)$(SRC_DIR)/utils.o \
$(OUT_PATH)$(SRC_DIR)/analog.o \
$(OUT_PATH)$(SRC_DIR)/flash.o \
$(OUT_PATH)$(SRC_DIR)/usb/usb_cdc.o \
$(OUT_PATH)$(SRC_DIR)/usb/usb_ctrl.o \
$(OUT_PATH)$(SRC_DIR)/usb/usb_desc.o \
$(OUT_PATH)$(SRC_DIR)/usb/usb_hw.o \
$(OUT_PATH)$(SRC_DIR)/mcu/div_mod.o \
$(OUT_PATH)$(SRC_DIR)/mcu/cstartup_826x.o 

# Each subdirectory must supply rules for building sources it contributes
$(OUT_PATH)$(SRC_DIR)/%.o: $(PROJECT_PATH)$(SRC_DIR)/%.c
	@echo 'Building file: $<'
	@$(TC32_PATH)tc32-elf-gcc $(GCC_FLAGS) $(INCLUDE_PATHS) -c -o"$@" "$<"
	
$(OUT_PATH)$(SRC_DIR)/%.o: $(PROJECT_PATH)$(SRC_DIR)/%.S
	@echo 'Building file: $<'
	@$(TC32_PATH)tc32-elf-gcc $(GCC_FLAGS) $(ASM_FLAGS) $(INCLUDE_PATHS) -c -o"$@" "$<"