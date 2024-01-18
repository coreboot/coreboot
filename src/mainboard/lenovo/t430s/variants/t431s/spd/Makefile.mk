## SPDX-License-Identifier: GPL-2.0-only

SPD_SOURCES  = samsung_4gb  # 0b0010 4GiB

LIB_SPD_DEPS := $(foreach f, $(SPD_SOURCES), src/mainboard/$(MAINBOARDDIR)/variants/$(VARIANT_DIR)/spd/$(f).spd.hex)
