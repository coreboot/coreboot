## SPDX-License-Identifier: GPL-2.0-only

SPD_BIN = $(obj)/spd.bin

SPD_SOURCES  = elpida.hex    # 0b0000 Single Channel 2GB
SPD_SOURCES  += hynix.hex    # 0b0001 2GiB
SPD_SOURCES  += samsung.hex  # 0b0010 4GiB
SPD_DEPS := $(foreach f, $(SPD_SOURCES), src/mainboard/$(MAINBOARDDIR)/spd/$(f))

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
