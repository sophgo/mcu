PROJ ?= sa5
TOP ?= ..
OUTPUT ?= $(PROJ)
TARGET ?= $(PROJ)-mcu.bin

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

