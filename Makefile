# ------------------------------------------------
# Generic Makefile for STM32F411RE (UART_DMA)
# ------------------------------------------------

# Target execution name
TARGET = SENSOR_NODE

# Build directory
BUILD_DIR = build

# C sources
# This grabs all .c files in the src folder. 
# If you have a Drivers/ folder, you will need to add it here (e.g., $(wildcard Drivers/.../*.c))
C_SOURCES = \
$(wildcard src/*.c)

# ASM sources 
# Assumes your startup file is in the src directory and ends in .s
ASM_SOURCES = \
$(wildcard src/*.s)

# ------------------------------------------------
# Toolchain setup
# ------------------------------------------------
PREFIX = arm-none-eabi-
CC = $(PREFIX)gcc
AS = $(CC) -x assembler-with-cpp
SZ = $(PREFIX)size
OD = $(PREFIX)objdump

# ------------------------------------------------
# MCU and Compiler Flags (Extracted from your logs)
# ------------------------------------------------
# cpu, fpu, float-abi, thumb
MCU = -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard

# Include directories
C_INCLUDES = \
-Iinc

# Compile flags
# Note: Added -ffunction-sections and -fdata-sections so that the linker's --gc-sections works efficiently
CFLAGS = $(MCU) $(C_INCLUDES) -O0 -g3 -Wall -ffunction-sections -fdata-sections

# ------------------------------------------------
# Linker Flags
# ------------------------------------------------
# Make sure this file is in the same directory as the Makefile!
LDSCRIPT = STM32F411RETX_FLASH.ld

LDFLAGS = $(MCU) -T$(LDSCRIPT) --specs=nosys.specs -Wl,-Map=$(BUILD_DIR)/$(TARGET).map -Wl,--gc-sections -static --specs=nano.specs -Wl,--start-group -lc -lm -Wl,--end-group

# ------------------------------------------------
# Build Rules
# ------------------------------------------------

# Generate list of object files
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))
vpath %.s $(sort $(dir $(ASM_SOURCES)))

# Default target
all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).list

# Compile C files
$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	@echo "Compiling $<..."
	@$(CC) -c $(CFLAGS) $< -o $@

# Compile Assembly files
$(BUILD_DIR)/%.o: %.s | $(BUILD_DIR)
	@echo "Assembling $<..."
	@$(AS) -c $(CFLAGS) $< -o $@

# Link object files to create .elf
$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) | $(BUILD_DIR)
	@echo "Linking $@..."
	@$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	@echo "------------------------------------------------"
	@$(SZ) $@
	@echo "------------------------------------------------"

# Generate the .list file (objdump)
$(BUILD_DIR)/$(TARGET).list: $(BUILD_DIR)/$(TARGET).elf
	@echo "Generating $@..."
	@$(OD) -h -S $< > $@

# Create build directory
$(BUILD_DIR):
	mkdir $@

# Clean up
clean:
	@echo "Cleaning build directory..."
	rm -rf $(BUILD_DIR)

.PHONY: all clean
