
ifeq ($(CONFIG_SOC_QUALCOMM_SDM845),y)

################################################################################
bootblock-y += bootblock.c
bootblock-y += spi.c
bootblock-y += mmu.c
bootblock-y += timer.c

################################################################################
verstage-y += spi.c
verstage-y += timer.c

################################################################################
romstage-y += spi.c
romstage-y += cbmem.c
romstage-y += timer.c

################################################################################
ramstage-y += soc.c
ramstage-y += spi.c
ramstage-y += cbmem.c
ramstage-y += timer.c

################################################################################

CPPFLAGS_common += -Isrc/soc/qualcomm/sdm845/include

$(objcbfs)/bootblock.bin: $(objcbfs)/bootblock.raw.bin
	@printf "Generating: $(subst $(obj)/,,$(@))\n"
	cp $(objcbfs)/bootblock.raw.bin $(objcbfs)/bootblock.bin

endif
