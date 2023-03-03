APP_DIR = $(TOP)/LF1704_BM1684_SE6V2.X
OUT_DIR = dist/default/production
HEX = $(APP_DIR)/$(OUT_DIR)/LF1704_BM1684_SE6V2.X.production.hex

$(TARGET): FORCE
	$(Q)rm -rf $(OUTPUT)
	$(Q)mkdir -p $(OUTPUT)
	$(Q)cd $(APP_DIR) && make clean && make
	$(Q)cp $(HEX) $(OUTPUT)/$(TARGET)

clean: FORCE
	$(Q)cd $(APP_DIR) && make clean

distclean: clean
	$(Q)rm -rf $(OUTPUT)

FORCE: