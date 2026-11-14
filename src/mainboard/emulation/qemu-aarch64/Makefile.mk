# SPDX-License-Identifier: GPL-2.0-or-later

bootblock-y += bootblock.c

romstage-y += cbmem.c

bootblock-y += media.c
romstage-y += media.c
ramstage-y += media.c

bootblock-y += mmio.c
romstage-y += mmio.c
ramstage-y += mmio.c

ramstage-y += cpus.c

bootblock-y += bootblock_custom.S

all-y += fw_cfg.c

CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/include

# Build BL31 with GICv2 support
BL31_MAKEARGS += PLAT=qemu ARM_ARCH_MAJOR=8 M0_CROSS_COMPILE="$(CROSS_COMPILE_arm)"
