## SPDX-License-Identifier: GPL-2.0-only

bootblock-$(CONFIG_CONSOLE_QEMU_DEBUGCON) += qemu_debugcon.c
romstage-$(CONFIG_CONSOLE_QEMU_DEBUGCON) += qemu_debugcon.c
postcar-$(CONFIG_CONSOLE_QEMU_DEBUGCON) += qemu_debugcon.c
ramstage-$(CONFIG_CONSOLE_QEMU_DEBUGCON) += qemu_debugcon.c

ramstage-$(CONFIG_DRIVERS_EMULATION_QEMU_BOCHS) += bochs.c
ramstage-$(CONFIG_DRIVERS_EMULATION_QEMU_CIRRUS) += cirrus.c
