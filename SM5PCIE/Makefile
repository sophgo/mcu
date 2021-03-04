TARGET = SM5PCIE
DEBUG = 0
# BOARD = NUCLEO

ifeq ($(strip $(V)), 1)
	Q =
	E =
else
	Q = @
	E = 1>/dev/null 2>&1
endif

OPENCM3_DIR = ../libopencm3

OUTDIR = build
SRCDIR = . ulibSD
INCDIR = . $(OPENCM3_DIR)/include ulibSD

#fatfs
INCDIR += ecdc
SRCDIR += ecdc

C_SRCS = $(foreach dir, $(SRCDIR), $(wildcard $(dir)/*.c))
S_SRCS = $(foreach dir, $(SRCDIR), $(wildcard $(dir)/*.s))
S_SRCS += $(foreach dir, $(SRCDIR), $(wildcard $(dir)/*.S))

SRCS = $(C_SRCS) $(S_SRCS)
OBJS = $(addprefix $(OUTDIR)/, $(notdir $(addsuffix .o, $(basename $(SRCS)))))
VPATH = $(sort $(dir $(SRCS)))

OBJS := $(sort $(OBJS))
DEPS := $(patsubst %.o, %.d, $(OBJS))

LDSCRIPT = $(TARGET).ld

OPT_CFLAGS = -g -O2
ARCH_CFLAGS = -mthumb -mcpu=cortex-m0plus
SPEC_CFLAGS = -specs=nano.specs

CPPFLAGS = $(foreach dir, $(INCDIR), -I$(dir))
CFLAGS = $(CPPFLAGS) $(OPT_CFLAGS) $(ARCH_CFLAGS) $(SPEC_CFLAGS) \
	  -std=gnu11 -ffunction-sections -fdata-sections -Wall -Werror \
	  -DSTM32L0 $(foreach dir, $(INCDIR), -I$(dir))

ifeq ($(strip $(DEBUG)), 1)
CFLAGS += -DDEBUG
endif

ifeq ($(strip $(BOARD)), NUCLEO)
CFLAGS += -DNUCLEO
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
	$(OUTDIR)/$(TARGET).elf \
	$(OUTDIR)/$(TARGET).bin \
	$(OUTDIR)/$(TARGET).dis \
	display_version

$(OUTDIR)/%.bin: $(OUTDIR)/%.elf
	@echo '[CP] $@'
	$(Q)$(OBJCOPY) -O binary $< $@
	$(Q)chmod a-x $@

$(OUTDIR)/%.dis: $(OUTDIR)/%.elf
	@echo '[DP] $@'
	$(Q)$(READELF) -e $< > $@
	$(Q)$(OBJDUMP) -D $< >> $@

$(OUTDIR)/%.elf: $(OUTDIR)/%.ld $(OBJS)
	@echo '[LD] $@'
	$(Q)$(LD) $(OBJS) $(LDFLAGS) -Wl,-Map,$(basename $@).map \
            -T $< -o $@

$(OUTDIR)/$(LDSCRIPT): target.ldi
	@echo '[XP] $@'
	$(Q)$(CC) $(CPPFLAGS) \
	-D'ROM_START=0x08000000' -D'ROM_SIZE=64K' \
	-D'RAM_START=0x20000000' -D'RAM_SIZE=20K' \
	-E -xc -P -C $< > $@

$(OUTDIR)/%.o: %.c | $(OUTDIR)
	@echo '[CC] $@'
	$(Q)$(CC) $(CFLAGS) -c $< -o $@

$(OUTDIR)/%.d: %.c | $(OUTDIR)
	@echo '[CD] $@'
	$(Q)$(CC) $(CFLAGS) -MM -MF $@ $<

$(OUTDIR):
	@mkdir -p $@

burn: $(OUTDIR)/$(TARGET).bin
	st-flash write $< 0x08000000

clean:
	rm -rf $(OUTDIR)

.PHONY: clean burn display_version
.SECONDARY: $(OBJS)

include $(wildcard $(DEPS))
