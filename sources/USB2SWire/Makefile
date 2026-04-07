
PROJECT_NAME ?= usb2swire
VERSION_BIN ?=

# E104_BT10 set POJECT_DEF=-DSET_PLL=QUARTZ_12MHZ 
# E180_Z6907A set POJECT_DEF=-DSET_PLL=QUARTZ_12MHZ 

PGM_PORT?=COM15
PGM_PORT_BAUD?=1500000

PROJECT_PATH ?= .
SRC_DIR ?= /src
MAKE_PATH ?= ./make
OUT_PATH ?=./build
BIN_PATH ?=./bin
LS_FLAGS ?= $(SRC_PATH)/mcu/boot.link
LIBS ?=
TEL_PATH ?= .

SRC_PATH ?= $(PROJECT_PATH)$(SRC_DIR)

OBJ_SRCS :=
S_SRCS :=
ASM_SRCS :=
C_SRCS :=
S_UPPER_SRCS :=
O_SRCS :=
FLASH_IMAGE :=
ELFS :=
OBJS :=
LST :=
OUT_DIR :=

COMPILEOS = $(shell uname -o)
LINUX_OS = GNU/Linux

ifeq ($(COMPILEOS),$(LINUX_OS))
	PYTHON ?= python3
	CUR_OS := linux
	TOOLS_PATH := $(TEL_PATH)/tools/linux
else
	PYTHON ?= python
	CUR_OS := windows
	TOOLS_PATH := $(TEL_PATH)/tools/windows
ifeq ($(TOOLS_PATH)tc32/bin/tc32-elf-gcc.exe, $(wildcard $(TOOLS_PATH)tc32/bin/tc32-elf-gcc.exe))
	TC32_PATH := $(TOOLS_PATH)tc32/bin/
endif
endif

LNK_FLAGS := --gc-sections -nostartfiles

GCC_FLAGS := \
-O2 \
-ffunction-sections \
-fdata-sections \
-Wall \
-fpack-struct \
-fshort-enums \
-finline-small-functions \
-std=gnu99 \
-funsigned-char \
-fshort-wchar \
-fms-extensions \
-nostartfiles \
-nostdlib

ASM_FLAGS := \
-fomit-frame-pointer \
-fshort-enums \
-Wall \
-Wpacked \
-Wcast-align \
-fdata-sections \
-ffunction-sections \
-fno-use-cxa-atexit \
-fno-rtti \
-fno-threadsafe-statics


GCC_FLAGS += $(POJECT_DEF)

-include $(SRC_PATH)/src.mk

INCLUDE_PATHS := -I$(INC_PATH) 

# Add inputs and outputs from these tool invocations to the build variables
LST_FILE := $(OUT_PATH)/$(PROJECT_NAME).lst
BIN_FILE := $(BIN_PATH)/$(PROJECT_NAME)$(VERSION_BIN).bin
ELF_FILE := $(OUT_PATH)/$(PROJECT_NAME).elf

SIZEDUMMY := sizedummy

# All Target
all: pre-build main-build

# Main-build Target
main-build: $(ELF_FILE) secondary-outputs

OBJ_LIST := $(OBJS) $(USER_OBJS)
# Tool invocations
$(ELF_FILE): $(OBJ_LIST)
	@echo 'Building Standard target: $@'
	@$(TC32_PATH)tc32-elf-ld $(LNK_FLAGS) $(LS_INCLUDE) -T$(LS_FLAGS) -o $(ELF_FILE) $(OBJ_LIST) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

$(LST_FILE): $(ELF_FILE)
	@echo 'Invoking: TC32 Create Extended Listing'
	@$(TC32_PATH)tc32-elf-objdump -x -D -l -S  $(ELF_FILE)  > $(LST_FILE)
	@echo 'Finished building: $@'
	@echo ' '

$(BIN_FILE): $(ELF_FILE)
	@echo 'Create Flash image (binary format)'
	@echo ' '
	@$(TC32_PATH)tc32-elf-objcopy -v -O binary $(ELF_FILE) $(BIN_FILE)
	@echo 'Finished building: $@'
	@echo ' '

sizedummy: $(ELF_FILE)
	@$(PYTHON) $(MAKE_PATH)/TlsrMemInfo.py $(ELF_FILE)
	@echo ' '

clean:
	@$(RM) $(FLASH_IMAGE) $(ELFS) $(OBJS) $(LST) $(SIZEDUMMY) $(ELF_FILE) $(BIN_FILE) $(LST_FILE)
	@echo 'Clean ...'
	@echo ' '

pre-build:
	@mkdir -p $(foreach s,$(OUT_DIR),$(OUT_PATH)$(s))
	@mkdir -p $(BIN_PATH)
	@echo ' '

secondary-outputs: $(BIN_FILE) $(OTA_FILE) $(LST_FILE) $(SIZEDUMMY)


flash: $(BIN_FILE)
	@$(PYTHON) $(MAKE_PATH)/TlsrPgm.py -p$(PGM_PORT) -b$(PGM_PORT_BAUD) -u -z15 -a-50 -s we 0 $(BIN_FILE)
	@$(PYTHON) $(MAKE_PATH)/TlsrPgm.py -p$(PGM_PORT) -b$(PGM_PORT_BAUD) -u -m i

reset:
	@$(PYTHON) $(MAKE_PATH)/TlsrPgm.py -p$(PGM_PORT) -b$(PGM_PORT_BAUD) -u -z15 -m -w i

erase:
	@$(PYTHON) $(MAKE_PATH)/TlsrPgm.py -p$(PGM_PORT) -b$(PGM_PORT_BAUD) -u -z15 -s ea

info:
	@$(PYTHON) $(MAKE_PATH)/TlsrPgm.py -p$(PGM_PORT) -b$(PGM_PORT_BAUD) -u -z15 i

stop:
	@$(PYTHON) $(MAKE_PATH)/TlsrPgm.py -p$(PGM_PORT) -b$(PGM_PORT_BAUD) -u -z15 -s i

go:
	@$(PYTHON) $(MAKE_PATH)/TlsrPgm.py -p$(PGM_PORT) -b$(PGM_PORT_BAUD) -u -m

TADDR?=0x20000
TLEN?=128
test_damp:
	@$(PYTHON) $(MAKE_PATH)/TlsrPgm.py -p$(PGM_PORT) -b$(PGM_PORT_BAUD) -u -z10 -c -g ds $(TADDR) $(TLEN)

install: $(SDK_FLAGS) $(TC32_PATH)
	@echo "SDK & TC32 installed."
	@echo "Use: make clean"
	@echo "     make all"

$(TC32_PATH): $(TOOLS_PATH)
ifneq ($(TC32_PATH),$(wildcard $(TC32_PATH)))
ifeq (linux,$(CUR_OS))
	#@wget -P $(TOOLS_PATH) http://shyboy.oss-cn-shenzhen.aliyuncs.com/readonly/tc32_gcc_v2.0.tar.bz2 
	@tar -xvjf $(TOOLS_PATH)/tc32_gcc_v2.0.tar.bz2 -C $(TOOLS_PATH)
else
	@unzip -o $(TOOLS_PATH)/tc32.zip -d $(TOOLS_PATH)
	@echo Use "Telink IoT Studio"! - http://wiki.telink-semi.cn/wiki/IDE-and-Tools/Telink_IoT_Studio/
endif
endif

$(TOOLS_PATH):
	mkdir -p $(TOOLS_PATH)


.PHONY: all clean
.SECONDARY: main-build
