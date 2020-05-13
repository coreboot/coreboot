## SPDX-License-Identifier: GPL-2.0-only

bootblock-y += bootblock.c
bootblock-y += uart.c
romstage-y += cbmem.c
romstage-y += romstage.c
ramstage-y += timer.c
romstage-y += uart.c
ramstage-y += uart.c

bootblock-y += memlayout.ld
romstage-y += memlayout.ld
ramstage-y += memlayout.ld
