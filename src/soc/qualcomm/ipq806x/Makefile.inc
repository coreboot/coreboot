bootblock-y += cbfs.c
bootblock-y += clock.c
bootblock-y += gpio.c

romstage-y += cbfs.c
romstage-y += clock.c
romstage-y += gpio.c

ramstage-y += cbfs.c
ramstage-y += clock.c
ramstage-y += gpio.c
ramstage-y += timer.c

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
$(objcbfs)/bootblock.bin: $(call strip_quotes,$(CONFIG_SBL_BLOB)) \
			   $(objcbfs)/bootblock.mbn
	@printf "    CAT    $(subst $(obj)/,,$(@))\n"
	@cat $^  >  $@.tmp
	@mv $@.tmp $@

endif

CPPFLAGS_common += -Isrc/soc/qualcomm/ipq806x/include
