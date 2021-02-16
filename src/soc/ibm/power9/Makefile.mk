## SPDX-License-Identifier: GPL-2.0-only

ifeq ($(CONFIG_SOC_IBM_POWER9),y)

bootblock-y += bootblock.c
bootblock-y += rom_media.c

romstage-y += cbmem.c
romstage-y += rom_media.c
romstage-y += romstage.c

ramstage-y += cbmem.c
ramstage-y += chip.c
ramstage-y += rom_media.c
ramstage-y += timer.c

ifeq ($(CONFIG_SIGNING_KEYS_DIR),"")
    KEYDIR = $(top)/3rdparty/open-power-signing-utils/test/keys
else
    KEYDIR = $(CONFIG_SIGNING_KEYS_DIR)
endif

PHONY += sign_and_add_ecc
sign_and_add_ecc: $(obj)/coreboot.rom | $(ECCTOOL) $(CREATE_CONTAINER)
ifeq ($(CONFIG_SIGNING_KEYS_DIR),"")
	@printf "    NOTE: signing firmware with test keys\n"
endif
	@printf "    SBSIGN  $(subst $(obj)/,,$<)\n"
	[ -e "$(KEYDIR)/hw_key_a.key" ] || ( echo "error: $(KEYDIR)/hw_key_a.key" is missing; exit 1 )
	[ -e "$(KEYDIR)/hw_key_b.key" ] || ( echo "error: $(KEYDIR)/hw_key_b.key" is missing; exit 1 )
	[ -e "$(KEYDIR)/hw_key_c.key" ] || ( echo "error: $(KEYDIR)/hw_key_c.key" is missing; exit 1 )
	[ -e "$(KEYDIR)/sw_key_p.key" ] || ( echo "error: $(KEYDIR)/sw_key_p.key" is missing; exit 1 )
	$(CREATE_CONTAINER) -a $(KEYDIR)/hw_key_a.key -b $(KEYDIR)/hw_key_b.key -c $(KEYDIR)/hw_key_c.key \
	                    -p $(KEYDIR)/sw_key_p.key --payload $< --imagefile $<.signed
	@printf "    ECC     $(subst $(obj)/,,$<)\n"
	$(ECCTOOL) --inject $<.signed --output $<.signed.ecc --p8
ifeq ($(CONFIG_BOOTBLOCK_IN_SEEPROM),y)
	@printf "    ECC     bootblock\n"
	$(ECCTOOL) --inject $(objcbfs)/bootblock.bin --output $(obj)/bootblock.ecc --p8
else
	@printf "    SBSIGN  bootblock\n"
	$(CREATE_CONTAINER) -a $(KEYDIR)/hw_key_a.key -b $(KEYDIR)/hw_key_b.key -c $(KEYDIR)/hw_key_c.key \
	                    -p $(KEYDIR)/sw_key_p.key --payload $(objcbfs)/bootblock.bin \
	                    --imagefile $(obj)/bootblock.signed
	$(ECCTOOL) --inject $< --output $<.ecc --p8
	@printf "    ECC     bootblock\n"
	dd if=$(obj)/bootblock.signed of=$(obj)/bootblock.signed.pad ibs=25486 conv=sync 2> /dev/null
	$(ECCTOOL) --inject $(obj)/bootblock.signed.pad --output $(obj)/bootblock.signed.ecc --p8
	rm $(obj)/bootblock.signed $(obj)/bootblock.signed.pad
endif # CONFIG_BOOTBLOCK_IN_SEEPROM

files_added:: sign_and_add_ecc

endif
