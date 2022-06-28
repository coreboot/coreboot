/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef SOC_INTEL_COMMON_CSE_H
#define SOC_INTEL_COMMON_CSE_H

#include <types.h>
#include <vb2_api.h>

/* MKHI Command groups */
#define MKHI_GROUP_ID_CBM	0x0
#define MKHI_GROUP_ID_HMRFPO	0x5
#define MKHI_GROUP_ID_GEN	0xff
#define MKHI_GROUP_ID_BUP_COMMON	0xf0
#define MKHI_GROUP_ID_FWCAPS	0x3

/* Global Reset Command ID */
#define MKHI_CBM_GLOBAL_RESET_REQ	0xb

/* Set State Command ID */
#define MKHI_SET_ME_DISABLE	0x3
#define MKHI_SET_ME_ENABLE	0x3

/* Origin of Global Reset command */
#define GR_ORIGIN_BIOS_POST	0x2

/* HMRFPO Command Ids */
#define MKHI_HMRFPO_ENABLE	0x1
#define MKHI_HMRFPO_GET_STATUS	0x3

/* Get Firmware Version Command Id */
#define MKHI_GEN_GET_FW_VERSION	0x2

/* MEI bus disable command. Must be sent to MEI client endpoint, not MKHI */
#define MEI_BUS_DISABLE_COMMAND	0xc

/* Set End-of-POST in CSE */
#define MKHI_END_OF_POST	0xc

/* Boot partition info and set boot partition info command ids */
#define MKHI_BUP_COMMON_GET_BOOT_PARTITION_INFO	0x1c
#define MKHI_BUP_COMMON_SET_BOOT_PARTITION_INFO	0x1d
#define MKHI_BUP_COMMON_DATA_CLEAR		0x20

/* Get boot performance command id */
#define MKHI_BUP_COMMON_GET_BOOT_PERF_DATA	0x8

/* ME Current Working States */
#define ME_HFS1_CWS_NORMAL	0x5

/* ME Current Operation Modes */
#define ME_HFS1_COM_NORMAL	0x0
#define ME_HFS1_COM_SOFT_TEMP_DISABLE	0x3
#define ME_HFS1_COM_SECOVER_MEI_MSG	0x5

/* ME Disable Rule */
#define ME_DISABLE_RULE_ID	6
#define ME_DISABLE_RULE_LENGTH	4
#define ME_DISABLE_COMMAND	0
#define ME_DISABLE_ATTEMPTS	3

/* ME Firmware SKU Types */
#define ME_HFS3_FW_SKU_CONSUMER	0x2
#define ME_HFS3_FW_SKU_CORPORATE	0x3
#define ME_HFS3_FW_SKU_LITE	0x5

/* Number of cse boot performance data */
#define NUM_CSE_BOOT_PERF_DATA	64

/* HFSTS register offsets in PCI config space */
enum {
	PCI_ME_HFSTS1 = 0x40,
	PCI_ME_HFSTS2 = 0x48,
	PCI_ME_HFSTS3 = 0x60,
	PCI_ME_HFSTS4 = 0x64,
	PCI_ME_HFSTS5 = 0x68,
	PCI_ME_HFSTS6 = 0x6C,
};

/* MKHI Message Header */
struct mkhi_hdr {
	uint8_t group_id;
	uint8_t command:7;
	uint8_t is_resp:1;
	uint8_t rsvd;
	uint8_t result;
} __packed;

/* CSE FW Version */
struct fw_version {
	uint16_t major;
	uint16_t minor;
	uint16_t hotfix;
	uint16_t build;
} __packed;

/* ME FW Version */
struct me_version {
	uint16_t minor;
	uint16_t major;
	uint16_t build;
	uint16_t hotfix;
} __packed;

/* ME FW Version response */
struct me_fw_ver_resp {
	struct mkhi_hdr hdr;
	struct me_version code;
	struct me_version rec;
	struct me_version fitc;
} __packed;

