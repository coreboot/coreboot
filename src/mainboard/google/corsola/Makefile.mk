## SPDX-License-Identifier: GPL-2.0-only

bootblock-y += memlayout.ld
bootblock-y += bootblock.c
bootblock-y += chromeos.c

verstage-y += memlayout.ld
verstage-y += chromeos.c
verstage-y += reset.c

romstage-y += memlayout.ld
romstage-y += boardid.c
romstage-y += chromeos.c
romstage-y += regulator.c
romstage-y += reset.c
romstage-y += romstage.c

ramstage-y += memlayout.ld
ramstage-y += boardid.c
ramstage-y += chromeos.c
ramstage-y += mainboard.c
ramstage-y += panel.c
ramstage-y += panel_anx7625.c
ramstage-y += panel_ps8640.c
ramstage-y += panel_tps65132s.c

ramstage-y += regulator.c
ramstage-y += reset.c

ramstage-$(CONFIG_BOARD_GOOGLE_STARMIE) += panel_starmie.c
ramstage-$(CONFIG_BOARD_GOOGLE_WUGTRIO) += panel_wugtrio.c
ramstage-$(CONFIG_BOARD_GOOGLE_WYRDEER) += panel_wyrdeer.c
