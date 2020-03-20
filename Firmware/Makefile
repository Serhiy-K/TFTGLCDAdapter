# ------------------------------------------------
# Generic Makefile (based on gcc)
# ------------------------------------------------

######################################
# target
######################################
TARGET = 3D-GLCD

#######################################
# paths
#######################################
BUILD_DIR = $(TARGET)/Debug/obj
OUT_DIR = $(TARGET)/Debug/bin

######################################
# source
######################################
# C sources
C_SOURCES =  \
	$(wildcard src/*.c) \
	$(wildcard cmsis_boot/*.c) \
	$(wildcard cmsis_boot/startup/*.c) \
	$(wildcard stm_lib/src/*.c)

# ASM sources
#ASM_SOURCES =

# CPP sources
#CPP_SOURCES =

#######################################
# binaries
#######################################
PREFIX = arm-none-eabi-
# The gcc compiler bin path can be either defined in make command via GCC_PATH variable (> make GCC_PATH=xxx)
# either it can be added to the PATH environment variable.
ifdef GCC_PATH
CC = $(GCC_PATH)/$(PREFIX)gcc
CPP= $(GCC_PATH)/$(PREFIX)g++
AS = $(GCC_PATH)/$(PREFIX)gcc -x assembler-with-cpp
CP = $(GCC_PATH)/$(PREFIX)objcopy
SZ = $(GCC_PATH)/$(PREFIX)size
else
CC = $(PREFIX)gcc
CPP = $(PREFIX)g++
AS = $(PREFIX)gcc -x assembler-with-cpp
CP = $(PREFIX)objcopy
SZ = $(PREFIX)size
endif
HEX = $(CP) -O ihex
BIN = $(CP) -O binary -S
 
#######################################
# CFLAGS
#######################################
# cpu
CPU = -mcpu=cortex-m3

# mcu
MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI)

# macros for gcc
# AS defines
AS_DEFS = 

# C defines
C_DEFS = -DUSE_STDPERIPH_DRIVER -DSTM32F103xB -DSTM32F10X_MD

# AS includes
AS_INCLUDES = 

# C includes
C_INCLUDES = \
	-Iinc \
	-Icmsis \
	-Icmsis_boot \
	-Istm_lib/inc

# CPP includes
#CPP_INCLUDES =

# optimization
OPT = -Og

# compile gcc flags
ASFLAGS = $(MCU) $(AS_DEFS) $(AS_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections
CFLAGS = $(MCU) $(C_DEFS) $(C_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections 
CPPFLAGS = $(CPP_INCLUDES) -std=c++14

CFLAGS += -g -gdwarf-2 

# Generate dependency information
CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)"

#######################################
# LDFLAGS
#######################################
# link script
LDSCRIPT = STM32F103C8Tx_FLASH.ld

# libraries
LIBS = -lstdc++ -lm -lc -lgcc -lnosys
LIBDIR = 
LDFLAGS = $(MCU) -specs=nano.specs -T$(LDSCRIPT) $(LIBDIR) $(LIBS) -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections -Wl,--start-group -Wl,--end-group

# default action: build all
all: $(OUT_DIR)/$(TARGET).elf $(OUT_DIR)/$(TARGET).hex $(OUT_DIR)/$(TARGET).bin

#######################################
# build the application
#######################################
OBJECTS =
# list of CPP program objects
#OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(CPP_SOURCES:.cpp=.o)))
#vpath %.cpp $(sort $(dir $(CPP_SOURCES)))
# list of objects
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))
# list of ASM program objects
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))
vpath %.s $(sort $(dir $(ASM_SOURCES)))


#$(BUILD_DIR)/%.o: %.s Makefile | $(BUILD_DIR)
#	@echo "[ AS ]  : $@"
#	$(AS) -c $(CFLAGS) $< -o $@

#$(BUILD_DIR)/%.o: %.cpp Makefile | $(BUILD_DIR) 
#	@echo "[ CPP ] : $@"
#	$(CPP) -c $(CFLAGS) $(CPPFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.cpp=.lst)) $< -o $@

$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR)
	@echo "[ CC ]  : $@" 
	$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@

$(OUT_DIR)/$(TARGET).elf: $(OBJECTS) Makefile
	@echo "[ ELF ] : $@"
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	$(SZ) $@

$(OUT_DIR)/%.hex: $(OUT_DIR)/%.elf | $(OUT_DIR)
	@echo "[ HEX ] : $@"
	$(HEX) $< $@
	
$(OUT_DIR)/%.bin: $(OUT_DIR)/%.elf | $(OUT_DIR)
	@echo "[ BIN ] : $@"
	$(BIN) $< $@
	
$(BUILD_DIR):
	mkdir $(BUILD_DIR)
	mkdir $(OUT_DIR)

#######################################
# clean up
#######################################
clean:
	del $(BUILD_DIR)/*.*
	del $(OUT_DIR)/*.*
  
#######################################
# dependencies
#######################################
-include $(wildcard $(BUILD_DIR)/*.d)
