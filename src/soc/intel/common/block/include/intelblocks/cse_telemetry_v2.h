/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef SOC_INTEL_COMMON_CSE_TELEMETRY_V2_H
#define SOC_INTEL_COMMON_CSE_TELEMETRY_V2_H

enum cse_boot_perf_data_v2 {
	/* CSME ROM start execution  */
	PERF_DATA_CSME_ROM_START = 0,

	/* EC Boot Load Done (CSME ROM starts main execution) */
	PERF_DATA_EC_BOOT_LOAD_DONE = 1,

	/* CSME ROM completed execution / CSME RBE started */
	PERF_DATA_CSME_ROM_COMPLETED = 2,

	/* CSME got ESE Init Done indication from ESE */
	PERF_DATA_CSME_GOT_ESE_INIT_DONE = 3,

	/* CSME RBE started SOC.PMC patch and payloads read from SPI flash */
	PERF_DATA_CSME_RBE_SOC_PMC_PATCH_LOADING_START = 4,

	/* CSME RBE completed SOC.PMC patch and payloads read from SPI flash */
	PERF_DATA_CSME_RBE_SOC_PMC_PATCH_LOADING_COMPLETED = 5,

	/* CSME RBE started pushing SOC.PMC patch and payloads */
	PERF_DATA_CSME_RBE_SOC_PMC_PATCH_PUSH_START = 6,

	/* CSME RBE completed pushing SOC.PMC patch and payloads */
	PERF_DATA_CSME_RBE_SOC_PMC_PATCH_PUSH_COMPLETED = 7,

	/* CSME RBE set "Boot Stall Done" indication to SOC.PMC */
	PERF_DATA_CSME_RBE_BOOT_STALL_DONE_TO_PMC = 8,

	/* CSME RBE Started load IOE.PMC patch and payloads */
	PERF_DATA_CSME_RBE_IOE_PMC_PATCH_LOADING_START = 9,

	/* CSME RBE completed to load IOE.PMC patch and payloads */
	PERF_DATA_CSME_RBE_IOE_PMC_PATCH_LOADING_COMPLETED = 10,

	/* CSME RBE jumped to CSME Kernel */
	PERF_DATA_CSME_RBE_KERNEL_JUMP = 11,

	/* CSME BUP start running  */
	PERF_DATA_CSME_BUP_START = 12,

	/* CSME established IPC channel communication with ESE */
	PERF_DATA_CSME_IPC_CHANNEL_FOR_ESE_UP = 13,

	/* ESE FW initialization completed */
	PERF_DATA_ESE_FW_INIT_DONE = 14,

	/* PMC set PPS */
	PERF_DATA_PMC_SET_PPS = 15,

	/* CSME got ICC_CFG_START message from PMC */
	PERF_DATA_CSME_GOT_ICC_CFG_START_MSG_FROM_PMC = 16,

	/* PMC got PCH_PWROK */
	PERF_DATA_PMC_GOT_PCH_PWROK = 17,

	/* CSME set "Host Boot Prep Done" indication to PMC  */
	PERF_DATA_CSME_HOST_BOOT_PREP_DONE = 18,

	/* CSME starts PHYs loading - phase 1 - USB, PCIe */
	PERF_DATA_CSME_PHASE1_PHY_LOADING_START = 19,

	/* CSME completed phase 1 PHYs loading - USB, PCIe */
	PERF_DATA_CSME_PHASE1_PHY_LOADING_COMPLETED = 20,

	/* PMC indicated CSME that xxPWRGOOD was asserted */
	PERF_DATA_PMC_PWRGOOD_ASSERTED = 21,

	/* PMC indicated CSME that SYS_PWROK was asserted */
	PERF_DATA_PMC_SYS_PWROK_ASSERTED = 22,

	/* ESE sent IPC message to CSME indicating PUnit load completed */
	PERF_DATA_ESE_PUNIT_LOAD_COMPLETED = 23,

