/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <device/device.h>
#include <device/mmio.h>
#include <soc/iomap.h>

#define BIOS_CONTROL_REG	0xFC
#define   BIOS_CONTROL_WPD	(1 << 0)

static void mainboard_enable(struct device *dev)
{
	volatile void *addr = (void *)(SPI_BASE_ADDRESS + BIOS_CONTROL_REG);

	/* Set Bios Write Protect Disable bit to allow saving MRC cache */
	write8(addr, read8(addr) | BIOS_CONTROL_WPD);
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
