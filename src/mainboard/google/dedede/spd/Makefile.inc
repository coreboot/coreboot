## SPDX-License-Identifier: GPL-2.0-or-later

ifneq ($(SPD_SOURCES),)
SPD_BIN = $(obj)/spd.bin

SPD_DEPS := $(foreach f, $(SPD_SOURCES), src/soc/intel/jasperlake/spd/lp4x/$(f))

# Include spd ROM data
$(SPD_BIN): $(SPD_DEPS)
	for f in $+; \
	  do for c in $$(cat $$f | grep -v ^#); \
	    do printf $$(printf '\\%o' 0x$$c); \
	  done; \
	done > $@

cbfs-files-y += spd.bin
spd.bin-file := $(SPD_BIN)
spd.bin-type := spd
endif
