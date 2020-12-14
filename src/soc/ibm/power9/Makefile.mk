## SPDX-License-Identifier: GPL-2.0-only

ifeq ($(CONFIG_SOC_IBM_POWER9),y)

bootblock-y += bootblock.c
bootblock-y += rom_media.c

romstage-y += cbmem.c
romstage-y += rom_media.c
romstage-y += romstage.c

ramstage-y += cbmem.c
ramstage-y += chip.c
ramstage-y += rom_media.c
ramstage-y += timer.c

endif
