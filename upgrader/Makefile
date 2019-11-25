TARGET = upgrader

VERSION := $(shell ./version.sh $(TARGET))

CROSS_COMPILE = arm-none-eabi-
CC = $(CROSS_COMPILE)gcc
AS = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)gcc
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdump

C_SRC = $(wildcard *.c) $(wildcard Drivers/STM32L0xx_HAL_Driver/Src/*.c)
S_SRC = $(wildcard *.s) $(wildcard *.S)
SRC = $(C_SRC) $(S_SRC)
OBJ = $(addsuffix .o, $(basename $(SRC)))
SU = $(addsuffix .su, $(basename $(SRC)))

OPT_CFLAGS = -g -O2
ARCH_CFLAGS = -mthumb -mcpu=cortex-m0plus

SPEC_CFLAGS = -specs=nano.specs

INCDIR = . config \
Drivers/STM32L0xx_HAL_Driver/Inc Drivers/STM32L0xx_HAL_Driver/Inc/Legacy \
Drivers/CMSIS/Device/ST/STM32L0xx/Include \
Drivers/CMSIS/Include

CFLAGS = $(OPT_CFLAGS) $(ARCH_CFLAGS) $(SPEC_CFLAGS) -std=gnu11 \
	 -ffunction-sections -fdata-sections -Wall -Werror \
	 -DUSE_HAL_DRIVER -DSTM32L071xx -D"__weak=__attribute__((weak))" \
	 -D__packed="__attribute__((__packed__))" \
	 -DVERSION='"$(VERSION)"' \
	 $(foreach dir, $(INCDIR), -I$(dir))

ASFLAGS = $(OPT_CFLAGS) $(ARCH_CFLAGS) $(SPEC_CFLAGS) -x assembler-with-cpp \
          -Wa,--no-warn

LDFLAGS = $(OPT_CFLAGS) $(ARCH_CFLAGS) $(SPEC_CFLAGS) -static \
	  -Wl,-Map,$(TARGET).map -Wl,--gc-sections

ifeq ($(strip $(V)), 1)
	Q =
	E =
else
	Q = @
	E = 1>/dev/null 2>&1
endif

all: $(TARGET).bin

$(TARGET).bin: $(TARGET).elf $(TARGET).dis
	@echo '[OBJCOPY] $@'
	$(Q)$(OBJCOPY) -O binary $< $@

%.o: %.c
	@echo '[CC] $@'
	$(Q)$(CC) $(CFLAGS) -c -o $@ $<

%.o: %.s
	@echo '[AS] $@'
	$(Q)$(AS) $(ASFLAGS) -c -o $@ $<

%.o: %.S
	@echo '[AS] $@'
	$(Q)$(AS) $(ASFLAGS) -c -o $@ $<

%.i: %.c
	@echo '[PP] $@'
	$(Q)$(CC) $(CFLAGS) -E -o $@ $<

$(TARGET).elf: $(OBJ) target.ld
	@echo '[LD] $@'
	$(Q)$(LD) $(OBJ) -T target.ld $(LDFLAGS) -o $@

%.dis: %.elf
	@echo '[OBJDUMP] $@'
	$(Q)$(OBJDUMP) -D $< > $@

.PHONY: clean distclean

clean:
	@echo '[CLEAN]'
	$(Q)-rm -rf $(OBJ)
	$(Q)-rm -rf $(SU)

distclean: clean
	@echo '[DISTCLEAN]'
	$(Q)-rm -rf $(TARGET).elf $(TARGET).dis $(TARGET).bin $(TARGET).map flash.bin
	$(Q)-rm -rf *.su
