## SPDX-License-Identifier: GPL-2.0-only

romstage-y += romstage.c
romstage-y += spd_util.c

ramstage-y += gpio.c
ramstage-y += ramstage.c

SPD_BIN = $(obj)/spd.bin

SPD_SOURCES = samsung_2GiB_dimm_K4E8E304EE-EGCF
SPD_SOURCES += hynix_2GiB_dimm_H9CCNNN8GTMLAR-NUD
SPD_SOURCES += micron_2GiB_dimm_MT52L256M32D1PF
SPD_SOURCES += samsung_2GiB_dimm_K4E8E324EB-EGCF
SPD_SOURCES += micron_2GiB_dimm_EDF8132A3MA-JD-F

SPD_DEPS := $(foreach f, $(SPD_SOURCES), src/mainboard/$(MAINBOARDDIR)/spd/$(f).spd.hex)

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
