ramstage-y += printk.c
ramstage-y += console.c
ramstage-y += vtxprintf.c
ramstage-y += vsprintf.c
ramstage-y += post.c
ramstage-y += die.c

smm-y += printk.c
smm-y += vtxprintf.c

romstage-y += vtxprintf.c
romstage-$(CONFIG_CACHE_AS_RAM) += console.c
romstage-y += post.c
romstage-y += die.c

driver-$(CONFIG_CONSOLE_SERIAL8250) += uart8250_console.c
driver-$(CONFIG_CONSOLE_SERIAL8250MEM) += uart8250mem_console.c
driver-$(CONFIG_USBDEBUG) += usbdebug_console.c
driver-$(CONFIG_CONSOLE_LOGBUF) += logbuf_console.c
driver-$(CONFIG_CONSOLE_NE2K) += ne2k_console.c

$(obj)/console/console.ramstage.o : $(obj)/build.h
$(obj)/console/console.romstage.o : $(obj)/build.h
