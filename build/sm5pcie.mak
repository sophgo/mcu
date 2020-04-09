FIRMWARE = ../SM5PCIE
TOOL = ../tools/pcie-signal-debugger
DOC = ../docs/sm5-pcie-signal-debugger

$(TARGET): FORCE
	$(Q)rm -rf $(OUTPUT)
	$(Q)mkdir -p $(OUTPUT)
	$(Q)cd $(FIRMWARE) && make clean && make
	$(Q)cd $(TOOL) && make clean && make
	$(Q)cd $(DOC) && make clean && make
	$(Q) cp $(FIRMWARE)/build/SM5PCIE.bin $(OUTPUT)/$(TARGET)
	$(Q) cp $(TOOL)/pcie-signal-debugger $(OUTPUT)
	$(Q) cp $(DOC)/sm5-pcie-signal-debugger.pdf $(OUTPUT)

clean: FORCE
	$(Q)cd $(FIRMWARE) && make clean
	$(Q)cd $(TOOL) && make clean
	$(Q)cd $(DOC) && make clean

distclean: clean
	$(Q)rm -rf $(OUTPUT)

burn: FORCE
	st-flash write $(OUTPUT)/$(TARGET) 0x08000000
