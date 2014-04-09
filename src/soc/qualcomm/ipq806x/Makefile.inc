bootblock-y += cbfs.c

romstage-y += cbfs.c

ramstage-y += cbfs.c

ifeq ($(CONFIG_USE_BLOBS),y)

# Generate the actual coreboot bootblock code
$(objcbfs)/bootblock.raw: $(objcbfs)/bootblock.elf
	@printf "    OBJCOPY    $(subst $(obj)/,,$(@))\n"
	$(OBJCOPY_bootblock) -O binary $< $@.tmp
	@mv $@.tmp $@

# Add MBN header to allow SBL3 to start coreboot bootblock
$(objcbfs)/bootblock.mbn: $(objcbfs)/bootblock.raw
	@printf "    ADD MBN    $(subst $(obj)/,,$(@))\n"
	./util/ipqheader/ipqheader.py $(CONFIG_BOOTBLOCK_BASE) $< $@.tmp
	@mv $@.tmp $@

# Create a complete bootblock which will start up the system
$(objcbfs)/bootblock.bin: ./$(call strip_quotes,$(CONFIG_SBL_BLOB)) \
			   $(objcbfs)/bootblock.mbn
	@printf "    CAT    $(subst $(obj)/,,$(@))\n"
	@cat $^  >  $@.tmp
	@mv $@.tmp $@

endif
