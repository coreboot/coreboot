
ifeq ($(CONFIG_SOC_QUALCOMM_QCS405),y)

################################################################################
bootblock-y += bootblock.c
bootblock-y += timer.c
bootblock-y += spi.c
bootblock-y += mmu.c
bootblock-y += gpio.c
bootblock-y += clock.c

################################################################################
verstage-y += timer.c
verstage-y += spi.c
verstage-y += gpio.c
verstage-y += clock.c

################################################################################
romstage-y += timer.c
romstage-y += spi.c
romstage-y += cbmem.c
romstage-y += gpio.c
romstage-y += clock.c
romstage-y += usb.c

################################################################################
ramstage-y += soc.c
ramstage-y += timer.c
ramstage-y += spi.c
ramstage-y += cbmem.c
ramstage-y += gpio.c
ramstage-y += clock.c
ramstage-y += usb.c

################################################################################

CPPFLAGS_common += -Isrc/soc/qualcomm/qcs405/include

$(objcbfs)/bootblock.bin: $(objcbfs)/bootblock.raw.bin
	@printf "Generating: $(subst $(obj)/,,$(@))\n"
	cp $(objcbfs)/bootblock.raw.bin $(objcbfs)/bootblock.bin

endif
