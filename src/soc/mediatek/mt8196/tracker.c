/* SPDX-License-Identifier: GPL-2.0-only */

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

static const u32 offsets[] = {
	AR_TRACK_LOG_OFFSET, AR_ENTRY_ID_OFFSET, AR_TRACK_L_OFFSET,
	AR_TRACK_H_OFFSET, AW_TRACK_LOG_OFFSET, AW_ENTRY_ID_OFFSET,
	AW_TRACK_L_OFFSET, AW_TRACK_H_OFFSET,
};

struct tracker tracker_data[TRACKER_NUM] = {
	[TRACKER_SYSTRACKER] = {
		.base_addr = INFRA_TRACKER_BASE,
		.timeout = BUS_DBG_CON_TIMEOUT,
		.entry = SYS_TRACK_ENTRY,
		.offsets = offsets,
		.offsets_size = ARRAY_SIZE(offsets),
		.str = "systracker",
	},
	[TRACKER_INFRATRACKER] = {
		.base_addr = INFRA_TRACKER_BASE,
		.timeout = BUS_DBG_CON_TIMEOUT,
		.entry = INFRA_ENTRY_NUM,
		.offsets = offsets,
		.offsets_size = ARRAY_SIZE(offsets),
		.str = "infra_tracker",
	},
	[TRACKER_VLPSYSTRACKER] = {
		.base_addr = VLP_TRACKER_BASE,
		.timeout = BUS_DBG_CON_TIMEOUT,
		.entry = VLP_ENTRY_NUM,
		.offsets = offsets,
		.offsets_size = ARRAY_SIZE(offsets),
		.str = "vlp_tracker",
	},
};

void tracker_setup(void)
{
	u32 val;
	/*
	 * Set infra/peri tracker timeout.
	 * timeout = clock_in_mhz / 15 * timeout_in_us
	 *
	 * timeout: 10ms
	 * ap tracker clock: 26MHz
	 * infra tracker clock: 26MHz
	 * vlp tracker clock: 26MHz
	 */
	val = 26 / 15 * 10000;

	write32p(BUS_TRACE_MONITOR_BASE + BUS_TRACE_CON_AO_PRESCALE, val);
	write32p(BUS_TRACE_MONITOR_BASE + BUS_TRACE_CON_AO_1_PRESCALE, val);
	write32p(VLP_CFG_BASE + VLP_TRACE_CON_AO_PRESCALE, val);
	/*
	 * Enable infra/peri tracker.
	 * bit[0] - BUS_DBG_EN
	 * bit[1] - TIMEOUT_EN
	 * bit[2] - SLV_ERR_EN
	 * bit[13] - HALT_ON_TIMEOUT_EN
	 * bit[14] - BUS_OT_WEN_CTRL
	 */
	val = BIT(0) | BIT(1) | BIT(2) | BIT(13) | BIT(14);
	write32p(VLP_AO_BASE + VLP_CON_AO, val);
	write32p(BUS_TRACE_MONITOR_BASE + BUS_TRACE_CON_AO_1, val);
	write32p(BUS_TRACE_MONITOR_BASE + BUS_TRACE_CON_AO_2, val);
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
