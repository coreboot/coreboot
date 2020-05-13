## SPDX-License-Identifier: GPL-2.0-only

romstage-y += spd_util.c

ramstage-y += gpio.c

SPD_BIN = $(obj)/spd.bin

SPD_SOURCES =  samsung_2GiB_dimm_K4B4G1646Q-HYK0
SPD_SOURCES += hynix_2GiB_dimm_HMT425S6CFR6A_H5TC4G63CFR
SPD_SOURCES += samsung_2GiB_dimm_K4B4G1646Q-HYK0
SPD_SOURCES += hynix_2GiB_dimm_HMT425S6CFR6A_H5TC4G63CFR
SPD_SOURCES += samsung_2GiB_dimm_K4B4G1646E-BYK0
SPD_SOURCES += micron_2GiB_dimm_MT41K256M16TW-107
SPD_SOURCES += samsung_2GiB_dimm_K4B4G1646E-BYK0
SPD_SOURCES += micron_2GiB_dimm_MT41K256M16TW-107

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
