/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <console/console.h>
#include <intelblocks/cse.h>
#include <timestamp.h>

#define MSEC_TO_USEC(x) (x * 1000)

enum cb_err cse_get_boot_performance_data(struct cse_boot_perf_rsp *boot_perf_rsp)
{
	struct cse_boot_perf_req {
		struct mkhi_hdr hdr;
		uint32_t reserved;
	} __packed;

	struct cse_boot_perf_req req = {
		.hdr.group_id = MKHI_GROUP_ID_BUP_COMMON,
		.hdr.command = MKHI_BUP_COMMON_GET_BOOT_PERF_DATA,
		.reserved = 0,
	};

	size_t resp_size = sizeof(struct cse_boot_perf_rsp);

	if (heci_send_receive(&req, sizeof(req), boot_perf_rsp, &resp_size,
									HECI_MKHI_ADDR)) {
		printk(BIOS_ERR, "cse: Could not get boot performance data\n");
		return CB_ERR;
	}

	if (boot_perf_rsp->hdr.result) {
		printk(BIOS_ERR, "cse: Get boot performance data resp failed: %d\n",
				boot_perf_rsp->hdr.result);
		return CB_ERR;
	}

	return CB_SUCCESS;
}

static void process_cse_telemetry_data(void)
{
	struct cse_boot_perf_rsp cse_perf_data;
	s64 ts[NUM_CSE_BOOT_PERF_DATA] = {0};
	s64 current_time;
	int zero_point_idx = 0;

	/*
	 * 1. Each TS holds the time elapsed between the "Zero-Point" till the TS itself
	 *    happened.
	 * 2. In case CSME did not hit some of the TS in the latest boot flow that value of
	 *    these TS will be 0x00000000.
	 * 3. In case of error, TS value will be set to 0xFFFFFFFF.
	 * 4. All other TS values will be relative to the zero point. The API caller should
	 *    normalize the TS values to the zero-point value.
	 */
	if (cse_get_boot_performance_data(&cse_perf_data) != CB_SUCCESS)
		return;

	current_time = timestamp_get();

	for (unsigned int i = 0; i < NUM_CSE_BOOT_PERF_DATA; i++) {
		if (cse_perf_data.timestamp[i] == 0xffffffff) {
			printk(BIOS_ERR, "%s: CSME timestamps invalid\n", __func__);
			return;
		}

		ts[i] = (s64)MSEC_TO_USEC(cse_perf_data.timestamp[i]) *
							timestamp_tick_freq_mhz();
	}

	/* Find zero-point */
	for (unsigned int i = 0; i < NUM_CSE_BOOT_PERF_DATA; i++) {
		if (cse_perf_data.timestamp[i] != 0) {
			zero_point_idx = i;
			break;
		}
	}

	/* Normalize TS values to zero-point */
	for (unsigned int i = zero_point_idx + 1; i < NUM_CSE_BOOT_PERF_DATA; i++) {
		if (ts[i] && ts[i] < ts[zero_point_idx]) {
			printk(BIOS_ERR, "%s: CSME timestamps invalid,"
					" wraparound detected\n", __func__);
			return;
		}

		if (ts[i])
			ts[i] -= ts[zero_point_idx];
	}

	/* Inject CSME timestamps into the coreboot timestamp table */
	soc_cbmem_inject_telemetry_data(ts, current_time);
}

void cse_get_telemetry_data(void)
{
	/* If CSE is already hidden then accessing CSE registers should be avoided */
	if (!is_cse_enabled()) {
		printk(BIOS_DEBUG, "CSE is disabled, not sending `Get Boot Perf` message\n");
		return;
	}

	process_cse_telemetry_data();
}
