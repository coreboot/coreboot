## SPDX-License-Identifier: GPL-2.0-only

bootblock-y += memlayout.ld
bootblock-y += bootblock.c
bootblock-y += chromeos.c
bootblock-$(CONFIG_VBOOT_CBFS_INTEGRATION) += reset.c

verstage-y += memlayout.ld
verstage-y += chromeos.c
verstage-y += reset.c
verstage-y += verstage.c

romstage-y += memlayout.ld
romstage-y += chromeos.c
romstage-y += regulator.c
romstage-y += reset.c
romstage-y += romstage.c

ramstage-y += memlayout.ld
ramstage-y += boardid.c
ramstage-y += chromeos.c
ramstage-y += mainboard.c
ramstage-y += panel.c
ramstage-y += regulator.c
ramstage-y += reset.c
ramstage-$(CONFIG_BOARD_GOOGLE_CIRI) += panel_ciri.c
ramstage-$(CONFIG_BOARD_GOOGLE_GERALT) += panel_geralt.c