/* CSE RX and TX error status */
enum cse_tx_rx_status {
	/*
	 * Transmission of HECI message is success or
	 * Reception of HECI message is success.
	 */
	CSE_TX_RX_SUCCESS = 0,

	 /* Timeout to send a message to CSE */
	CSE_TX_ERR_TIMEOUT = 1,

	/* Timeout to receive the response message from CSE */
	CSE_RX_ERR_TIMEOUT = 2,

	/*
	 * Response length doesn't match with expected
	 * response message length
	 */
	CSE_RX_ERR_RESP_LEN_MISMATCH = 3,

	/* CSE is not ready during TX flow */
	CSE_TX_ERR_CSE_NOT_READY = 4,

	/* CSE is not ready during RX flow */
	CSE_RX_ERR_CSE_NOT_READY = 5,

	/* Invalid input arguments provided for TX API */
	CSE_TX_ERR_INPUT = 6,

	/* Invalid input arguments provided for RX API */
	CSE_RX_ERR_INPUT = 7,
};

/* CSE recovery sub-error codes */
enum csme_failure_reason {
	/* No error */
	CSE_NO_ERROR = 0,

	/* Unspecified error */
	CSE_ERROR_UNSPECIFIED = 1,

	/* CSE fails to boot from RW */
	CSE_LITE_SKU_RW_JUMP_ERROR = 2,

	/* CSE RW boot partition access error */
	CSE_LITE_SKU_RW_ACCESS_ERROR = 3,

	/* Fails to set next boot partition as RW */
	CSE_LITE_SKU_RW_SWITCH_ERROR = 4,

	/* CSE firmware update failure */
	CSE_LITE_SKU_FW_UPDATE_ERROR = 5,

	/* Fails to communicate with CSE */
	CSE_COMMUNICATION_ERROR = 6,

	/* Fails to wipe CSE runtime data */
	CSE_LITE_SKU_DATA_WIPE_ERROR = 7,

	/* CSE RW is not found */
	CSE_LITE_SKU_RW_BLOB_NOT_FOUND = 8,

	/* CSE CBFS RW SHA-256 mismatch with the provided SHA */
	CSE_LITE_SKU_RW_BLOB_SHA256_MISMATCH = 9,

	/* CSE CBFS RW metadata is not found */
	CSE_LITE_SKU_RW_METADATA_NOT_FOUND = 10,

	/* CSE CBFS RW blob layout is not correct */
	CSE_LITE_SKU_LAYOUT_MISMATCH_ERROR = 11,

	/* Error sending EOP to CSE */
	CSE_EOP_FAIL = 12,

	/* CSE Sub-partition update fail */
	CSE_LITE_SKU_SUB_PART_UPDATE_FAIL = 13,

	/* CSE sub-partition access failure */
	CSE_LITE_SKU_SUB_PART_ACCESS_ERR = 14,

	/* CSE CBFS sub-partition access error */
	CSE_LITE_SKU_SUB_PART_BLOB_ACCESS_ERR = 15,

	/* CSE Lite sub-partition update is not required */
	CSE_LITE_SKU_SUB_PART_UPDATE_NOT_REQ = 16,

	/* CSE Lite sub-partition layout mismatch error */
	CSE_LITE_SKU_SUB_PART_LAYOUT_MISMATCH_ERROR = 17,

	/* CSE Lite sub-partition update success */
	CSE_LITE_SKU_PART_UPDATE_SUCCESS = 18,
};

/* Boot performance data */
enum cse_boot_perf_data {
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
	PERF_DATA_CSME_CPU_BOOT_CONFIG_DONW = 16,

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

/* CSE boot performance data */
struct cse_boot_perf_rsp {
	struct mkhi_hdr hdr;

	/* Data version */
	uint32_t version;

	/* Data length in DWORDs, represents number of valid elements in timestamp array */
	uint32_t num_valid_timestamps;

