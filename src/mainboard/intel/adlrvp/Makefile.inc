## SPDX-License-Identifier: GPL-2.0-only

subdirs-y += spd

bootblock-y += bootblock.c
bootblock-$(CONFIG_CHROMEOS) += chromeos.c
bootblock-y += early_gpio.c

verstage-$(CONFIG_CHROMEOS) += chromeos.c

romstage-$(CONFIG_CHROMEOS) += chromeos.c
romstage-y += romstage_fsp_params.c
romstage-y += board_id.c
romstage-y += memory.c

smm-y += smihandler.c

ramstage-$(CONFIG_CHROMEOS) += chromeos.c
ramstage-y += ec.c
ramstage-y += mainboard.c
ramstage-y += board_id.c
ramstage-y += gpio.c

CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/include

subdirs-y += variants/$(VARIANT_DIR)
