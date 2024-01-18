## SPDX-License-Identifier: GPL-2.0-or-later

bootblock-y += cache_as_ram_bootblock.S
bootblock-y += bootblock.c

romstage-y += ../intel/car/romstage.c

ramstage-y += qemu.c
