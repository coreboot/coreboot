obj-y += printk.o
obj-y += console.o
obj-y += vtxprintf.o
obj-y += vsprintf.o

smmobj-y += printk.o
smmobj-y += vtxprintf.o

initobj-y += vtxprintf.o
initobj-$(CONFIG_CACHE_AS_RAM) += console.o

driver-$(CONFIG_CONSOLE_SERIAL8250) += uart8250_console.o
driver-$(CONFIG_USBDEBUG) += usbdebug_console.o
driver-$(CONFIG_CONSOLE_VGA) += vga_console.o
driver-$(CONFIG_CONSOLE_BTEXT) += btext_console.o
driver-$(CONFIG_CONSOLE_BTEXT) += font-8x16.o
driver-$(CONFIG_CONSOLE_LOGBUF) += logbuf_console.o
driver-$(CONFIG_CONSOLE_NE2K) += ne2k_console.o

$(obj)/console/console.o : $(obj)/build.h
$(obj)/console/console.initobj.o : $(obj)/build.h
