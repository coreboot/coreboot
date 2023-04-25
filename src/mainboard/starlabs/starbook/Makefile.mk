## SPDX-License-Identifier: GPL-2.0-only

CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/include
subdirs-$(CONFIG_HAVE_SPD_IN_CBFS) += ./spd
subdirs-y += variants/$(VARIANT_DIR)

bootblock-y += bootblock.c

verstage-$(CONFIG_VBOOT) += vboot.c

romstage-$(CONFIG_VBOOT) += vboot.c

ramstage-$(CONFIG_DRIVERS_OPTION_CFR) += cfr.c
ramstage-y += hda_verb.c
ramstage-y += mainboard.c
ramstage-y += smbios.c
ramstage-$(CONFIG_MAINBOARD_USE_LIBGFXINIT) += variants/$(VARIANT_DIR)/gma-mainboard.ads
