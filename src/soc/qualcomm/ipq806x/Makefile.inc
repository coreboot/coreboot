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

bootblock-y += bootblock.c
bootblock-y += cbfs.c
bootblock-y += clock.c
bootblock-y += gpio.c
bootblock-y += timer.c

romstage-y += cbfs.c
romstage-y += clock.c
romstage-y += gpio.c
romstage-y += timer.c

ramstage-y += cbfs.c
ramstage-y += clock.c
ramstage-y += gpio.c
ramstage-y += timer.c

ifeq ($(CONFIG_USE_BLOBS),y)

# Generate the actual coreboot bootblock code
$(objcbfs)/bootblock.raw: $(objcbfs)/bootblock.elf
	@printf "    OBJCOPY    $(subst $(obj)/,,$(@))\n"
	$(OBJCOPY_bootblock) -O binary $< $@.tmp
	@mv $@.tmp $@

# Add MBN header to allow SBL3 to start coreboot bootblock
$(objcbfs)/bootblock.mbn: $(objcbfs)/bootblock.raw
	@printf "    ADD MBN    $(subst $(obj)/,,$(@))\n"
	./util/ipqheader/ipqheader.py $(CONFIG_BOOTBLOCK_BASE) $< $@.tmp
	@mv $@.tmp $@

# Create a complete bootblock which will start up the system
$(objcbfs)/bootblock.bin: $(call strip_quotes,$(CONFIG_SBL_BLOB)) \
			   $(objcbfs)/bootblock.mbn
	@printf "    CAT    $(subst $(obj)/,,$(@))\n"
	@cat $^  >  $@.tmp
	@mv $@.tmp $@

endif

CPPFLAGS_common += -Isrc/soc/qualcomm/ipq806x/include
