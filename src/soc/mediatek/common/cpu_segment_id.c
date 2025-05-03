/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#include <console/console.h>
#include <device/mmio.h>
#include <soc/cpu_id.h>
#include <soc/efuse.h>

u32 get_cpu_segment_id(void)
{
	u32 id = read32(&mtk_efuse->cpu_seg_id_reg);

	printk(BIOS_INFO, "CPU Segment ID: %#x\n", id);

	return id;
}
