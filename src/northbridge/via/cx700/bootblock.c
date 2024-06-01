/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/pci_ops.h>
#include <static_devices.h>
#include <arch/bootblock.h>

#define MISC_CONFIG_1		0x94
#define  SMBUS_CLOCK_SELECT	(1 << 7)

#define GP2_GP3_TIMER_CONTROL	0x98
#define  GP3_TIMER_TICK_SELECT	(3 << 4)

#define SMBUS_IO_BASE		0xd0
#define SMBUS_HOST_CONFIG	0xd2
#define  SMBUS_CLOCK_FROM_128K	(1 << 2)
#define  SMBUS_ENABLE		(1 << 0)

void bootblock_early_northbridge_init(void)
{
	pci_and_config8(_sdev_lpc, GP2_GP3_TIMER_CONTROL, ~GP3_TIMER_TICK_SELECT);

	pci_and_config8(_sdev_lpc, MISC_CONFIG_1, (u8)~SMBUS_CLOCK_SELECT); /* 14.318MHz */
	pci_write_config16(_sdev_lpc, SMBUS_IO_BASE, CONFIG_FIXED_SMBUS_IO_BASE);
	pci_or_config8(_sdev_lpc, SMBUS_HOST_CONFIG, SMBUS_CLOCK_FROM_128K | SMBUS_ENABLE);
}
