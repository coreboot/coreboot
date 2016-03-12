ifeq ($(CONFIG_ARCH_X86),y)

ramstage-y += isa-dma.c
ramstage-y += i8254.c
ramstage-y += i8259.c
ramstage-$(CONFIG_UDELAY_IO) += udelay_io.c
romstage-$(CONFIG_UDELAY_IO) += udelay_io.c
ramstage-y += keyboard.c
ramstage-$(CONFIG_SPKMODEM) += spkmodem.c
romstage-$(CONFIG_SPKMODEM) += spkmodem.c

endif
