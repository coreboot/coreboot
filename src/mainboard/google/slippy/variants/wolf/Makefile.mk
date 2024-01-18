## SPDX-License-Identifier: GPL-2.0-only

# Order of names in SPD_SOURCES is important!
SPD_SOURCES  = Micron_4KTF25664HZ	# 0: 4GB / CH0 + CH1
SPD_SOURCES += Hynix_HMT425S6AFR6A	# 1: 4GB / CH0 + CH1
SPD_SOURCES += Samsung_K4B4G1646B	# 2: 4GB / CH0 + CH1
SPD_SOURCES += Micron_4KTF25664HZ	# 3: 2GB / CH0
SPD_SOURCES += Hynix_HMT425S6AFR6A	# 4: 2GB / CH0
SPD_SOURCES += Samsung_K4B4G1646B	# 4: 2GB / CH0

LIB_SPD_DEPS := $(foreach f, $(SPD_SOURCES), src/mainboard/$(MAINBOARDDIR)/variants/$(VARIANT_DIR)/spd/$(f).spd.hex)
