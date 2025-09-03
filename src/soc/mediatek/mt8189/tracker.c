/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#include <console/console.h>
#include <device/mmio.h>
#include <soc/addressmap.h>
#include <soc/tracker.h>

struct tracker tracker_data[TRACKER_NUM] = {
	[TRACKER_SYSTRACKER] = {
		.base_addr = AP_TRACKER_BASE,
		.timeout = BUS_DBG_CON_TIMEOUT,
		.entry = SYS_TRACK_ENTRY,
		.offsets = tracker_v3_offsets,
		.offsets_size = TRACKER_V3_OFFSETS_SIZE,
		.str = "systracker",
	},
	[TRACKER_INFRATRACKER] = {
		.base_addr = INFRA_TRACKER_BASE,
		.timeout = BUS_DBG_CON_TIMEOUT,
		.entry = INFRA_ENTRY_NUM,
		.offsets = tracker_v3_offsets,
		.offsets_size = TRACKER_V3_OFFSETS_SIZE,
		.str = "infra_tracker",
	},
};

void tracker_setup(void)
{
	u32 val;
	/*
	 * Set ap/infra tracker timeout.
	 * timeout = clock_in_mhz / 15 * timeout_in_us
	 *
	 * timeout: 200ms
	 * ap tracker clock: 26MHz
	 * infra tracker clock: 26MHz
	 */
	val = 26 / 15 * 200000;

	write32p(AP_TRACKER_BASE + BUS_DBG_TIMER_CON0_26M, val);
	write32p(INFRA_TRACKER_BASE + BUS_DBG_TIMER_CON0_26M, val);

	/*
	 * Enable infra/peri tracker.
	 * bit[0] - BUS_DBG_EN
	 * bit[1] - TIMEOUT_EN
	 * bit[2] - SLV_ERR_EN
	 * bit[9] - TIMER_CLOCK_SEL
	 * bit[13] - HALT_ON_TIMEOUT_EN
	 * bit[14] - BUS_OT_WEN_CTRL
	 */
	val = BIT(0) | BIT(1) | BIT(2) | BIT(9) | BIT(13) | BIT(14);
	write32p(BUS_TRACE_MONITOR_BASE + BUS_TRACE_CON_AO_1, val);
	write32p(BUS_TRACE_MONITOR_BASE + BUS_TRACE_CON_AO_2, val);
}
