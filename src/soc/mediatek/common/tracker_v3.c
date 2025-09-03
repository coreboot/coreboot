/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#include <commonlib/bsd/helpers.h>
#include <console/console.h>
#include <device/mmio.h>
#include <soc/addressmap.h>
#include <soc/tracker.h>

/*
 * for systracker:
 * offset[0] dump from offset 0x100 ~ 0x2F8
 * offset[1] dump from offset 0x300 ~ 0x4FC
 *
 * for infra tracker:
 * offset[0] dump from offset 0x100 ~ 0x1F8
 * offset[1] dump from offset 0x300 ~ 0x3FC
 *
 * for vlpsys tracker:
 * offset[0] dump from offset 0x100 ~ 0x2F8
 * offset[1] dump from offset 0x300 ~ 0x4FC
 */

const u32 tracker_v3_offsets[TRACKER_V3_OFFSETS_SIZE] = {
	AR_TRACK_LOG_OFFSET, AR_ENTRY_ID_OFFSET, AR_TRACK_L_OFFSET,
	AR_TRACK_H_OFFSET, AW_TRACK_LOG_OFFSET, AW_ENTRY_ID_OFFSET,
	AW_TRACK_L_OFFSET, AW_TRACK_H_OFFSET,
};

__weak void tracker_setup(void)
{
	/* do nothing. */
}

static void tracker_dump_data(void)
{
	u64 reg_entry;
	int i, j;
	uintptr_t reg_log, reg_id, reg_low, reg_high;
	struct tracker *tra;

	for (j = 0; j < TRACKER_NUM; j++) {
		tra = &tracker_data[j];

		if (!(read32p(tra->base_addr) & tra->timeout))
			continue;
		printk(BIOS_INFO, "**Dump %s ar debug register start**\n", tra->str);
		for (i = 0; i < tra->entry; i++) {
			reg_log = tra->base_addr + tra->offsets[0] + i * 4;
			reg_id = tra->base_addr + tra->offsets[1] + i * 4;
			reg_low = tra->base_addr + tra->offsets[2] + i * 4;
			reg_high = tra->base_addr + tra->offsets[3] + i * 4;
			reg_entry = ((u64)read32p(reg_high)) << 32 | read32p(reg_low);
			printk(BIOS_INFO, "%#lx:%#x:%#x:%#x:%#x:%#llx\n",
			       reg_low, read32p(reg_log), read32p(reg_id), read32p(reg_low),
			       read32p(reg_high), reg_entry);
		}
		printk(BIOS_INFO, "**Dump %s aw debug register start**\n", tra->str);
		for (i = 0; i < tra->entry; i++) {
			reg_log = tra->base_addr + tra->offsets[4] + i * 4;
			reg_id = tra->base_addr + tra->offsets[5] + i * 4;
			reg_low = tra->base_addr + tra->offsets[6] + i * 4;
			reg_high = tra->base_addr + tra->offsets[7] + i * 4;
			reg_entry = ((u64)read32p(reg_high)) << 32 | read32p(reg_low);
			printk(BIOS_INFO, "%#lx:%#x:%#x:%#x:%#x:%#llx\n",
			       reg_low, read32p(reg_log), read32p(reg_id), read32p(reg_low),
			       read32p(reg_high), reg_entry);
		}
		printk(BIOS_INFO, "**Dump %s debug register end**\n", tra->str);
	}
}

void bustracker_init(void)
{
	tracker_dump_data();
	tracker_setup();
}
