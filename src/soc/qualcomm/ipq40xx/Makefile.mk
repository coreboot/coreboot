## SPDX-License-Identifier: GPL-2.0-only

ifeq ($(CONFIG_SOC_QC_IPQ40XX),y)

bootblock-y += clock.c
bootblock-y += gpio.c
bootblock-$(CONFIG_SPI_FLASH) += spi.c
bootblock-y += timer.c
bootblock-y += uart.c

verstage-y += clock.c
verstage-y += gpio.c
verstage-y += blsp.c
verstage-y += i2c.c
verstage-y += qup.c
verstage-y += spi.c
verstage-y += timer.c
verstage-y += uart.c

romstage-y += clock.c
romstage-y += blobs_init.c
romstage-y += gpio.c
romstage-$(CONFIG_SPI_FLASH) += spi.c
romstage-y += timer.c
romstage-y += uart.c
romstage-y += cbmem.c
romstage-y += i2c.c
romstage-y += blsp.c
romstage-y += qup.c

ramstage-y += blobs_init.c
ramstage-y += clock.c
ramstage-y += gpio.c
ramstage-y += lcc.c
ramstage-y += soc.c
ramstage-$(CONFIG_SPI_FLASH) += spi.c
ramstage-y += timer.c
ramstage-y += uart.c  # Want the UART always ready for the kernels' earlyprintk
ramstage-y += usb.c
ramstage-y += tz_wrapper.S

ramstage-y += blsp.c
ramstage-y += i2c.c
ramstage-y += qup.c
ramstage-y += spi.c

ifeq ($(CONFIG_USE_BLOBS),y)

$(objcbfs)/bootblock.bin: $(call strip_quotes,$(CONFIG_SBL_ELF)) \
			   $(objcbfs)/bootblock.elf
	@printf "    CRXBL      $(subst $(obj)/,,$(^)) $(subst $(obj)/,,$(@))\n"
	@$(CONFIG_SBL_UTIL_PATH)/createxbl.py -f $(CONFIG_SBL_ELF) \
		-s $(objcbfs)/bootblock.elf -o $@ -a 32 -b 32

endif

CPPFLAGS_common += -Isrc/soc/qualcomm/ipq40xx/include

# List of binary blobs coreboot needs in CBFS to be able to boot up this SOC
mbn-files := $(CONFIG_CDT_MBN) $(CONFIG_DDR_MBN) $(CONFIG_TZ_MBN)

# Location of the binary blobs
mbn-root := 3rdparty/blobs/cpu/qualcomm/ipq40xx

# Create make variables to aid cbfs-files-handler in processing the blobs (add
# them all as raw binaries at the root level).
$(foreach f,$(mbn-files),$(eval cbfs-files-y += $(f))\
	  $(eval $(f)-file := $(mbn-root)/$(f))\
	  $(eval $(f)-type := raw))

endif
