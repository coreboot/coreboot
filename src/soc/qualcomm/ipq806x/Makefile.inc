bootblock-y += cbfs.c
bootblock-y += timer.c

romstage-y += cbfs.c
romstage-y += timer.c

ramstage-y += cbfs.c
ramstage-y += timer.c

ifeq ($(CONFIG_MBN_ENCAPSULATION),y)

$(objcbfs)/%.bin: $(objcbfs)/%.elf
	@printf "    OBJCOPY    $(subst $(obj)/,,$(@))\n"
	$(OBJCOPY_bootblock) -O binary $< $@.prembn
	@printf "    ADD MBN    $(subst $(obj)/,,$(@))\n"
	./util/ipqheader/ipqheader.py $(CONFIG_BOOTBLOCK_BASE) $@.prembn  $@.tmp
	@mv $@.tmp $@
endif
