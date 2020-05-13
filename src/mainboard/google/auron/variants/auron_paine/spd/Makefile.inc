## SPDX-License-Identifier: GPL-2.0-only

romstage-y += spd.c

SPD_BIN = $(obj)/spd.bin

# { GPIO47, GPIO9, GPIO13 }
SPD_SOURCES  = Micron_4KTF25664HZ			# 0b0000
SPD_SOURCES += Hynix_HMT425S6AFR6A			# 0b0001
SPD_SOURCES += Hynix_HMT425S6CFR6A_H5TC4G63CFR		# 0b0010
SPD_SOURCES += Micron_4KTF25664HZ			# 0b0011
SPD_SOURCES += Micron_4KTF25664HZ			# 0b0100
SPD_SOURCES += Hynix_HMT425S6AFR6A			# 0b0101
SPD_SOURCES += Hynix_HMT425S6CFR6A_H5TC4G63CFR		# 0b0110
SPD_SOURCES += empty					# 0b0111
SPD_SOURCES += empty					# 0b1000
SPD_SOURCES += empty					# 0b1001
SPD_SOURCES += empty					# 0b1010
SPD_SOURCES += empty					# 0b1011
SPD_SOURCES += empty					# 0b1100
SPD_SOURCES += empty					# 0b1101
SPD_SOURCES += empty					# 0b1110
SPD_SOURCES += empty					# 0b1111

SPD_DEPS := $(foreach f, $(SPD_SOURCES), src/mainboard/$(MAINBOARDDIR)/variants/$(VARIANT_DIR)/spd/$(f).spd.hex)

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
