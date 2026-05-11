/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef SOC_INTEL_COMMON_CSE_TELEMETRY_V5_H
#define SOC_INTEL_COMMON_CSE_TELEMETRY_V5_H

/* Number of CSE boot performance data entries */
#define NUM_CSE_BOOT_PERF_DATA	96

enum cse_boot_perf_data_v5 {
	/* CSME RBE set "Boot Stall Done" indication to PMC */
	PERF_DATA_CSME_RBE_BOOT_STALL_DONE_TO_PMC = 6,

	/* CSME got ICC_CONFIG_START message from PMC */
	PERF_DATA_CSME_GOT_ICC_CFG_START_MSG_FROM_PMC = 16,

	/* CSME set "Host Boot Prep Done" indication to PMC */
	PERF_DATA_CSME_HOST_BOOT_PREP_DONE = 17,

	/* ESE completed DMU binaries loading */
	PERF_DATA_ESE_LOAD_DMU_COMPLETED = 31,

	/* PMC sent "Core Reset Done Ack - Sent" message to CSME */
	PERF_DATA_PMC_SENT_CRDA = 35,

	/* ESE completed AUnit binaries loading */
	PERF_DATA_ESE_LOAD_AUNIT_COMPLETED = 37,

	/* Timestamp when CSME responded to BupGetEarlyBootData message itself */
	PERF_DATA_CSME_GET_PERF_RESPONSE = 95,
};

#endif /* SOC_INTEL_COMMON_CSE_TELEMETRY_V5_H */
