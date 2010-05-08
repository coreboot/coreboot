obj-y += mc146818rtc.o
obj-y += isa-dma.o
obj-y += i8259.o
obj-$(CONFIG_UDELAY_IO) += udelay_io.o
obj-y += keyboard.o

initobj-$(CONFIG_USE_DCACHE_RAM) += serial.o
subdirs-y += vga

$(obj)/pc80/mc146818rtc.o : $(OPTION_TABLE_H)
