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

ramstage-$(CONFIG_CHROMEOS) += chromeos.c
ramstage-y += ec.c
ramstage-y += mainboard.c
ramstage-y += board_id.c
ramstage-y += gpio.c

ifeq ($(CONFIG_INTEL_GMA_ADD_VBT),y)
$(call add_vbt_to_cbfs, vbt_lp5.bin, 3rdparty/blobs/mainboard/$(MAINBOARDDIR)/$(VARIANT_DIR)/vbt_lp5.bin)
$(call add_vbt_to_cbfs, vbt_ddr5.bin, 3rdparty/blobs/mainboard/$(MAINBOARDDIR)/$(VARIANT_DIR)/vbt_ddr5.bin)
endif

CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/include

subdirs-y += variants/$(VARIANT_DIR)
