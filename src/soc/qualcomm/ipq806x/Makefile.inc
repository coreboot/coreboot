##
## This file is part of the coreboot project.
##
## Copyright 2014 Google Inc.
##
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; version 2 of the License.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with this program; if not, write to the Free Software
## Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
##

bootblock-y += clock.c
bootblock-y += gpio.c
bootblock-$(CONFIG_SPI_FLASH) += spi.c
bootblock-y += timer.c
bootblock-$(CONFIG_DRIVERS_UART) += uart.c

verstage-y += clock.c
verstage-y += gpio.c
verstage-y += gsbi.c
verstage-y += i2c.c
verstage-y += qup.c
verstage-y += spi.c
verstage-y += timer.c
verstage-$(CONFIG_CONSOLE_SERIAL_IPQ806X) += uart.c

romstage-y += clock.c
romstage-y += gpio.c
romstage-$(CONFIG_SPI_FLASH) += spi.c
romstage-y += timer.c
romstage-$(CONFIG_DRIVERS_UART) += uart.c
romstage-y += cbmem.c

ramstage-y += cbmem.c
ramstage-y += clock.c
ramstage-y += gpio.c
ramstage-y += soc.c
ramstage-$(CONFIG_SPI_FLASH) += spi.c
ramstage-y += timer.c
ramstage-$(CONFIG_DRIVERS_UART) += uart.c
ramstage-y += usb.c

ifeq ($(CONFIG_USE_BLOBS),y)

# Generate the actual coreboot bootblock code
$(objcbfs)/bootblock.raw: $(objcbfs)/bootblock.elf
	@printf "    OBJCOPY    $(subst $(obj)/,,$(@))\n"
	$(OBJCOPY_bootblock) -O binary $< $@.tmp
	@mv $@.tmp $@

# Add MBN header to allow SBL3 to start coreboot bootblock
$(objcbfs)/bootblock.mbn: $(objcbfs)/bootblock.raw
	@printf "    ADD MBN    $(subst $(obj)/,,$(@))\n"
	./util/ipqheader/ipqheader.py $(call loadaddr,bootblock) $< $@.tmp
	@mv $@.tmp $@

# Create a complete bootblock which will start up the system
$(objcbfs)/bootblock.bin: $(call strip_quotes,$(CONFIG_SBL_BLOB)) \
			   $(objcbfs)/bootblock.mbn
	@printf "    MBNCAT     $(subst $(obj)/,,$(@))\n"
	@util/ipqheader/mbncat.py -o $@.tmp $^
	@mv $@.tmp $@

endif

CPPFLAGS_common += -Isrc/soc/qualcomm/ipq806x/include

# List of binary blobs coreboot needs in CBFS to be able to boot up this SOC
mbn-files := cdt.mbn  ddr.mbn  rpm.mbn tz.mbn

# Location of the binary blobs
mbn-root := 3rdparty/cpu/qualcomm/ipq806x

# Create make variables to aid cbfs-files-handler in processing the blobs (add
# them all as raw binaries at the root level).
$(foreach f,$(mbn-files),$(eval cbfs-files-y += $(f))\
	  $(eval $(f)-file := $(mbn-root)/$(f))\
	  $(eval $(f)-type := raw))
