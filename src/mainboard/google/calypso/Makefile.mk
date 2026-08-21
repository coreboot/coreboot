## SPDX-License-Identifier: GPL-2.0-only

all-y += board_misc.c
all-y += chromeos.c
ifneq ($(CONFIG_MISSING_BOARD_RESET),y)
all-y += reset.c
endif

bootblock-y += bootblock.c

romstage-y += romstage.c

romstage-y += charging.c
romstage-$(CONFIG_SOC_QUALCOMM_CDT) += fw_config_calypso.c

ramstage-y += charging.c
ramstage-$(CONFIG_SOC_QUALCOMM_CDT) += fw_config_calypso.c

ramstage-y += mainboard.c

ramstage-y += display.c
