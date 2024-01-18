## SPDX-License-Identifier: GPL-2.0-only

subdirs-y += sdram_params/

bootblock-y += bootblock.c
bootblock-y += chromeos.c
bootblock-y += pwm_regulator.c
bootblock-y += boardid.c
bootblock-y += reset.c

verstage-y += chromeos.c
verstage-y += reset.c

romstage-y += boardid.c
romstage-y += chromeos.c
romstage-y += pwm_regulator.c
romstage-y += romstage.c
romstage-y += reset.c
romstage-y += sdram_configs.c

ramstage-y += boardid.c
ramstage-y += chromeos.c
ramstage-y += mainboard.c
ramstage-y += reset.c
ramstage-y += sdram_configs.c  # Needed for ram_code()
