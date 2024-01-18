## SPDX-License-Identifier: GPL-2.0-only

bootblock-y += gpio.c
romstage-y += memory.c
ramstage-y += gpio.c
ramstage-y += mainboard.c

SPD_SOURCES = Micron_MT53E512M32D1NP-046WTB.spd.hex
LIB_SPD_CBFS := $(foreach f, $(SPD_SOURCES), \
		src/mainboard/$(MAINBOARDDIR)/variants/$(VARIANT_DIR)/spd/$(f).spd.hex)
