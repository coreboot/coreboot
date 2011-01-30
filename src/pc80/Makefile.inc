ramstage-y += mc146818rtc.c
ramstage-y += isa-dma.c
ramstage-y += i8259.c
ramstage-$(CONFIG_UDELAY_IO) += udelay_io.c
ramstage-y += keyboard.c

romstage-$(CONFIG_USE_OPTION_TABLE) += mc146818rtc_early.c
subdirs-y += vga

$(obj)/pc80/mc146818rtc.ramstage.o : $(OPTION_TABLE_H)
$(obj)/pc80/mc146818rtc_early.romstage.o : $(OPTION_TABLE_H)

cbfs-files-$(CONFIG_HAVE_CMOS_DEFAULT) += cmos.default
cmos.default-file = $(CONFIG_CMOS_DEFAULT_FILE)
cmos.default-type = 0xaa

