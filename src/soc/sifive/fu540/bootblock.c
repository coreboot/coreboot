/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <bootblock_common.h>
#include <console/console.h>
#include <soc/addressmap.h>

void bootblock_soc_init(void)
{
	printk(BIOS_INFO, "Boot mode: %d\n", read32((uint32_t *)FU540_MSEL));
}
