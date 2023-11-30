APP_ROOT = $(TOP)/SM7M_MP_1_2
TOOLS = $(TOP)/tools
PACK_ROOT = $(TOOLS)/pack
PACK = $(PACK_ROOT)/pack
UTILROOT = $(TOOLS)/util
UPGRADE = $(UTILROOT)/mcu-util

$(TARGET): FORCE
	$(Q)rm -rf $(OUTPUT)
	$(Q)mkdir -p $(OUTPUT)
	$(Q)cd $(APP_ROOT) && make clean && make
	$(Q)cd $(PACK_ROOT) && make distclean && make
	$(Q)cd $(UTILROOT) && make clean && make && cd - && cp $(UPGRADE) $(OUTPUT)/mcu-util-amd64
	$(Q)cd $(UTILROOT) && make clean && make CROSS_COMPILE=$(AARCH64_CROSS_COMPILE) && cd - && cp $(UPGRADE) $(OUTPUT)/mcu-util-aarch64
	$(Q)$(PACK) sm7m_mp_1_2-layout.xml $@ $(E) && mv $@ $(OUTPUT)/

clean: FORCE
	$(Q)cd $(APP_ROOT) && make clean
	$(Q)cd $(PACK_ROOT) && make distclean
	$(Q)cd $(UTILROOT) && make clean
	$(Q)cd $(UTILROOT) && make clean

distclean: clean
	$(Q)rm -rf $(OUTPUT)

burn: FORCE
	gd-flash write $(OUTPUT)/$(TARGET) 0x08000000
