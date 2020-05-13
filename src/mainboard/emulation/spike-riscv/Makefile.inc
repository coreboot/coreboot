## SPDX-License-Identifier: GPL-2.0-only

bootblock-y += uart.c
bootblock-y += rom_media.c
bootblock-y += clint.c
romstage-y += romstage.c
romstage-y += uart.c
romstage-y += rom_media.c
romstage-y += clint.c
ramstage-y += uart.c
ramstage-y += rom_media.c
ramstage-y += clint.c

bootblock-y += memlayout.ld
romstage-y += memlayout.ld
ramstage-y += memlayout.ld
