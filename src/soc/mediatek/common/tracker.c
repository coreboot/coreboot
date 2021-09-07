/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdio.h>
#include <console/console.h>
#include <device/mmio.h>
#include <soc/addressmap.h>
#include <soc/tracker_common.h>

static struct tracker {
	uintptr_t base_addr;
	u32 timeout;
	u32 entry;
	u32 offset[2];
	const char *str;
} tracker_data[TRACKER_NUM] = {
	[TRACKER_SYSTRACKER] = {
		.base_addr = DBG_TRACKER_BASE,
		.timeout = BUS_DBG_CON_TIMEOUT,
		.entry = SYS_TRACK_ENTRY,
		.offset[0] = AR_TRACK_OFFSET,
		.offset[1] = AW_TRACK_OFFSET,
		.str = "systracker",
	},
	[TRACKER_INFRATRACKER] = {
		.base_addr = INFRA_TRACKER_BASE,
		.timeout = BUSTRACKER_TIMEOUT,
		.entry = INFRA_ENTRY_NUM,
		.offset[0] = AR_TRACK_OFFSET,
		.offset[1] = AW_TRACK_OFFSET,
		.str = "infra_tracker",
	},
	[TRACKER_PERISYSTRACKER] = {
		.base_addr = PERI_TRACKER_BASE,
		.timeout = BUSTRACKER_TIMEOUT,
		.entry = PERI_ENTRY_NUM,
		.offset[0] = AR_TRACK_OFFSET,
		.offset[1] = AW_TRACK_OFFSET,
		.str = "peri_tracker",
	},
};

static void setup_init(void)
{
	u32 val;
	/*
	 * Set infra/peri tracker timeout.
	 * timeout = clock_in_mhz * 1000 / 16 * timeout_in_ms
	 *
	 * timeout: 200ms
	 * infra tracker clock: 156MHz
	 * peri tracker clock: 78MHz
	 */
	val = 156 * 1000 / 16 * 200;
	write32((void *)(INFRA_TRACKER_BASE + BUS_DBG_TIMER_CON0), val);
	write32((void *)(INFRA_TRACKER_BASE + BUS_DBG_TIMER_CON1), val);

	val = 78 * 1000 / 16 * 200;
	write32((void *)(PERI_TRACKER_BASE + BUS_DBG_TIMER_CON0), val);
	write32((void *)(PERI_TRACKER_BASE + BUS_DBG_TIMER_CON1), val);

	/* Enable infra/peri tracer because tracker and tracer share the same enable bit. */
	write32((void *)(BUS_TRACE_MONITOR_BASE + BUS_TRACE_CON_1), 1 << BUS_TRACE_EN);
	write32((void *)(BUS_TRACE_MONITOR_BASE + BUS_TRACE_CON_2), 1 << BUS_TRACE_EN);

	/*
	 * Enable infra/peri tracker.
	 * bit[0] - BUS_DBG_EN
	 * bit[1] - TIMEOUT_EN
	 * bit[2] - SLV_ERR_EN
	 * bit[13] - HALT_ON_TIMEOUT_EN
	 * bit[14] - BUS_OT_WEN_CTRL
	 */
	val = BIT(0) | BIT(1) | BIT(2) | BIT(13) | BIT(14);
	write32((void *)(BUS_TRACE_MONITOR_BASE + BUS_TRACE_CON_AO_1), val);
	write32((void *)(BUS_TRACE_MONITOR_BASE + BUS_TRACE_CON_AO_2), val);

}

static void tracker_dump_data(void)
{
	int i, j, k;
	int size;
	uintptr_t reg;
	struct tracker *tra;

	for (j = 0; j < TRACKER_NUM; j++) {
		tra = &tracker_data[j];

		if (!(read32((void *)(tra->base_addr)) & tra->timeout))
			continue;

		printk(BIOS_INFO, "**Dump %s debug register start**\n", tra->str);
		for (k = 0; k < 2; k++) {
			/*
			 * for systracker:
			 * offset[0] dump from offset 0x100 ~ 0x2F8.
			 * offset[1] dump from offset 0x300 ~ 0x4FC
			 *
			 * for infra tracker:
			 * offset[0] dump from offset 0x100 ~ 0x1F8
			 * offset[1] dump from offset 0x300 ~ 0x3FC
			 *
			 * for perisys tracker:
			 * offset[0] dump from offset 0x100 ~ 0x2F8
			 * offset[1] dump from offset 0x300 ~ 0x4FC
			 */
			size = 2 * tra->entry;
			for (i = 0; i < size; i++) {
				reg = tra->base_addr + tra->offset[k] + i * 4;
				printk(BIOS_INFO, "%#lx:%#x,", reg, read32((void *)reg));

				if (i % 4 == 3 || i == size - 1)
					printk(BIOS_INFO, "\n");
			}
		}

		printk(BIOS_INFO, "**Dump %s debug register end**\n", tra->str);
	}
}

void bustracker_init(void)
{
	tracker_dump_data();
	setup_init();
}
