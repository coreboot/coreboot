obj-y += mc146818rtc.o
obj-y += isa-dma.o
obj-y += i8259.o 
#obj-y += udelay_timer2.o CONFIG_UDELAY_TIMER2
obj-$(CONFIG_UDELAY_IO) += udelay_io.o
obj-y += keyboard.o

#initobj-y += serial.o
subdirs-y += vga
