## SPDX-License-Identifier: GPL-2.0-only

all-y += ec.c
all-y += vpd.c

bootblock-y += bootblock.c
bootblock-y += early_gpio.c

romstage-y += romstage_fsp_params.c

ramstage-$(CONFIG_DRIVERS_OPTION_CFR) += cfr.c
ramstage-y += gpio.c
ramstage-y += mainboard.c
ramstage-y += ramstage_fsp_params.c
ramstage-y += smbios.c
