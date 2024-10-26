/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef SOC_INTEL_COMMON_CSE_TELEMETRY_V3_H
#define SOC_INTEL_COMMON_CSE_TELEMETRY_V3_H

enum cse_boot_perf_data_v3 {
	/* CSME ROM start execution  */
	PERF_DATA_CSME_ROM_START = 0,

	/* 1 - 5 Reserved */

	/* CSME RBE set "Boot Stall Done" indication to PMC */
	PERF_DATA_CSME_RBE_BOOT_STALL_DONE_TO_PMC = 6,

	/* 7 - 14 Reserved */

	/* CSME got ICC_CFG_START message from PMC */
	PERF_DATA_CSME_GOT_ICC_CFG_START_MSG_FROM_PMC = 15,

	/* 15 - 16 Reserved */

	/* CSME set "Host Boot Prep Done" indication to PMC */
	PERF_DATA_CSME_HOST_BOOT_PREP_DONE = 17,

	/* 18 - 32 Reserved */

	/* PMC sent "Core Reset Done Ack - Sent" message to CSME */
	PERF_DATA_PMC_SENT_CRDA = 33,

	/* 34 - 35 Reserved */

	/* ESE completed AUnit loading */
	PERF_DATA_ESE_LOAD_AUNIT_COMPLETED = 36,

	/* 37 - 62 Reserved */

	/* Timestamp when CSME responded to BupGetBootData message itself */
	PERF_DATA_CSME_GET_PERF_RESPONSE = 63,
};

#endif /* SOC_INTEL_COMMON_CSE_TELEMETRY_V3_H */
