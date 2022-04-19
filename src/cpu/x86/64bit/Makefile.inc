bootblock-y += mode_switch.S
ifneq ($(CONFIG_VBOOT_STARTS_BEFORE_BOOTBLOCK),y)
verstage-y += mode_switch.S
endif
romstage-y += mode_switch.S
postcar-y += mode_switch.S
ramstage-y += mode_switch.S

# Add --defsym=_start=0 to suppress a linker warning.
$(objcbfs)/pt: $(dir)/pt.S
	$(CC_bootblock) $(CFLAGS_bootblock) $(CPPFLAGS_bootblock) -o $@.tmp $< -Wl,--section-start=.rodata=$(CONFIG_ARCH_X86_64_PGTBL_LOC),--defsym=_start=0
	$(OBJCOPY_ramstage) -Obinary -j .rodata $@.tmp $@
	rm $@.tmp

cbfs-files-y += pagetables
pagetables-file := $(objcbfs)/pt
pagetables-type := raw
pagetables-compression := none
pagetables-COREBOOT-position := $(CONFIG_ARCH_X86_64_PGTBL_LOC)
