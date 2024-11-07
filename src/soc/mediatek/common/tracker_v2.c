/* SPDX-License-Identifier: GPL-2.0-only */

#include <commonlib/bsd/helpers.h>
#include <device/mmio.h>
#include <soc/addressmap.h>
#include <soc/tracker.h>
#include <soc/tracker_common.h>

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

static const u32 offsets[] = { AR_TRACK_OFFSET, AW_TRACK_OFFSET };

struct tracker tracker_data[TRACKER_NUM] = {
	[TRACKER_SYSTRACKER] = {
		.base_addr = DBG_TRACKER_BASE,
		.timeout = BUS_DBG_CON_TIMEOUT,
		.entry = SYS_TRACK_ENTRY,
		.offsets = offsets,
		.offsets_size = ARRAY_SIZE(offsets),
		.str = "systracker",
	},
	[TRACKER_INFRATRACKER] = {
		.base_addr = INFRA_TRACKER_BASE,
		.timeout = BUSTRACKER_TIMEOUT,
		.entry = INFRA_ENTRY_NUM,
		.offsets = offsets,
		.offsets_size = ARRAY_SIZE(offsets),
		.str = "infra_tracker",
	},
	[TRACKER_PERISYSTRACKER] = {
		.base_addr = PERI_TRACKER_BASE,
		.timeout = BUSTRACKER_TIMEOUT,
		.entry = PERI_ENTRY_NUM,
		.offsets = offsets,
		.offsets_size = ARRAY_SIZE(offsets),
		.str = "peri_tracker",
	},
};

void tracker_setup(void)
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
	write32p(INFRA_TRACKER_BASE + BUS_DBG_TIMER_CON0, val);
	write32p(INFRA_TRACKER_BASE + BUS_DBG_TIMER_CON1, val);

	val = 78 * 1000 / 16 * 200;
	write32p(PERI_TRACKER_BASE + BUS_DBG_TIMER_CON0, val);
	write32p(PERI_TRACKER_BASE + BUS_DBG_TIMER_CON1, val);

	/* Enable infra/peri tracer because tracker and tracer share the same enable bit. */
	write32p(BUS_TRACE_MONITOR_BASE + BUS_TRACE_CON_1, 1 << BUS_TRACE_EN);
	write32p(BUS_TRACE_MONITOR_BASE + BUS_TRACE_CON_2, 1 << BUS_TRACE_EN);

	/*
	 * Enable infra/peri tracker.
	 * bit[0] - BUS_DBG_EN
	 * bit[1] - TIMEOUT_EN
	 * bit[2] - SLV_ERR_EN
	 * bit[13] - HALT_ON_TIMEOUT_EN
	 * bit[14] - BUS_OT_WEN_CTRL
	 */
	val = BIT(0) | BIT(1) | BIT(2) | BIT(13) | BIT(14);
	write32p(BUS_TRACE_MONITOR_BASE + BUS_TRACE_CON_AO_1, val);
	write32p(BUS_TRACE_MONITOR_BASE + BUS_TRACE_CON_AO_2, val);
}
