## SPDX-License-Identifier: GPL-2.0-only

all_x86-y += mode_switch.S
all_x86-y += mode_switch2.S

ifeq ($(CONFIG_NEED_SMALL_2MB_PAGE_TABLES),y)
PAGETABLE_SRC := pt.S
else
PAGETABLE_SRC := pt1G.S
endif

all_x86-y += $(PAGETABLE_SRC)

# Add --defsym=_start=0 to suppress a linker warning.
$(objcbfs)/pt: $(dir)/$(PAGETABLE_SRC) $(obj)/config.h
	$(CC_bootblock) $(CFLAGS_bootblock) $(CPPFLAGS_bootblock) -o $@.tmp $< -Wl,--section-start=.rodata=$(CONFIG_ARCH_X86_64_PGTBL_LOC),--defsym=_start=0
	$(OBJCOPY_ramstage) -Obinary -j .rodata $@.tmp $@
	rm $@.tmp

cbfs-files-$(CONFIG_PAGE_TABLES_IN_CBFS) += pagetables
pagetables-file := $(objcbfs)/pt
pagetables-type := raw
pagetables-compression := none
pagetables-COREBOOT-position := $(CONFIG_ARCH_X86_64_PGTBL_LOC)
