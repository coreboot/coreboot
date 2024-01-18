## SPDX-License-Identifier: GPL-2.0-only

bootblock-y += bootblock.c
bootblock-y += leds.c
bootblock-y += sd_media.c

romstage-y += romstage.c
romstage-y += sd_media.c

ramstage-y += sd_media.c
