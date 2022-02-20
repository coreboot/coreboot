# SPDX-License-Identifier: GPL-2.0-only

bootblock-y += gpio.c
romstage-y += gpio.c

bootblock-y += early_init.c
romstage-y += early_init.c

bootblock-y += sch5545_ec_early.c

romstage-y += sch5545_ec.c
ramstage-y += sch5545_ec.c

ramstage-$(CONFIG_MAINBOARD_USE_LIBGFXINIT) += gma-mainboard.ads

ramstage-y += variants/$(VARIANT_DIR)/hda_verb.c

ifeq ($(CONFIG_INCLUDE_SMSC_SCH5545_EC_FW),y)
cbfs-files-y += sch5545_ecfw.bin
sch5545_ecfw.bin-file := $(call strip_quotes,$(CONFIG_SMSC_SCH5545_EC_FW_FILE))
sch5545_ecfw.bin-type := raw
endif

subdirs-y += variants/baseboard
CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/variants/baseboard/include

subdirs-y += variants/$(VARIANT_DIR)
CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/variants/$(VARIANT_DIR)/include
