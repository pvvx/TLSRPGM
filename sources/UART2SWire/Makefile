#

TARGET  = urat2swire

# This assumes that you install Telink toolchain for Windows, MacOS or Linux
# Under Windows you will be using MingW/Msys environment
#
GCCPATH = /opt/tc32/bin/tc32
AS      = $(GCCPATH)-elf-as
CC      = $(GCCPATH)-elf-gcc
LD      = $(GCCPATH)-elf-ld
NM      = $(GCCPATH)-elf-nm
OBJCOPY = $(GCCPATH)-elf-objcopy
OBJDUMP = $(GCCPATH)-elf-objdump
ARCH	= $(GCCPATH)-elf-ar
SIZE	= $(GCCPATH)-elf-size

CRT0=./mcu/cstartup_825x_floader.S

WARNINGS=-Wall -Wextra -Wshadow -Wpointer-arith -Wbad-function-cast -Wcast-align -Wsign-compare \
		-Waggregate-return -Wunused

OTHER_CFLAGS= -fdata-sections -ffunction-sections \
-fpack-struct -fshort-enums -finline-small-functions -fshort-wchar -fms-extensions


# This allow you to use portDEBUG_PRINTF()
DEBUG_FLAGS = -DTDEBUG=1

# It's unreal for people at this stage and still  using old C standards - PS
CFLAGS= -O2 $(WARNINGS) $(DEBUG_FLAGS) $(OTHER_FLAGS) -std=gnu99 -I. -I./Include
	

LINKER_FLAGS=-Xlinker -o$(TARGET).abs -Xlinker -M -Xlinker -Map=$(TARGET).map

LDFLAGS = -Wl,--gc-sections -T ./mcu/boot_825x.link

SOURCE_DIR=./Source

TC32_SRC = \
$(SOURCE_DIR)/main.c \
$(SOURCE_DIR)/crc.c \
$(SOURCE_DIR)/clock.c \
$(SOURCE_DIR)/analog.c \
$(SOURCE_DIR)/swire.c


#
# Define all object files.
#
TC32_OBJ = $(TC32_SRC:.c=.o)


all: $(TARGET).elf


$(TARGET).elf : $(ASM_OBJ) $(TC32_OBJ) $(CRT0) 
	$(CC) $(CFLAGS) $(TC32_OBJ)  -nostartfiles -nostdlib $(CRT0) $(LDFLAGS) $(LINKER_FLAGS)
	$(OBJCOPY) -O binary $(TARGET).abs $(TARGET).bin
	$(OBJCOPY) -O elf32-littletc32  $(TARGET).abs $(TARGET).elf
	$(OBJDUMP) -x -D -l -S $(TARGET).abs > $(TARGET).lst
	$(SIZE) --format=SysV $(TARGET).elf

$(TC32_OBJ) : %.o : %.c $(LDSCRIPT)
	$(CC) -c $(CFLAGS) $< -o $@
	$(OBJDUMP) -x -D -l -S $*.o > $*.lst
	
print-%  : ; @echo $* = $($*)

clean :
	rm -rf $(TC32_OBJ) *.map *.mot *.elf *.a *.abs *.i *.s *.list
	find $(SOURCE_DIR) -name "*.lst" -exec rm {} \;
	find ./ -name "*.o" -exec rm {} \;
	find ./ -name "*.a" -exec rm {} \;
	find ./ -name "cscope.*" -exec rm {} \;
	find ./ -name "tags" -exec rm {} \;
	touch Makefile
