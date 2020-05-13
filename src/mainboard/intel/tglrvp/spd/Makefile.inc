## SPDX-License-Identifier: GPL-2.0-only

SPD_BIN = $(obj)/spd.bin

SPD_SOURCES = Micron-MT53D1G64D8SQ-046
SPD_SOURCES += Samsung-K4UBE3D4AA-MGCL
SPD_SOURCES += Hynix-H9HKNNNEBMAV-4267

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
