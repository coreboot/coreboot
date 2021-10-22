## SPDX-License-Identifier: GPL-2.0-or-later

ramstage-y += slot_1.c
ramstage-y += l2_cache.c
subdirs-y += ../model_6xx
subdirs-y += ../model_65x
subdirs-y += ../model_67x
subdirs-y += ../model_68x
subdirs-y += ../model_6bx
subdirs-y += ../../x86/lapic
subdirs-y += ../../x86/cache
subdirs-y += ../microcode

bootblock-y += ../car/p3/cache_as_ram.S
bootblock-y += ../car/bootblock.c
postcar-y += ../car/p4-netburst/exit_car.S
romstage-y += ../car/romstage.c
