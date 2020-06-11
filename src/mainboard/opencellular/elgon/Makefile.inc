## SPDX-License-Identifier: GPL-2.0-only

bootblock-y += bootblock.c
bootblock-y += death.c

romstage-y += romstage.c
romstage-y += bdk_devicetree.c
romstage-y += death.c

ramstage-y += mainboard.c
ramstage-y += bdk_devicetree.c
ramstage-y += death.c

verstage-y += death.c
