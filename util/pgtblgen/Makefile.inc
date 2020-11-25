ifeq ($(CONFIG_ARCH_BOOTBLOCK_X86_64),y)

PGTBLGEN:= $(obj)/pgtblgen

cbfs-files-y += pagetables
pagetables-file := $(obj)/mainboard/$(MAINBOARDDIR)/pagetables
pagetables-type := raw
pagetables-compression := none
pagetables-COREBOOT-position := $(CONFIG_ARCH_X86_64_PGTBL_LOC)

$(obj)/mainboard/$(MAINBOARDDIR)/pagetables: $(PGTBLGEN) $(obj)/config.h
	printf "    TOOL       Creating page tables\n"
	$(PGTBLGEN) -b $(CONFIG_ARCH_X86_64_PGTBL_LOC) -a x86_64 -o $@

$(PGTBLGEN): util/pgtblgen/pgtblgen.c
	printf "    MAKE       Creating PGTBLGEN tool\n"
	$(HOSTCC) -std=c99 $< -I$(obj) -o $@

endif
