#
# This file is part of the coreboot project.
#
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; version 2 of the License.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#

ifeq ($(CONFIG_AHCI_BIOS),y)
stripped_ahcibios_id = $(call strip_quotes,$(CONFIG_AHCI_BIOS_ID))
cbfs-files-$(CONFIG_AHCI_BIOS) += pci$(stripped_ahcibios_id).rom
pci$(stripped_ahcibios_id).rom-file := $(call strip_quotes,$(CONFIG_AHCI_BIOS_FILE))
pci$(stripped_ahcibios_id).rom-type := optionrom
endif

romstage-y += buildOpts.c
romstage-y += BiosCallOuts.c
romstage-y += OemCustomize.c

ramstage-y += buildOpts.c
ramstage-y += BiosCallOuts.c
ramstage-y += OemCustomize.c

## DIMM SPD for on-board memory
SPD_BIN = $(obj)/spd.bin

# Order of names in SPD_SOURCES is important!
SPD_SOURCES  = Elpida_EDJ2116DEBG

SPD_DEPS := $(foreach f, $(SPD_SOURCES), src/mainboard/$(MAINBOARDDIR)/$(f).spd.hex)

# Include spd ROM data
$(SPD_BIN): $(SPD_DEPS)
	for f in $+; \
	  do for c in $$(cat $$f | grep -v ^#); \
	    do printf $$(printf '\%o' 0x$$c); \
	  done; \
	done > $@

cbfs-files-y += spd.bin
spd.bin-file := $(SPD_BIN)
spd.bin-type := spd
