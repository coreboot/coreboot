#
# This file is part of the coreboot project.
#
# Copyright (C) 2014 Imagination Technologies
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation; version 2 of
# the License.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc.
#

ifeq ($(CONFIG_CPU_IMGTEC_PISTACHIO),y)

# We enable CBFS_SPI_WRAPPER for Pistachio targets.
bootblock-y += clocks.c
bootblock-y += spi.c
romstage-y += spi.c
ramstage-y += spi.c

ifeq ($(CONFIG_DRIVERS_UART),y)
bootblock-$(CONFIG_BOOTBLOCK_CONSOLE) += uart.c
romstage-y += uart.c
ramstage-y += uart.c
endif

bootblock-y += monotonic_timer.c

ramstage-y += cbmem.c
ramstage-y += monotonic_timer.c
ramstage-y += soc.c

romstage-y += cbmem.c
romstage-y += ddr2_init.c
romstage-y += ddr3_init.c
romstage-y += romstage.c
romstage-y += monotonic_timer.c

CPPFLAGS_common += -Isrc/soc/imgtec/pistachio/include/

# Create a complete bootblock which will start up the system
$(objcbfs)/bootblock.bin: $(objcbfs)/bootblock.raw.bin $(BIMGTOOL)
	@printf "    BIMGTOOL   $(subst $(obj)/,,$(@))\n"
	$(BIMGTOOL) $< $@ $(call loadaddr,bootblock)

endif