	/* PMC indicates CSME that xxPLTRST was de-asserted */
	PERF_DATA_PMC_PLTRST_DEASSERTED = 24,

	/* PMC sent "CPU_BOOT_CONFIG" start message to CSME */
	PERF_DATA_PMC_CPU_BOOT_CONFIG_START = 25,

	/* CSME starts PHYs loading - phase 2 - UFS */
	PERF_DATA_CSME_PHASE2_PHY_LOADING_START = 26,

	/* CSME completed phase 2 PHYs loading - UFS */
	PERF_DATA_CSME_PHASE2_PHY_LOADING_COMPLETED = 27,

	/* CSME sent "CPU_BOOT_CONFIG" done message to PMC */
	PERF_DATA_CSME_CPU_BOOT_CONFIG_DONE = 28,

	/* PMC sent "Core Reset Done Ack - Sent" message to CSME */
	PERF_DATA_PMC_SENT_CRDA = 29,

	/* ESE sent IPC message to CSME indicating DMU load completed */
	PERF_DATA_ESE_DMU_LOAD_COMPLETED = 30,

	/* ACM Active indication - ACM started its execution */
	PERF_DATA_ACM_START = 31,

	/* ACM Done indication - ACM completed execution */
	PERF_DATA_ACM_DONE = 32,

	/* BIOS sent "Get_BIOS_Seed" message to CSME */
	PERF_DATA_BIOS_SEED_MSG_RECEIVED = 33,

	/* CSME responded to "Get_BIOS_Seed" message */
	PERF_DATA_BIOS_SEED_MSG_REPLIED = 34,

	/* BIOS sent DRAM Init Done message */
	PERF_DATA_BIOS_DRAM_INIT_DONE = 35,

	/* CSME sent DRAM Init Done message back to BIOS */
	PERF_DATA_CSME_DRAM_INIT_DONE = 36,

	/* CSME started loading TCSS components (IOM, NPHY, TBT) */
	PERF_DATA_CSME_LOAD_TCSS_START = 37,

	/* CSME start loading ACE */
	PERF_DATA_CSME_LOAD_ACE_START = 38,

	/* CSME started loading eDP PHY */
	PERF_DATA_CSME_LOAD_EDP_PHY_START = 39,

	/* CSME completed loading eDP PHY */
	PERF_DATA_CSME_LOAD_EDP_PHY_COMPLETED = 40,

	/* BIOS sent "Core BIOS Done" message to CSME */
	PERF_DATA_BIOS_BIOS_CORE_DONE = 41,

	/* CSME sent "Core BIOS Done" ack message back to BIOS */
	PERF_DATA_CSME_BIOS_CORE_DONE = 42,

	/* BIOS sent "End Of Post" message to CSME */
	PERF_DATA_BIOS_END_OF_POST = 43,

	/* CSME sent "End Of Post" ack message back to BIOS */
	PERF_DATA_CSME_END_OF_POST = 44,

	/* CSME started loading ISH Bringup module */
	PERF_DATA_PERF_DATA_CSME_LOAD_ISH_BRINGUP_START = 45,

	/* CSME completed loading ISH Bringup module */
	PERF_DATA_CSME_LOAD_ISH_BRINGUP_DONE = 46,

	/* CSME started loading ISH Main module */
	PERF_DATA_CSME_LOAD_ISH_MAIN_START = 47,

	/* CSME completed loading Main module */
	PERF_DATA_CSME_LOAD_ISH_MAIN_DONE = 48,

	/* CSME reached Transition Complete */
	PERF_DATA_CSME_TRANSITION_COMPLETE = 49,

	/* 50 - 62 Reserved */

	/* Timestamp when CSME responded to BupGetBootData message itself */
	PERF_DATA_CSME_GET_PERF_RESPONSE = 63,
};

#endif // SOC_INTEL_COMMON_CSE_TELEMETRY_V2_H
