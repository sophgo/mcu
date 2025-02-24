TARGET = BM1684X_M_2
DEBUG = 1
STANDALONE = 0
CLOSEPMIC = 0

ifeq ($(strip $(V)), 1)
	Q =
	E =
else
	Q = @
	E = 1>/dev/null 2>&1
endif

OPENCM3_DIR = ../libopencm3

OUTDIR = build
SRCDIR = . i2c-slaves
INCDIR = . $(OPENCM3_DIR)/include i2c-slaves

C_SRCS = $(foreach dir, $(SRCDIR), $(wildcard $(dir)/*.c))
S_SRCS = $(foreach dir, $(SRCDIR), $(wildcard $(dir)/*.s))
S_SRCS += $(foreach dir, $(SRCDIR), $(wildcard $(dir)/*.S))

SRCS = $(C_SRCS) $(S_SRCS)
OBJS = $(addprefix $(OUTDIR)/, $(notdir $(addsuffix .o, $(basename $(SRCS)))))
VPATH = $(sort $(dir $(SRCS)))

OBJS := $(sort $(OBJS))
DEPS := $(patsubst %.o, %.d, $(OBJS))

LDSCRIPT_APP = $(TARGET)_APP.ld
LDSCRIPT_LOADER = $(TARGET)_LOADER.ld

OPT_CFLAGS = -Os
ARCH_CFLAGS = -mthumb -mcpu=cortex-m0plus
SPEC_CFLAGS = -specs=nano.specs

CPPFLAGS = $(foreach dir, $(INCDIR), -I$(dir))
CFLAGS = $(CPPFLAGS) $(OPT_CFLAGS) $(ARCH_CFLAGS) $(SPEC_CFLAGS) \
	  -std=gnu11 -ffunction-sections -fdata-sections -Wall -Werror \
	  -DSTM32L0

ifeq ($(strip $(DEBUG)), 1)
CFLAGS += -DDEBUG
endif

ifeq ($(strip $(STANDALONE)), 1)
CFLAGS += -DSTANDALONE
endif

ifeq ($(strip $(CLOSEPMIC)), 1)
CFLAGS += -DCLOSEPMIC
endif

ASFLAGS = $(CFLAGS) -x assembler-with-cpp -Wa,--no-warn

LDFLAGS = $(SPEC_CFLAGS) $(ARCH_CFLAGS) $(OPT_CFLAGS) -static \
	   -nostartfiles -Wl,--cref -Wl,--gc-sections \
	   -L$(OPENCM3_DIR)/lib -lopencm3_stm32l0

CROSS_COMPILE = arm-none-eabi-
CC = $(CROSS_COMPILE)gcc
AS = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)gcc
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdump
READELF = $(CROSS_COMPILE)readelf

all:$(OUTDIR) \
	$(OUTDIR)/$(TARGET)_APP.elf \
	$(OUTDIR)/$(TARGET)_APP.bin \
	$(OUTDIR)/$(TARGET)_APP.dis \
	$(OUTDIR)/$(TARGET)_LOADER.elf \
	$(OUTDIR)/$(TARGET)_LOADER.bin \
	$(OUTDIR)/$(TARGET)_LOADER.dis

$(OUTDIR)/%.bin: $(OUTDIR)/%.elf
	@echo '[CP] $@'
	$(Q)$(OBJCOPY) -O binary $< $@

$(OUTDIR)/%.dis: $(OUTDIR)/%.elf
	@echo '[DP] $@'
	$(Q)$(READELF) -e $< > $@
	$(Q)$(OBJDUMP) -D $< >> $@

$(OUTDIR)/%.elf: $(OUTDIR)/%.ld $(OBJS)
	@echo '[LD] $@'
	$(Q)$(LD) $(OBJS) $(LDFLAGS) -Wl,-Map,$@.map \
            -T $< -o $@

$(OUTDIR)/$(LDSCRIPT_APP): target.ldi
	@echo '[XP] $@'
	$(Q)$(CC) $(CPPFLAGS) \
	-D'ROM_START=0x08008000' -D'ROM_SIZE=32K' \
	-D'RAM_START=0x20000000' -D'RAM_SIZE=8K' \
	-E -xc -P -C $< > $@

$(OUTDIR)/$(LDSCRIPT_LOADER): target.ldi
	@echo '[XP] $@'
	$(Q)$(CC) $(CPPFLAGS) \
	-D'ROM_START=0x08000000' -D'ROM_SIZE=32K' \
	-D'RAM_START=0x20000000' -D'RAM_SIZE=8K' \
	-E -xc -P -C $< > $@

$(OUTDIR)/%.o: %.c | $(OUTDIR)
	@echo '[CC] $@'
	$(Q)$(CC) $(CFLAGS) -c $< -o $@

$(OUTDIR)/%.d: %.c | $(OUTDIR)
	@echo '[CD] $@'
	$(Q)$(CC) $(CFLAGS) -MM -MF $@ $<

$(OUTDIR):
	@mkdir -p $@

burn: $(OUTDIR)/$(TARGET)_LOADER.bin
	st-flash write $< 0x08000000

clean:
	rm -rf $(OUTDIR)

code: gencode.py config.xlsx
	@echo '[GEN]'
	$(Q)./gencode.py config.xlsx -s pin.c -e pin.h -y pin
	$(Q)./gencode.py config.xlsx -s board_power.c -e board_power.h -p board_power_porting.c -y power

cclean:
	rm -rf pin.c pin.h board_power.c board_power.h

.PHONY: clean burn

include $(wildcard $(DEPS))
