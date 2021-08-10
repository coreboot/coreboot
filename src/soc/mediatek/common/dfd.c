/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/mmio.h>
#include <soc/dfd.h>

void dfd_init(void)
{
	printk(BIOS_INFO, "%s: enable DFD (Design For Debug)\n", __func__);
	setbits32(dfd_cfg, RESET_ON_KEEP_EN);
	dsb();
}
