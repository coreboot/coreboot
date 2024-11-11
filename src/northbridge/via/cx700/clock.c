/* SPDX-License-Identifier: GPL-2.0-only */

#define __SIMPLE_DEVICE__
#include <console/console.h>
#include <device/pci_ops.h>
#include <static_devices.h>
#include <delay.h>

static unsigned int read_timer_fsb(void)
{
	/* Allows access to all northbridge PCI devfn's */
	pci_write_config8(_sdev_host_ctrl, 0x4f, 0x01);

	const u8 misc_1 = pci_read_config8(_sdev_host_if, 0x54);
	switch (misc_1 >> 5) {
	case 0:
		return 100;
	case 1:
		return 133;
	case 2:
		return 166;
	case 3:
		return 200;
	default:
		printk(BIOS_WARNING, "Unknown FSB frequency encoding: 0x%x\n", misc_1 >> 5);
		return 200;
	}
}

u32 get_timer_fsb(void)
{
	static unsigned int fsb_mhz;

	if (!fsb_mhz)
		fsb_mhz = read_timer_fsb();

	return fsb_mhz;
}
