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
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
# MA 02110-1301 USA
#

# We enable CBFS_SPI_WRAPPER for Pistachio targets.
bootblock-y += spi.c
romstage-y += spi.c
ramstage-y += spi.c

ifeq ($(CONFIG_DRIVERS_UART),y)
bootblock-$(CONFIG_BOOTBLOCK_CONSOLE) += uart.c
romstage-y += uart.c
ramstage-y += uart.c
endif

bootblock-y += monotonic_timer.c
bootblock-y += timestamp.c

ramstage-y += cbmem.c
ramstage-y += monotonic_timer.c
ramstage-y += timestamp.c

romstage-y += cbmem.c
romstage-y += romstage.c
romstage-y += monotonic_timer.c
romstage-y += timestamp.c

# Generate the actual coreboot bootblock code
$(objcbfs)/bootblock.raw: $(objcbfs)/bootblock.elf
	@printf "    OBJCOPY    $(subst $(obj)/,,$(@))\n"
	$(OBJCOPY_bootblock) -O binary $< $@.tmp
	@mv $@.tmp $@

# Create a complete bootblock which will start up the system
$(objcbfs)/bootblock.bin: $(objcbfs)/bootblock.raw $(BIMGTOOL)
	@printf "    BIMGTOOL   $(subst $(obj)/,,$(@))\n"
	$(BIMGTOOL) $< $@ $(CONFIG_BOOTBLOCK_BASE)

