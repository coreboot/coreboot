## SPDX-License-Identifier: GPL-2.0-only

subdirs-y += sdram_params/

decompressor-y += memlayout.ld

bootblock-y += bootblock.c
bootblock-y += chromeos.c
bootblock-y += memlayout.ld
bootblock-y += pwm_regulator.c
bootblock-y += boardid.c
bootblock-y += reset.c

verstage-y += chromeos.c
verstage-y += memlayout.ld
verstage-y += reset.c

romstage-y += boardid.c
romstage-y += chromeos.c
romstage-y += memlayout.ld
romstage-y += pwm_regulator.c
romstage-y += romstage.c
romstage-y += reset.c
romstage-y += sdram_configs.c

ramstage-y += boardid.c
ramstage-y += chromeos.c
ramstage-y += mainboard.c
ramstage-y += memlayout.ld
ramstage-y += reset.c
ramstage-y += sdram_configs.c  # Needed for ram_code()
