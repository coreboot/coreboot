/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef SOC_INTEL_COMMON_CSE_H
#define SOC_INTEL_COMMON_CSE_H

#include <intelblocks/cse_telemetry.h>
#include <types.h>
#include <vb2_api.h>

/* MKHI Command groups */
enum mkhi_group_id {
	MKHI_GROUP_ID_CBM	 = 0x0,
	MKHI_GROUP_ID_HMRFPO	 = 0x5,
	MKHI_GROUP_ID_GEN	 = 0xff,
	MKHI_GROUP_ID_BUP_COMMON = 0xf0,
	MKHI_GROUP_ID_FWCAPS	 = 0x3,
};

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

/* Firmware Feature Shipment Time State Override Command Id */
#define MKHI_GEN_FW_FEATURE_SHIPMENT_OVER	0x14
#define   ME_FW_FEATURE_PTT			BIT(29)

/* Get Firmware Feature State Command Id */
#define MKHI_FWCAPS_GET_FW_FEATURE_STATE	0x02
#define   ME_FEATURE_STATE_RULE_ID		0x20
#define   ME_FW_FEATURE_PSR			BIT(5)
/* MEI bus disable command. Must be sent to MEI client endpoint, not MKHI */
#define MEI_BUS_DISABLE_COMMAND	0xc

/* Set End-of-POST in CSE */
#define MKHI_END_OF_POST	0xc

/* Boot partition info and set boot partition info command ids */
#define MKHI_BUP_COMMON_GET_BOOT_PARTITION_INFO	0x1c
#define MKHI_BUP_COMMON_SET_BOOT_PARTITION_INFO	0x1d
#define MKHI_BUP_COMMON_DATA_CLEAR		0x20
#define GEN_GET_IMAGE_FW_VERSION	0x1c

/* Get boot performance command id */
#define MKHI_BUP_COMMON_GET_BOOT_PERF_DATA	0x8

/* ME Current Working States */
#define ME_HFS1_CWS_M3_NO_UMA	0x4
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
enum me_fw_sku {
	ME_HFS3_FW_SKU_CONSUMER	 = 0x2,
	ME_HFS3_FW_SKU_CORPORATE = 0x3,
	ME_HFS3_FW_SKU_LITE	 = 0x5,
};

/* Number of cse boot performance data */
#define NUM_CSE_BOOT_PERF_DATA	64

/* PSR_HECI_FW_DOWNGRADE_BACKUP Command */
#define PSR_HECI_FW_DOWNGRADE_BACKUP 0x3

/* HFSTS register offsets in PCI config space */
enum {
	PCI_ME_HFSTS1 = 0x40,
	PCI_ME_HFSTS2 = 0x48,
	PCI_ME_HFSTS3 = 0x60,
	PCI_ME_HFSTS4 = 0x64,
	PCI_ME_HFSTS5 = 0x68,
	PCI_ME_HFSTS6 = 0x6C,
};

/* CSE partition list */
enum fpt_partition_id {
	FPT_PARTITION_NAME_UNDEFINED = 0x0,
	FPT_PARTITION_NAME_ISHC = 0x43485349,
};

/* MKHI Message Header */
struct mkhi_hdr {
	uint8_t group_id;
	uint8_t command:7;
	uint8_t is_resp:1;
	uint8_t rsvd;
	uint8_t result;
} __packed;

/* PSR HECI message status */
enum psr_status {
	PSR_STATUS_SUCCESS,
	PSR_STATUS_FEATURE_NOT_SUPPORTED,
	PSR_STATUS_UPID_DISABLED,
	PSR_STATUS_ACTION_NOT_ALLOWED,
	PSR_STATUS_INVALID_INPUT_PARAMETER,
	PSR_STATUS_INTERNAL_ERROR,
	PSR_STATUS_NOT_ALLOWED_AFTER_EOP,
};

