## SPDX-License-Identifier: GPL-2.0-only

bootblock-y += gpio.c
romstage-y += memory.c
ramstage-y += gpio.c

all-$(CONFIG_NC_FPGA_POST_CODE) += post.c

SPD_SOURCES = mc_ehl1	# 0b000
LIB_SPD_DEPS := $(foreach f, $(SPD_SOURCES), \
			src/mainboard/$(MAINBOARDDIR)/variants/$(VARIANT_DIR)/spd/$(f).spd.hex)
