## SPDX-License-Identifier: GPL-2.0-only

bootblock-y += boardid.c
bootblock-y += chromeos.c
bootblock-y += bootblock.c
bootblock-y += reset.c

verstage-y += boardid.c
verstage-y += chromeos.c
verstage-y += reset.c

romstage-y += boardid.c
romstage-y += chromeos.c
romstage-y += romstage.c
romstage-y += reset.c

ramstage-y += boardid.c
ramstage-y += chromeos.c
ramstage-y += mainboard.c
ramstage-y += reset.c
