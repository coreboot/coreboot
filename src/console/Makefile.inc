ramstage-y += vtxprintf.c printk.c vsprintf.c
ramstage-y += init.c console.c
ramstage-y += post.c
ramstage-y += die.c

smm-$(CONFIG_DEBUG_SMI) += vtxprintf.c printk.c
smm-$(CONFIG_SMM_TSEG) += die.c

romstage-$(CONFIG_EARLY_CONSOLE) += vtxprintf.c printk.c
romstage-$(CONFIG_EARLY_CONSOLE) += init.c console.c
romstage-y += post.c
romstage-y += die.c

bootblock-$(CONFIG_BOOTBLOCK_CONSOLE) += vtxprintf.c printk.c
bootblock-$(CONFIG_BOOTBLOCK_CONSOLE) += init.c console.c
bootblock-y += die.c

ramstage-$(CONFIG_CONSOLE_SERIAL) += uart_console.c
ramstage-$(CONFIG_SPKMODEM) += spkmodem_console.c
ramstage-$(CONFIG_CONSOLE_USB) += usbdebug_console.c
ramstage-$(CONFIG_CONSOLE_NE2K) += ne2k_console.c
ramstage-$(CONFIG_CONSOLE_CBMEM) += cbmem_console.c
ramstage-$(CONFIG_CONSOLE_QEMU_DEBUGCON) += qemu_debugcon_console.c


$(obj)/console/init.ramstage.o : $(obj)/build.h
$(obj)/console/init.romstage.o : $(obj)/build.h
$(obj)/console/init.bootblock.o : $(obj)/build.h