	/* Boot performance data */
	uint32_t timestamp[NUM_CSE_BOOT_PERF_DATA];
} __packed;

/*
 * Initialize the CSE device.
 *
 * Set up CSE device for use in early boot environment with temp bar.
 */
void cse_init(uintptr_t bar);

/* Initialize the HECI devices. */
void heci_init(void);

/*
 * Send message from BIOS_HOST_ADDR to cse_addr.
 * Sends snd_msg of size snd_sz, and reads message into buffer pointed by
 * rcv_msg of size rcv_sz
 * Returns CSE_TX_RX_SUCCESS on success and other enum values on failure scenarios.
 */
enum cse_tx_rx_status heci_send_receive(const void *snd_msg, size_t snd_sz, void *rcv_msg,
					size_t *rcv_sz,	uint8_t cse_addr);

/*
 * Attempt device reset. This is useful and perhaps only thing left to do when
 * CPU and CSE are out of sync or CSE fails to respond.
 * Returns 0 on failure and 1 on success.
 */
int heci_reset(void);
/* Disable HECI1 using Sideband interface communication */
void heci1_disable(void);

/* Reads config value from a specified offset in the CSE PCI Config space. */
uint32_t me_read_config32(int offset);

/*
 * Check if the CSE device as per function argument `devfn` is enabled in device tree
 * and also visible on the PCI bus.
 */
bool is_cse_devfn_visible(unsigned int devfn);

/*
 * Check if the CSE device is enabled in device tree. Also check if the device
 * is visible on the PCI bus by reading config space.
 * Return true if device present and config space enabled, else return false.
 */
bool is_cse_enabled(void);

/* Makes the host ready to communicate with CSE */
void cse_set_host_ready(void);

/*
 * Polls for ME state 'HECI_OP_MODE_SEC_OVERRIDE' for 15 seconds.
 * Returns 0 on failure and 1 on success.
 */
uint8_t cse_wait_sec_override_mode(void);

enum rst_req_type {
	GLOBAL_RESET = 1,
	CSE_RESET_ONLY = 3,
};

/*
 * Sends GLOBAL_RESET_REQ cmd to CSE with reset type GLOBAL_RESET.
 * Returns 0 on failure and 1 on success.
 */
int cse_request_global_reset(void);
/*
 * Sends HMRFPO_ENABLE command.
 * HMRFPO - Host ME Region Flash Protection Override.
 * For CSE Lite SKU, procedure to place CSE in HMRFPO (SECOVER_MEI_MSG) mode:
 *	1. Ensure CSE boots from RO(BP1).
 *		- Set CSE's next boot partition to RO
 *		- Issue GLOBAL_RESET command to reset the system
 *	2. Send HMRFPO_ENABLE command to CSE. Further, no reset is required.
 *
 * The HMRFPO mode prevents CSE to execute SPI I/O cycles to CSE region, and unlocks
 * the CSE region to perform updates to it.
 * This command is only valid before EOP.
 *
 * Returns 0 on failure to send HECI command and to enable HMRFPO mode, and 1 on success.
 *
 */
int cse_hmrfpo_enable(void);

/*
 * Send HMRFPO_GET_STATUS command.
 * returns -1 on failure and 0 (DISABLED)/ 1 (LOCKED)/ 2 (ENABLED)
 * on success.
 */
int cse_hmrfpo_get_status(void);

/* Fixed Address MEI Header's Host Address field value */
#define BIOS_HOST_ADDR	0x00

/* Fixed Address MEI Header's ME Address field value */
#define HECI_MKHI_ADDR	0x07

/* Fixed Address MEI Header's ME Address for MEI bus messages */
#define HECI_MEI_ADDR	0x00

/* HMRFPO Status types */
/* Host can't access ME region */
#define MKHI_HMRFPO_DISABLED	0

/*
 * ME Firmware locked down HMRFPO Feature.
 * Host can't access ME region.
 */
#define MKHI_HMRFPO_LOCKED	1

/* Host can access ME region */
#define MKHI_HMRFPO_ENABLED	2

/*
 * Queries and logs ME firmware version
 */
void print_me_fw_version(void *unused);

/*
 * Queries and gets ME firmware version
 */
enum cb_err get_me_fw_version(struct me_fw_ver_resp *resp);

/*
 * Checks current working operation state is normal or not.
 * Returns true if CSE's current working state is normal, otherwise false.
 */
bool cse_is_hfs1_cws_normal(void);

/*
 * Checks CSE's current operation mode is normal or not.
 * Returns true if CSE's current operation mode is normal, otherwise false.
 */
bool cse_is_hfs1_com_normal(void);

/*
 * Checks CSE's current operation mode is SECOVER_MEI_MSG or not.
 * Returns true if CSE's current operation mode is SECOVER_MEI_MSG, otherwise false.
 */
bool cse_is_hfs1_com_secover_mei_msg(void);

/*
 * Checks CSE's current operation mode is Soft Disable Mode or not.
 * Returns true if CSE's current operation mode is Soft Disable Mode, otherwise false.
 */
bool cse_is_hfs1_com_soft_temp_disable(void);

/*
 * Checks CSE's spi protection mode is protected or unprotected.
 * Returns true if CSE's spi protection mode is protected, otherwise false.
 */
bool cse_is_hfs1_spi_protected(void);

/*
 * Checks CSE's Firmware SKU is Lite or not.
 * Returns true if CSE's Firmware SKU is Lite, otherwise false
 */
bool cse_is_hfs3_fw_sku_lite(void);

/*
 * Polls for CSE's current operation mode 'Soft Temp Disable'.
 * Returns 0 on failure and 1 on success.
 */
uint8_t cse_wait_com_soft_temp_disable(void);

/*
 * The CSE Lite SKU supports notion of RO and RW boot partitions. The function will set
 * CSE's boot partition as per ChromeOS boot modes. In normal mode, the function allows CSE to
 * boot from RW and triggers recovery mode if CSE fails to jump to RW.
 * In software triggered recovery mode, the function allows CSE to boot from whatever is
 * currently selected partition.
 */
void cse_fw_sync(void);

/* Perform a board-specific reset sequence for CSE RO<->RW jump */
void cse_board_reset(void);

/* Trigger vboot recovery mode on a CSE error */
void cse_trigger_vboot_recovery(enum csme_failure_reason reason);

enum cse_device_state {
	DEV_IDLE,
	DEV_ACTIVE,
};

/* Function to get the current CSE device state as per `cse_device_state` */
enum cse_device_state get_cse_device_state(unsigned int devfn);

/* Function that put the CSE into desired state based on `requested_state` */
bool set_cse_device_state(unsigned int devfn, enum cse_device_state requested_state);

/*
 * Check if cse sub-parition update is required or not.
 * Returns true if cse sub-parition update is required otherwise false.
 */
bool skip_cse_sub_part_update(void);

/*
 * This command retrieves a set of boot performance timestamps CSME collected during
 * the last platform boot flow.
 */
bool cse_get_boot_performance_data(struct cse_boot_perf_rsp *boot_perf);

/* Function to make cse disable using PMC IPC */
bool cse_disable_mei_devices(void);

/* Set CSE device state to D0I3 */
void cse_set_to_d0i3(void);

/* Function sets D0I3 for all HECI devices */
void heci_set_to_d0i3(void);

/* Function performs the global reset lock */
void cse_control_global_reset_lock(void);

/* Send End of Post (EOP) command to CSE device */
void cse_send_end_of_post(void);

/*
 * SoC override API to make heci1 disable using PCR.
 *
 * Allow SoC to implement heci1 disable override due to PSF registers being
 * different across SoC generation.
 */
void soc_disable_heci1_using_pcr(void);

/*
 * Get all the timestamps CSE collected using cse_get_boot_performance_data() and
 * insert them into the CBMEM timestamp table.
 */
void cse_get_telemetry_data(void);

#endif // SOC_INTEL_COMMON_CSE_H
