# SPDX-License-Identifier: GPL-2.0-or-later

bootblock-y += bootblock.c

romstage-y += cbmem.c

bootblock-y += media.c
romstage-y += media.c
ramstage-y += media.c

bootblock-y += mmio.c
romstage-y += mmio.c
ramstage-y += mmio.c

ramstage-y += acpi.c

bootblock-y += bootblock_custom.S

CPPFLAGS_common += -mcmodel=large -I$(src)/mainboard/$(MAINBOARDDIR)/include

build_complete::
	@printf "Truncating coreboot.rom to 256M\n"
	truncate -s 256M $(obj)/coreboot.rom