/* PSR HECI message header */
struct psr_heci_header {
	uint8_t command;
	uint8_t reserved;
	uint16_t length;
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

/* Module data from manifest */
struct flash_partition_data {
	enum fpt_partition_id partition_id;
	uint8_t reserved1[8];
	struct fw_version version;
	uint32_t vendor_id;
	uint32_t tcb_svn;
	uint32_t arb_svn;
	uint32_t vcn;
	uint32_t reserved2[13];
};

/* Response header for partition information request */
struct fw_version_resp {
	struct mkhi_hdr hdr;
	uint32_t module_count;
	struct flash_partition_data manifest_data;
};

/* ISHC version */
struct cse_fw_ish_version_info {
	struct fw_version prev_cse_fw_version;
	struct fw_version cur_ish_fw_version;
};

/* CSE and ISHC version */
struct cse_fw_partition_info {
	struct fw_version cur_cse_fw_version;
	struct cse_fw_ish_version_info ish_partition_info;
};

/* CSE sync flags */
enum cse_sync_flags {
	CSE_DOWNGRADE_REQUEST		= 1 << 0,
	CSE_ENFORCED_SYNC_REQUEST	= 1 << 1,
	CSE_ENFORCED_SYNC_PERFORMED	= 1 << 2,
};

/* CSE Specific Information */
struct cse_specific_info {
	struct cse_fw_partition_info cse_fwp_version;
	int8_t cse_sync_status;
	uint32_t crc;
};

/* PSR backup status */
enum psr_backup_state {
	PSR_BACKUP_DONE	= 0,
	PSR_BACKUP_PENDING = 1,
};

struct psr_backup_status {
	uint32_t signature;
	int8_t value;
	uint16_t checksum;
};

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
 * Send message msg of size len to host from host_addr to cse_addr.
 * Returns CSE_TX_RX_SUCCESS on success and other enum values on failure scenarios.
 * Also, in case of errors, heci_reset() is triggered.
 */
enum cse_tx_rx_status heci_send(const void *msg, size_t len, uint8_t host_addr,
				uint8_t client_addr);

/*
 * Receive message into buff not exceeding maxlen. Message is considered
 * successfully received if a 'complete' indication is read from ME side
 * and there was enough space in the buffer to fit that message. maxlen
 * is updated with size of message that was received.
 * Returns CSE_TX_RX_SUCCESS on success and other enum values on failure scenarios.
 * Also, in case of errors, heci_reset() is triggered.
 */
enum cse_tx_rx_status heci_receive(void *buff, size_t *maxlen);

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

enum cse_fw_sts_current_pm_event {
	PWR_CYCLE_RESET_CMOFF = 0xb,
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
enum cb_err cse_hmrfpo_enable(void);

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

/* Fixed Address MEI Header's ME Address field value for PSR messages */
#define HECI_PSR_ADDR	0x04

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
 * Checks current working operation state is normal or not.
 * Returns true if CSE's current working state is normal, otherwise false.
 */
bool cse_is_hfs1_cws_normal(void);

/*
 * Checks current working operation state is M3_NO_UMA or not.
 * Returns true if CSE's current working state is M3_NO_UMA, otherwise false.
 */
bool cse_is_hfs1_cws_m3_no_uma(void);

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

/* Perform a misc operation before CSE firmware update. */
void cse_fw_update_misc_oper(void);

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
enum cb_err cse_get_boot_performance_data(struct cse_boot_perf_rsp *boot_perf);

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
 * This function to perform essential post EOP cse related operations
 * upon SoC selecting `SOC_INTEL_CSE_SEND_EOP_LATE` config
 */
void cse_late_finalize(void);

/*
 * SoC override API to make heci1 disable using PCR.
 *
 * Allow SoC to implement heci1 disable override due to PSF registers being
 * different across SoC generation.
 */
void soc_disable_heci1_using_pcr(void);

/*
 * SoC override API to identify if ISH Firmware existed inside CSE FPT.
 *
 * This override is required to avoid making default call into non-ISH
 * supported SKU to attempt to retrieve ISH version which would results into
 * increased boot time by 100ms+.
 *
 * Ideally SoC with UFS enabled would like to keep ISH enabled as well, hence
 * identifying the UFS enabled device is enough to conclude if ISH partition is
 * available.
 */
#if CONFIG(SOC_INTEL_STORE_ISH_FW_VERSION)
bool soc_is_ish_partition_enabled(void);
#else
static inline bool soc_is_ish_partition_enabled(void)
{
	/* Default implementation, ISH not enabled. */
	return false;
}
#endif

/*
 * Injects CSE timestamps into cbmem timestamp table. SoC code needs to
 * implement it since timestamp definitions differ from SoC to SoC.
 */
void soc_cbmem_inject_telemetry_data(s64 *ts, s64 current_time);

/*
 * Get all the timestamps CSE collected using cse_get_boot_performance_data() and
 * insert them into the CBMEM timestamp table.
 */
void cse_get_telemetry_data(void);

/* Function to log the cse WP information like range, if WP etc. */
void cse_log_ro_write_protection_info(bool mfg_mode);

/*
 * Changes Intel PTT feature state at runtime. Global reset is required after
 * successful HECI command completion.
 */
void cse_enable_ptt(bool state);

/*
 * Queries CSE for runtime status of firmware features.
 * Returns 0 on success and < 0 on failure.
 */
enum cb_err cse_get_fw_feature_state(uint32_t *feature_state);

/* Fills the CSE Boot Partition Info response */
void cse_fill_bp_info(void);

/*
 * Check if a CSE Firmware update is required
 * Returns true if an update is required, false otherwise
 */
bool is_cse_fw_update_required(void);

/*
 * Check if the CSE firmware is booting from RW slot.
 * Returns true if CSE is booting from RW slot, false otherwise
 */
bool is_cse_boot_to_rw(void);

/*
 * Check if the CSE FW Status Current Power Management Event indicates that the
 * host came out of cold reset.
 * Returns true if the host came out of a cold reset, false otherwise.
 */
bool cse_check_host_cold_reset(void);

#endif // SOC_INTEL_COMMON_CSE_H
