TARGET := testapp
TOOLCHAIN := arm-none-eabi

CUSTOM_DEFINE := -DUART_USE_USART1

FREERTOS_MODULE := tasks queue list timers
HAL_MODULE := cortex gpio rcc rcc_ex uart

BUILD_DIR := build

STFLASH := ~/bin/st-flash
OPENOCD := openocd

# Add include path here
INC_PATH := 
INC_PATH += ./include

# ---
space :=
space +=

COMMON_DIR := lib/common
INC_PATH += $(COMMON_DIR)/include

# FreeRTOS setting
FREERTOS_PATH := lib/FreeRTOS/Source

INC_PATH += $(FREERTOS_PATH)/include
INC_PATH += $(FREERTOS_PATH)/portable/GCC/ARM_CM4F
FREERTOS_CFILES := $(addprefix $(FREERTOS_PATH)/,$(addsuffix .c,$(FREERTOS_MODULE)))
FREERTOS_CFILES += $(FREERTOS_PATH)/portable/GCC/ARM_CM4F/port.c

# Change FreeRTOS memory management code here (http://www.freertos.org/a00111.html)
FREERTOS_CFILES += $(FREERTOS_PATH)/portable/MemMang/heap_1.c

INC_PATH += $(FREERTOS_PATH)/CMSIS_RTOS
FREERTOS_CFILES += $(FREERTOS_PATH)/CMSIS_RTOS/cmsis_os.c

# HAL setting
HAL_PATH := lib//Drivers/HAL
INC_PATH += $(HAL_PATH)/Inc
HAL_CFILES := $(HAL_PATH)/Src/stm32f3xx_hal.c
HAL_CFILES += $(addprefix $(HAL_PATH)/Src/stm32f3xx_hal_,$(addsuffix .c,$(HAL_MODULE)))

# CMSIS setting
CMSIS_PATH := lib/CMSIS/
INC_PATH += $(CMSIS_PATH)/Device/ST/STM32F3xx/Include
INC_PATH += $(CMSIS_PATH)/Include

# Driver setting
DRIVER_PATH := lib/Drivers
INC_PATH += $(DRIVER_PATH)/BSP/STM32F3-Discovery
DRIVER_CFILES := $(DRIVER_PATH)/BSP/STM32F3-Discovery/stm32f3_discovery.c

# Toolchain setting
CC := $(shell { which $(TOOLCHAIN)-gcc; } 2>/dev/null)
GDB := $(shell { which $(TOOLCHAIN)-gdb; } 2>/dev/null)
OBJCOPY := $(shell { which $(TOOLCHAIN)-objcopy; } 2>/dev/null)
CFLAGS := -D STM32F303xC $(CUSTOM_DEFINE) -g -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=softfp  -Os -ffunction-sections -fdata-sections -std=c99 $(subst $(space), -I,$(INC_PATH))
LDFLAGS := -Wl,--gc-sections -Wl,-Map=$(BUILD_DIR)/$(TARGET).map -T $(COMMON_DIR)/stm32f303vc.ld

vpath %.ld $(COMMON_DIR)
vpath %.s $(COMMON_DIR)
vpath %.c src 
vpath %.h include $(COMMON_DIR) 

.PHONY: clean flash ocd gdb

default: $(BUILD_DIR)/$(TARGET).bin

SOURCES := $(wildcard src/*.c) $(FREERTOS_CFILES) $(DRIVER_CFILES) $(HAL_CFILES)
OBJECTS := $(patsubst %.c,$(BUILD_DIR)/%.o,$(SOURCES))
DEPENDS := $(patsubst %.c,$(BUILD_DIR)/%.d,$(SOURCES))

ifneq ($(MAKECMDGOALS),clean)
-include $(DEPENDS)
endif

$(BUILD_DIR)/$(TARGET).bin: $(BUILD_DIR)/$(TARGET).elf
	$(OBJCOPY) $^ -Obinary $@

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) startup.s
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o$@

$(BUILD_DIR)/%.d: %.c
	@mkdir -p $(shell { dirname $@; })
	$(CC) -MM $(CFLAGS) $< | sed -e 's/.*:/$(BUILD_DIR)\/$(subst /,\/,$*).o:/' > $@

$(BUILD_DIR)/%.o: $(BUILD_DIR)/%.d
	@mkdir -p $(shell { dirname $@; })
	$(CC) -c $(CFLAGS) $*.c -o $@

ocd: $(BUILD_DIR)/$(TARGET).bin
	$(OPENOCD) -f $(COMMON_DIR)/stm32f3discovery.cfg

# After launching GDB, set the breakpoint and type "continue"
# You may also need to press reset button on the board.
gdb: $(BUILD_DIR)/$(TARGET).elf
	$(GDB) -tui -x $(COMMON_DIR)/start-gdb.cmd $^

flash: $(BUILD_DIR)/$(TARGET).bin
	$(STFLASH) write $^ 0x8000000

clean:
	-rm -rf $(BUILD_DIR)