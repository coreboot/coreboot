#
# This file is part of the coreboot project.
#
#
# SPDX-License-Identifier: GPL-2.0-or-later

bootblock-y += bootblock.c

romstage-y += cbmem.c

bootblock-y += media.c
romstage-y += media.c
ramstage-y += media.c

bootblock-y += mmio.c
romstage-y += mmio.c
ramstage-y += mmio.c

bootblock-y += memlayout.ld
romstage-y += memlayout.ld
ramstage-y += memlayout.ld

bootblock-y += bootblock_custom.S

CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/include

BL31_MAKEARGS += PLAT=qemu ARM_ARCH_MAJOR=8 M0_CROSS_COMPILE="$(CROSS_COMPILE_arm)"
