/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <console/console.h>
#include <intelblocks/cse.h>
#include <timestamp.h>

void soc_cbmem_inject_telemetry_data(s64 *ts, s64 current_time)
{
	s64 start_stamp;

	if (!ts) {
		printk(BIOS_ERR, "%s: Failed to insert CSME timestamps\n", __func__);
		return;
	}

	start_stamp = current_time - ts[PERF_DATA_CSME_GET_PERF_RESPONSE];

	timestamp_add(TS_ME_ROM_START, start_stamp);
	timestamp_add(TS_ME_BOOT_STALL_END,
		start_stamp + ts[PERF_DATA_CSME_RBE_BOOT_STALL_DONE_TO_PMC]);
	timestamp_add(TS_ME_ICC_CONFIG_START,
		start_stamp + ts[PERF_DATA_CSME_GOT_ICC_CFG_START_MSG_FROM_PMC]);
	timestamp_add(TS_ME_HOST_BOOT_PREP_END,
		start_stamp + ts[PERF_DATA_CSME_HOST_BOOT_PREP_DONE]);
	timestamp_add(TS_ME_RECEIVED_CRDA_FROM_PMC,
		start_stamp + ts[PERF_DATA_PMC_SENT_CRDA]);
	timestamp_add(TS_ISSE_DMU_LOAD_END,
		start_stamp + ts[PERF_DATA_ISSE_DMU_LOAD_COMPLETED]);
}
