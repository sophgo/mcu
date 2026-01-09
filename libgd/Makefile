TARGET ?= gd32e50x_hd
DEBUG ?= 0
SERIES ?= GD32E50x

CROSS_COMPILE := arm-none-eabi-

ifeq ($(strip $(V)), 1)
	Q =
	E =
else
	Q = @
	E = 1>/dev/null 2>&1
endif

OUTDIR = build
ifeq ($(SERIES),GD32E50x)
	SRCDIR = GD32E50x_standard_peripheral/Source	\
		 CMSIS/GD/GD32E50x/Source	\
	
	STARTUP = CMSIS/GD/GD32E50x/Source/GCC/vector.c	\
		  CMSIS/GD/GD32E50x/Source/GCC/crt.c
	
	INCDIR = GD32E50x_standard_peripheral/Include	\
		 CMSIS \
		 CMSIS/GD/GD32E50x/Include
else ifeq ($(SERIES),GD32F4xx)
	TARGET = gd32f4xx_hd
	SRCDIR = GD32F4xx_standard_peripheral/Source	\
		 CMSIS_f4xx/GD/GD32F4xx/Source	\
	
	STARTUP = CMSIS_f4xx/GD/GD32F4xx/Source/GCC/vector.c	\
		  CMSIS_f4xx/GD/GD32F4xx/Source/GCC/crt.c
	
	INCDIR = GD32F4xx_standard_peripheral/Include	\
		 CMSIS_f4xx \
		 CMSIS_f4xx/GD/GD32F4xx/Include
else
	$(error Unsupported SERIES: $(SERIES). Supported: GD32E50x, GD32F4xx)
endif

C_SRCS = $(foreach dir, $(SRCDIR), $(wildcard $(dir)/*.c))
S_SRCS = $(foreach dir, $(SRCDIR), $(wildcard $(dir)/*.s))
S_SRCS += $(foreach dir, $(SRCDIR), $(wildcard $(dir)/*.S))

SRCS = $(C_SRCS) $(S_SRCS) $(STARTUP)

OBJS = $(addprefix $(OUTDIR)/, $(notdir $(addsuffix .o, $(basename $(SRCS)))))
VPATH = $(sort $(dir $(SRCS)))

OBJS := $(sort $(OBJS))
DEPS := $(patsubst %.o, %.d, $(OBJS))

#OPT_CFLAGS = -g -O2
OPT_CFLAGS = -g -Os
ifeq ($(SERIES),GD32E50x)
	ARCH_CFLAGS = -mthumb -mcpu=cortex-m33
else ifeq ($(SERIES),GD32F4xx)
	ARCH_CFLAGS = -mthumb -mcpu=cortex-m4
endif

CPPFLAGS = $(foreach dir, $(INCDIR), -I$(dir))
CFLAGS = $(CPPFLAGS) $(OPT_CFLAGS) $(ARCH_CFLAGS) \
	  -std=gnu11 -ffunction-sections -fdata-sections -Wall -Werror \
	  $(foreach dir, $(INCDIR), -I$(dir))

ifeq ($(SERIES),GD32E50x)
	CFLAGS += -DGD32E50X -DGD32E50X_HD
else ifeq ($(SERIES),GD32F4xx)
	CFLAGS += -DGD32F4xx
endif
# use internal rc clock
RC_CLOCK ?= -D__SYSTEM_CLOCK_180M_PLL_IRC8M
CFLAGS += $(RC_CLOCK)

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
