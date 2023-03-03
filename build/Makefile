PROJ ?= sm5g
TOP ?= ..
OUTPUT ?= $(PROJ)

ifeq ($(findstring pic16, $(PROJ)), pic16)
    TARGET ?= $(PROJ)-mcu.hex
else
    TARGET ?= $(PROJ)-mcu.bin
endif


AARCH64_CROSS_COMPILE = aarch64-linux-gnu-
RISCV64_CROSS_COMPILE = riscv64-unknown-linux-gnu-

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

