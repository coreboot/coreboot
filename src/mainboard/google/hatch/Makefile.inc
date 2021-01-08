## SPDX-License-Identifier: GPL-2.0-only

bootblock-y += bootblock.c
bootblock-$(CONFIG_CHROMEOS) += chromeos.c

ramstage-y += ramstage.c
ramstage-$(CONFIG_CHROMEOS) += chromeos.c
ramstage-$(CONFIG_EC_GOOGLE_CHROMEEC) += ec.c

romstage-$(CONFIG_ROMSTAGE_SPD_CBFS) += romstage_spd_cbfs.c
romstage-$(CONFIG_ROMSTAGE_SPD_SMBUS) += romstage_spd_smbus.c
romstage-$(CONFIG_CHROMEOS) += chromeos.c

verstage-$(CONFIG_CHROMEOS) += chromeos.c

subdirs-y += variants/baseboard
CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/variants/baseboard/include

VARIANT_DIR:=$(call strip_quotes,$(CONFIG_VARIANT_DIR))
subdirs-y += variants/$(VARIANT_DIR)
CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/variants/$(VARIANT_DIR)/include

subdirs-$(CONFIG_ROMSTAGE_SPD_CBFS) += spd
