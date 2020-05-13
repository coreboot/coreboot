## SPDX-License-Identifier: GPL-2.0-only

bootblock-y += bootblock.c
bootblock-y += memlayout.ld
bootblock-y += death.c

romstage-y += memlayout.ld
romstage-y += romstage.c
romstage-y += bdk_devicetree.c
romstage-y += death.c

ramstage-y += mainboard.c
ramstage-y += memlayout.ld
ramstage-y += bdk_devicetree.c
ramstage-y += death.c

verstage-y += memlayout.ld
verstage-y += death.c
