## SPDX-License-Identifier: GPL-2.0-only

bootblock-$(CONFIG_CONSOLE_QEMU_DEBUGCON) += qemu_debugcon.c
romstage-$(CONFIG_CONSOLE_QEMU_DEBUGCON) += qemu_debugcon.c
postcar-$(CONFIG_CONSOLE_QEMU_DEBUGCON) += qemu_debugcon.c
ramstage-$(CONFIG_CONSOLE_QEMU_DEBUGCON) += qemu_debugcon.c

ramstage-$(CONFIG_DRIVERS_EMULATION_QEMU_BOCHS) += bochs.c
ramstage-$(CONFIG_DRIVERS_EMULATION_QEMU_CIRRUS) += cirrus.c

bootblock-$(CONFIG_DRIVERS_EMULATION_QEMU_FW_CFG) += fw_cfg.c
romstage-$(CONFIG_DRIVERS_EMULATION_QEMU_FW_CFG) += fw_cfg.c
verstage-$(CONFIG_DRIVERS_EMULATION_QEMU_FW_CFG) += fw_cfg.c
postcar-$(CONFIG_DRIVERS_EMULATION_QEMU_FW_CFG) += fw_cfg.c
ramstage-$(CONFIG_DRIVERS_EMULATION_QEMU_FW_CFG) += fw_cfg.c
