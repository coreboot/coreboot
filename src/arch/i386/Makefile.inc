#######################################################################
# Take care of subdirectories
subdirs-y += boot
subdirs-y += init
subdirs-y += lib
subdirs-y += smp

obj-$(CONFIG_HAVE_OPTION_TABLE) += ../../option_table.o

ifdef POST_EVALUATION

#######################################################################
# Build the final rom image
$(obj)/coreboot.rom: $(obj)/coreboot.pre $(obj)/coreboot_ram $(CBFSTOOL)
	cp $(obj)/coreboot.pre $@
	if [ -f fallback/coreboot_apc ]; \
	then \
		$(CBFSTOOL) $@ add-stage fallback/coreboot_apc fallback/coreboot_apc $(CBFS_COMPRESS_FLAG); \
	fi
	$(CBFSTOOL) $@ add-stage $(obj)/coreboot_ram fallback/coreboot_ram $(CBFS_COMPRESS_FLAG)
ifeq ($(CONFIG_PAYLOAD_NONE),y)
	@printf "    PAYLOAD    none (as specified by user)\n"
else
	@printf "    PAYLOAD    $(CONFIG_FALLBACK_PAYLOAD_FILE) $(CBFS_PAYLOAD_COMPRESS_FLAG)\n"
	$(CBFSTOOL) $(obj)/coreboot.rom add-payload $(CONFIG_FALLBACK_PAYLOAD_FILE) fallback/payload $(CBFS_PAYLOAD_COMPRESS_FLAG)
endif
ifeq ($(CONFIG_VGA_BIOS),y)
	@printf "    VGABIOS    $(CONFIG_FALLBACK_VGA_BIOS_FILE) $(CONFIG_FALLBACK_VGA_BIOS_ID)\n"
	$(CBFSTOOL) $(obj)/coreboot.rom add $(CONFIG_FALLBACK_VGA_BIOS_FILE) "pci$(CONFIG_FALLBACK_VGA_BIOS_ID).rom" optionrom
endif
	@printf "    CBFSPRINT  $(subst $(obj)/,,$(@))\n\n"
	$(CBFSTOOL) $(obj)/coreboot.rom print

#######################################################################
# i386 specific tools

$(obj)/option_table.h $(obj)/option_table.c $(obj)/arch/i386/../../option_table.c: $(obj)/build_opt_tbl $(top)/src/mainboard/$(MAINBOARDDIR)/cmos.layout
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

ifeq ($(CONFIG_TINY_BOOTBLOCK),y)
include $(src)/arch/i386/Makefile.tinybootblock.inc
else
include $(src)/arch/i386/Makefile.bigbootblock.inc
endif
