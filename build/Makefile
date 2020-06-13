PROJ ?= sa5
TOP ?= ..
OUTPUT ?= $(PROJ)

ifeq ($(findstring pic16, $(PROJ)), pic16)
    TARGET ?= $(PROJ)-mcu.hex
else
    TARGET ?= $(PROJ)-mcu.bin
endif


AARCH64_CROSS_COMPILE = aarch64-linux-gnu-

ifeq ($(strip $(V)), 1)
	Q =
	E =
else
	Q = @
	E = 1>/dev/null 2>&1
endif

sinclude $(PROJ).mak

default:
	@echo 'Unknown Project $(PROJ)'

.PHONY: FORCE

FORCE:

