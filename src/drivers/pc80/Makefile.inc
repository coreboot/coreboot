ramstage-y += mc146818rtc.c
ramstage-y += isa-dma.c
ramstage-y += i8254.c
ramstage-y += i8259.c
ramstage-$(CONFIG_UDELAY_IO) += udelay_io.c
ramstage-y += keyboard.c
romstage-$(CONFIG_TPM) += tpm.c

romstage-$(CONFIG_USE_OPTION_TABLE) += mc146818rtc_early.c
subdirs-y += vga

cbfs-files-$(CONFIG_HAVE_CMOS_DEFAULT) += cmos.default
cmos.default-file = $(CONFIG_CMOS_DEFAULT_FILE):nvramtool
cmos.default-type = 0xaa

smm-y += mc146818rtc.c

$(obj)/drivers/pc80/mc146818rtc.ramstage.o : $(obj)/build.h
$(obj)/drivers/pc80/mc146818rtc.smm.o : $(obj)/build.h
