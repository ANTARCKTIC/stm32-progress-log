TARGET = blink_nucleo

CC = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
SIZE = arm-none-eabi-size

# --- EDIT THESE PATHS to match your actual F411 folder names ---
INC_DIR = ../../common/F411/STMicroelectronics-cmsis-device-f4-master-Include
CMSIS_INC_DIR = ../../common/F411/CMSIS_Include
STARTUP = ../../common/F411/STMicroelectronics-cmsis-device-f4 master Source-Templates/gcc/startup_stm32f411xe.s
SYSTEM_SRC = ../../common/F411/STMicroelectronics-cmsis-device-f4 master Source-Templates/system_stm32f4xx.c
LDSCRIPT = ../../common/F411/F411_Linker.ld

SRCS = src/main.c $(STARTUP) $(SYSTEM_SRC)

CPU = -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard
DEFS = -DSTM32F411xE
INCLUDES = -I"$(INC_DIR)" -I"$(CMSIS_INC_DIR)"

CFLAGS = $(CPU) $(DEFS) $(INCLUDES) -Wall -O0 -g -ffunction-sections -fdata-sections
LDFLAGS = $(CPU) -T"$(LDSCRIPT)" -Wl,--gc-sections -specs=nano.specs -specs=nosys.specs

all: $(TARGET).elf $(TARGET).bin $(TARGET).hex size

$(TARGET).elf: $(SRCS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(SRCS) -o $@

$(TARGET).bin: $(TARGET).elf
	$(OBJCOPY) -O binary $< $@

$(TARGET).hex: $(TARGET).elf
	$(OBJCOPY) -O ihex $< $@

size: $(TARGET).elf
	$(SIZE) $(TARGET).elf

flash: $(TARGET).elf
	openocd -f interface/stlink.cfg -f target/stm32f4x.cfg -c "program $(TARGET).elf verify reset exit"

clean:
	rm -f $(TARGET).elf $(TARGET).bin $(TARGET).hex

.PHONY: all size flash clean