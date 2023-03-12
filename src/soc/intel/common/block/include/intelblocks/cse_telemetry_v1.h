/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef SOC_INTEL_COMMON_CSE_TELEMETRY_V1_H
#define SOC_INTEL_COMMON_CSE_TELEMETRY_V1_H

enum cse_boot_perf_data_v1 {
	/* CSME ROM start execution  */
	PERF_DATA_CSME_ROM_START = 0,

	/* EC Boot Load Done (CSME ROM starts main execution) */
	PERF_DATA_EC_BOOT_LOAD_DONE = 1,

	/* CSME ROM completed execution / CSME RBE started */
	PERF_DATA_CSME_ROM_COMPLETED = 2,

	/* CSME got ESE Init Done indication from ESE */
	PERF_DATA_CSME_GOT_ESE_INIT_DONE = 3,

	/* CSME RBE start PMC patch/es loading */
	PERF_DATA_CSME_RBE_PMC_PATCH_LOADING_START = 4,

	/* CSME RBE completed PMC patch/es loading */
	PERF_DATA_CSME_RBE_PMC_PATCH_LOADING_COMPLETED = 5,

	/* CSME RBE set "Boot Stall Done" indication to PMC */
	PERF_DATA_CSME_RBE_BOOT_STALL_DONE_TO_PMC = 6,

	/* CSME start poll for PMC PPS register */
	PERF_DATA_CSME_POLL_FOR_PMC_PPS_START = 7,

	/* PMC set PPS */
	PERF_DATA_PMC_SET_PPS = 8,

	/* CSME BUP start running  */
	PERF_DATA_CSME_BUP_START = 9,

	/* CSME set "Host Boot Prep Done" indication to PMC  */
	PERF_DATA_CSME_HOST_BOOT_PREP_DONE = 10,

	/* CSME starts PHYs loading */
	PERF_DATA_CSME_PHY_LOADING_START = 11,

	/* CSME completed PHYs loading */
	PERF_DATA_CSME_PHY_LOADING_COMPLETED = 12,

	/* PMC indicated CSME that xxPWRGOOD was asserted */
	PERF_DATA_PMC_PWRGOOD_ASSERTED = 13,

	/* PMC indicated CSME that SYS_PWROK was asserted */
	PERF_DATA_PMC_SYS_PWROK_ASSERTED = 14,

	/* PMC sent "CPU_BOOT_CONFIG" start message to CSME */
	PERF_DATA_PMC_CPU_BOOT_CONFIG_START = 15,

	/* CSME sent "CPU_BOOT_CONFIG" done message to PMC */
	PERF_DATA_CSME_CPU_BOOT_CONFIG_DONE = 16,

	/* PMC indicated CSME that xxPLTRST was de-asserted */
	PERF_DATA_PMC_PLTRST_DEASSERTED = 17,

	/* PMC indicated CSME that TCO_S0 was asserted */
	PERF_DATA_PMC_TC0_S0_ASSERTED = 18,

	/* PMC sent "Core Reset Done Ack - Sent" message to CSME */
	PERF_DATA_PMC_SENT_CRDA = 19,

	/* ACM Active indication - ACM started its execution */
	PERF_DATA_ACM_START = 20,

	/* ACM Done indication - ACM completed execution */
	PERF_DATA_ACM_DONE = 21,

	/* BIOS sent DRAM Init Done message */
	PERF_DATA_BIOS_DRAM_INIT_DONE = 22,

	/* CSME sent DRAM Init Done message back to BIOS */
	PERF_DATA_CSME_DRAM_INIT_DONE = 23,

	/* CSME completed loading TCSS */
	PERF_DATA_CSME_LOAD_TCSS_COMPLETED = 24,

	/* CSME started loading ISH Bringup module */
	PERF_DATA_PERF_DATA_CSME_LOAD_ISH_BRINGUP_START = 25,

	/* CSME completed loading ISH Bringup module */
	PERF_DATA_CSME_LOAD_ISH_BRINGUP_DONE = 26,

	/* CSME started loading ISH Main module */
	PERF_DATA_CSME_LOAD_ISH_MAIN_START = 27,

	/* CSME completed loading Main module */
	PERF_DATA_CSME_LOAD_ISH_MAIN_DONE = 28,

	/* BIOS sent "End Of Post" message to CSME */
	PERF_DATA_BIOS_END_OF_POST = 29,

	/* CSME sent "End Of Post" ack message back to BIOS */
	PERF_DATA_CSME_END_OF_POST = 30,

	/* BIOS sent "Core BIOS Done" message to CSME */
	PERF_DATA_BIOS_BIOS_CORE_DONE = 31,

	/* CSME sent "Core BIOS Done" ack message back to BIOS */
	PERF_DATA_CSME_BIOS_CORE_DONE = 32,

	/* CSME reached Firmware Init Done */
	PERF_DATA_CSME_GW_INIT_DONE = 33,

	/* 34 - 62 Reserved */

	/* Timestamp when CSME responded to BupGetBootData message itself */
	PERF_DATA_CSME_GET_PERF_RESPONSE = 63,
};

#endif // SOC_INTEL_COMMON_CSE_TELEMETRY_V1_H
