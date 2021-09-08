TARGET = gd32e50x_hd
DEBUG ?= 0

CROSS_COMPILE := arm-none-eabi-

ifeq ($(strip $(V)), 1)
	Q =
	E =
else
	Q = @
	E = 1>/dev/null 2>&1
endif

OUTDIR = build
SRCDIR = GD32E50x_standard_peripheral/Source	\
	 CMSIS/GD/GD32E50x/Source	\

STARTUP = CMSIS/GD/GD32E50x/Source/GCC/vector.c	\
	  CMSIS/GD/GD32E50x/Source/GCC/crt.c

INCDIR = GD32E50x_standard_peripheral/Include	\
	 CMSIS \
	 CMSIS/GD/GD32E50x/Include

C_SRCS = $(foreach dir, $(SRCDIR), $(wildcard $(dir)/*.c))
S_SRCS = $(foreach dir, $(SRCDIR), $(wildcard $(dir)/*.s))
S_SRCS += $(foreach dir, $(SRCDIR), $(wildcard $(dir)/*.S))

SRCS = $(C_SRCS) $(S_SRCS) $(STARTUP)

OBJS = $(addprefix $(OUTDIR)/, $(notdir $(addsuffix .o, $(basename $(SRCS)))))
VPATH = $(sort $(dir $(SRCS)))

OBJS := $(sort $(OBJS))
DEPS := $(patsubst %.o, %.d, $(OBJS))

OPT_CFLAGS = -g -O2
ARCH_CFLAGS = -mthumb -mcpu=cortex-m33

CPPFLAGS = $(foreach dir, $(INCDIR), -I$(dir))
CFLAGS = $(CPPFLAGS) $(OPT_CFLAGS) $(ARCH_CFLAGS) \
	  -std=gnu11 -ffunction-sections -fdata-sections -Wall -Werror \
	  $(foreach dir, $(INCDIR), -I$(dir))

CFLAGS += -DGD32E50X -DGD32E50X_HD
# use internal rc clock
CFLAGS += -D__SYSTEM_CLOCK_180M_PLL_IRC8M

ifeq ($(strip $(DEBUG)), 1)
CFLAGS += -DDEBUG
endif

ASFLAGS = $(CFLAGS) -x assembler-with-cpp -Wa,--no-warn

CC = $(CROSS_COMPILE)gcc
AS = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)gcc
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdump
READELF = $(CROSS_COMPILE)readelf

all: $(OUTDIR) $(OUTDIR)/lib$(TARGET).a

$(OUTDIR)/lib$(TARGET).a: $(OBJS)
	@echo '[AR] $@'
	$(Q)$(AR) crs $@ $(OBJS)

$(OUTDIR)/%.o: %.c | $(OUTDIR)
	@echo '[CC] $@'
	$(Q)$(CC) $(CFLAGS) -c $< -o $@

$(OUTDIR)/%.o: %.S | $(OUTDIR)
	@echo '[AS] $@'
	$(Q)$(CC) $(ASFLAGS) -c $< -o $@

$(OUTDIR)/%.o: %.s | $(OUTDIR)
	@echo '[AS] $@'
	$(Q)$(CC) $(ASFLAGS) -c $< -o $@

$(OUTDIR):
	@mkdir -p $@

clean:
	rm -rf $(OUTDIR)

.PHONY: clean

include $(wildcard $(DEPS))
