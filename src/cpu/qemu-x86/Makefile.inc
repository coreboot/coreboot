## SPDX-License-Identifier: GPL-2.0-or-later

bootblock-y += cache_as_ram_bootblock.S
bootblock-y += bootblock.c
bootblock-$(CONFIG_ARCH_BOOTBLOCK_X86_64) += $(top)/src/arch/x86/walkcbfs.S

romstage-y += ../intel/car/romstage.c

ramstage-y += qemu.c

subdirs-y += ../x86/mtrr
subdirs-y += ../x86/lapic
subdirs-y += ../x86/smm
subdirs-y += ../x86/tsc
