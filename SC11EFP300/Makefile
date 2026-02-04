TOP = ..
TARGET = SC11EFP300
DEBUG = 0
STANDALONE = 0

V ?= 0

ifeq ($(strip $(V)), 1)
	Q =
	E =
else
	Q = @
	E = 1>/dev/null 2>&1
endif

OUTDIR = build
SRCDIR = . ecdc dvfs
INCDIR = . ecdc dvfs \
	$(TOP)/libgd/GD32F4xx_standard_peripheral/Include	\
	$(TOP)/libgd/CMSIS_f4xx \
	$(TOP)/libgd/CMSIS_f4xx/GD/GD32F4xx/Include

C_SRCS = $(foreach dir, $(SRCDIR), $(wildcard $(dir)/*.c))
S_SRCS = $(foreach dir, $(SRCDIR), $(wildcard $(dir)/*.s))
S_SRCS += $(foreach dir, $(SRCDIR), $(wildcard $(dir)/*.S))

SRCS = $(C_SRCS) $(S_SRCS)
OBJS = $(addprefix $(OUTDIR)/, $(notdir $(addsuffix .o, $(basename $(SRCS)))))
VPATH = $(sort $(dir $(SRCS)))

OBJS := $(sort $(OBJS))
DEPS := $(patsubst %.o, %.d, $(OBJS))

LDSCRIPT = target.ld

#OPT_CFLAGS = -Os -g
OPT_CFLAGS = -Os
ARCH_CFLAGS = -mthumb -mcpu=cortex-m4
SPEC_CFLAGS = -specs=nano.specs

CPPFLAGS = $(foreach dir, $(INCDIR), -I$(dir))
CFLAGS = $(CPPFLAGS) $(OPT_CFLAGS) $(ARCH_CFLAGS) $(SPEC_CFLAGS) \
	  -std=gnu11 -ffunction-sections -fdata-sections -Wall -Werror \
	  $(foreach dir, $(INCDIR), -I$(dir))

CFLAGS += -DGD32F4xx

ifeq ($(strip $(DEBUG)), 1)
CFLAGS += -DDEBUG
endif

ifeq ($(strip $(STANDALONE)), 1)
CFLAGS += -DSTANDALONE
endif

ASFLAGS = $(CFLAGS) -x assembler-with-cpp -Wa,--no-warn

LDFLAGS = $(SPEC_CFLAGS) $(ARCH_CFLAGS) $(OPT_CFLAGS) -static \
	   -nostartfiles -Wl,--cref -Wl,--gc-sections \
	   -L$(TOP)/libgd/build -lgd32f4xx_hd

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
	$(OUTDIR)/$(TARGET).dis

$(OUTDIR)/$(TARGET).bin: $(OUTDIR)/$(TARGET).elf
	@echo '[CP] $@'
	$(Q)$(OBJCOPY) -O binary $< $@

$(OUTDIR)/$(TARGET).dis: $(OUTDIR)/$(TARGET).elf
	@echo '[DP] $@'
	$(Q)$(READELF) -e $< > $@
	$(Q)$(OBJDUMP) -D $< >> $@

$(OUTDIR)/$(TARGET).elf: $(LDSCRIPT) $(OBJS)
	@echo '[LD] $@'
	$(Q)$(LD) $(OBJS) $(LDFLAGS) -Wl,-Map,$(OUTDIR)/$(TARGET).map \
            -T $< -o $@

$(OUTDIR)/%.o: %.c | $(OUTDIR)
	@echo '[CC] $@'
	$(Q)$(CC) $(CFLAGS) -c $< -o $@

$(OUTDIR)/%.o: %.S | $(OUTDIR)
	@echo '[AS] $@'
	$(Q)$(CC) $(ASFLAGS) -c $< -o $@

$(OUTDIR):
	@mkdir -p $@

burn: $(OUTDIR)/$(TARGET).bin
	gd-flash write $< 0x08000000

clean:
	rm -rf $(OUTDIR)

code: gencode.py config.xlsx
	@echo '[GEN]'
	$(Q)./gencode.py config.xlsx -e pin.h -y pin
	$(Q)./gencode.py config.xlsx -s board_power.c -e board_power.h -p board_power_porting.c -y power

cclean:
	rm -rf pin.c pin.h board_power.c board_power.h

.PHONY: clean burn

include $(wildcard $(DEPS))
