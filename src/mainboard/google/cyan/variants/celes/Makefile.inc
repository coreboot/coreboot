## SPDX-License-Identifier: GPL-2.0-only

romstage-y += spd_util.c

ramstage-y += gpio.c
ramstage-y += ramstage.c

SPD_BIN = $(obj)/spd.bin

SPD_SOURCES = samsung_dimm_K4E8E304EE-EGCE    # Index0
SPD_SOURCES += samsung_dimm_K4E8E324EB-EGCF   # Index1

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
