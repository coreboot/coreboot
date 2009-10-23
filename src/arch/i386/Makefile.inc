#######################################################################
# Take care of subdirectories
subdirs-y += boot
subdirs-y += init
subdirs-y += lib
subdirs-y += smp

obj-$(CONFIG_HAVE_OPTION_TABLE) += ../../option_table.o

ifdef POST_EVALUATION
BOOTBLOCK_SIZE=65536

#######################################################################
# Build the final rom image

$(obj)/coreboot.rom: $(obj)/coreboot.bootblock $(obj)/coreboot_ram $(CBFSTOOL)
	rm -f $@
	$(CBFSTOOL) $@ create $(shell expr 1024 \* $(CONFIG_COREBOOT_ROMSIZE_KB)) $(BOOTBLOCK_SIZE) $(obj)/coreboot.bootblock
	if [ -f fallback/coreboot_apc ]; \
	then \
		$(CBFSTOOL) $@ add-stage fallback/coreboot_apc fallback/coreboot_apc $(CBFS_COMPRESS_FLAG); \
	fi
	$(CBFSTOOL) $@ add-stage  $(obj)/coreboot_ram fallback/coreboot_ram $(CBFS_COMPRESS_FLAG)
ifeq ($(CONFIG_PAYLOAD_NONE),y)
	@printf "    PAYLOAD    none (as specified by user)\n"
else
	@printf "    PAYLOAD    $(CONFIG_FALLBACK_PAYLOAD_FILE) $(COMPRESSFLAG)\n"
	$(CBFSTOOL) ./build/coreboot.rom add-payload $(CONFIG_FALLBACK_PAYLOAD_FILE)  fallback/payload $(CBFS_PAYLOAD_COMPRESS_FLAG)
endif
ifeq ($(CONFIG_VGA_BIOS),y)
	@printf "    VGABIOS    $(CONFIG_FALLBACK_VGA_BIOS_FILE) $(CONFIG_FALLBACK_VGA_BIOS_ID)\n"
	$(CBFSTOOL) ./build/coreboot.rom add $(CONFIG_FALLBACK_VGA_BIOS_FILE) "pci$(CONFIG_FALLBACK_VGA_BIOS_ID).rom" optionrom 
endif
	@printf "    CBFSPRINT  ./build/coreboot.rom\n\n"
	$(CBFSTOOL) build/coreboot.rom print


#######################################################################
# Build the bootblock

$(obj)/coreboot.bootblock: $(obj)/coreboot
	@printf "    OBJCOPY    $(subst $(obj)/,,$(@))\n"
	$(OBJCOPY) -O binary $< $@

$(obj)/ldscript.ld: $(ldscripts) $(obj)/ldoptions
	printf 'INCLUDE "ldoptions"\n' > $@
	printf '$(foreach ldscript,$(ldscripts),INCLUDE "$(ldscript)"\n)' >> $@

$(obj)/crt0_includes.h: $(crt0s)
	printf '$(foreach crt0,$(obj)/config.h $(crt0s),#include "$(crt0)"\n)' > $@

$(obj)/mainboard/$(MAINBOARDDIR)/crt0.o: $(obj)/mainboard/$(MAINBOARDDIR)/crt0.s
	$(CC) -I$(obj) -Wa,-acdlns -c -o $@ $<  > $(dir $@)/crt0.disasm

$(obj)/mainboard/$(MAINBOARDDIR)/crt0.s: $(src)/arch/i386/init/crt0.S.lb $(obj)/crt0_includes.h
	$(CC) -x assembler-with-cpp -DASSEMBLY -E -I$(src)/include -I$(src)/arch/i386/include -I$(obj) -include $(obj)/config.h -I. -I$(src) $< > $@.new && mv $@.new $@

$(obj)/coreboot: $(initobjs) $(obj)/ldscript.ld
	@printf "    LINK       $(subst $(obj)/,,$(@))\n"
	$(CC) -nostdlib -nostartfiles -static -o $@ -L$(obj) -T $(obj)/ldscript.ld $(initobjs)
	$(NM) -n $(obj)/coreboot | sort > $(obj)/coreboot.map

#######################################################################
# i386 specific tools

$(obj)/option_table.h $(obj)/option_table.c: $(obj)/build_opt_tbl $(top)/src/mainboard/$(MAINBOARDDIR)/cmos.layout
	@printf "    OPTION     $(subst $(obj)/,,$(@))\n"
	$(obj)/build_opt_tbl --config $(top)/src/mainboard/$(MAINBOARDDIR)/cmos.layout --header $(obj)/option_table.h --option $(obj)/option_table.c

$(obj)/build_opt_tbl: $(top)/util/options/build_opt_tbl.c $(top)/src/include/pc80/mc146818rtc.h $(top)/src/include/boot/coreboot_tables.h $(obj)/config.h
	@printf "    HOSTCC     $(subst $(obj)/,,$(@))\n"
	$(HOSTCC) $(HOSTCFLAGS) -include $(obj)/config.h $< -o $@

#######################################################################
# Build the coreboot_ram (stage 2)

$(obj)/coreboot_ram: $(obj)/coreboot_ram.o $(src)/config/coreboot_ram.ld #ldoptions
	@printf "    CC         $(subst $(obj)/,,$(@))\n"
	$(CC) -nostdlib -nostartfiles -static -o $@ -L$(obj) -T $(src)/config/coreboot_ram.ld $(obj)/coreboot_ram.o
	$(NM) -n $(obj)/coreboot_ram | sort > $(obj)/coreboot_ram.map

$(obj)/coreboot_ram.o: $(obj)/arch/i386/lib/c_start.o $(drivers) $(obj)/coreboot.a $(LIBGCC_FILE_NAME)
	@printf "    CC         $(subst $(obj)/,,$(@))\n"
	$(CC) -nostdlib -r -o $@ $(obj)/arch/i386/lib/c_start.o $(drivers) -Wl,-\( $(obj)/coreboot.a $(LIBGCC_FILE_NAME) -Wl,-\)

$(obj)/coreboot.a: $(objs)
	@printf "    AR         $(subst $(obj)/,,$(@))\n"
	rm -f $(obj)/coreboot.a
	$(AR) cr $(obj)/coreboot.a $(objs)

#######################################################################
# done

endif
