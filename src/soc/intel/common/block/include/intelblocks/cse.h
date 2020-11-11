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

/* Global Reset Command ID */
#define MKHI_CBM_GLOBAL_RESET_REQ	0xb

/* Origin of Global Reset command */
#define GR_ORIGIN_BIOS_POST	0x2

/* HMRFPO Command Ids */
#define MKHI_HMRFPO_ENABLE	0x1
#define MKHI_HMRFPO_GET_STATUS	0x3

/* Get Firmware Version Command Id */
#define MKHI_GEN_GET_FW_VERSION	0x2

/* Boot partition info and set boot partition info command ids */
#define MKHI_BUP_COMMON_GET_BOOT_PARTITION_INFO	0x1c
#define MKHI_BUP_COMMON_SET_BOOT_PARTITION_INFO	0x1d
#define MKHI_BUP_COMMON_DATA_CLEAR		0x20

/* ME Current Working States */
#define ME_HFS1_CWS_NORMAL	0x5

/* ME Current Operation Modes */
#define ME_HFS1_COM_NORMAL	0x0
#define ME_HFS1_COM_SOFT_TEMP_DISABLE	0x3
#define ME_HFS1_COM_SECOVER_MEI_MSG	0x5

/* ME Firmware SKU Types */
#define ME_HFS3_FW_SKU_CONSUMER	0x2
#define ME_HFS3_FW_SKU_CORPORATE	0x3
#define ME_HFS3_FW_SKU_LITE	0x5

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

/*
 * CSE RW metadata structure
 * fw_version - CSE RW firmware version
 * sha256 - Hash of the CSE RW binary.
 */
struct cse_rw_metadata {
	struct fw_version version;
	uint8_t sha256[VB2_SHA256_DIGEST_SIZE];
};

/* set up device for use in early boot enviroument with temp bar */
void heci_init(uintptr_t bar);
/*
 * Receive message into buff not exceeding maxlen. Message is considered
 * successfully received if a 'complete' indication is read from ME side
 * and there was enough space in the buffer to fit that message. maxlen
 * is updated with size of message that was received. Returns 0 on failure
 * and 1 on success.
 * In case of error heci_reset() may be requiered.
 */
int heci_receive(void *buff, size_t *maxlen);
/*
 * Send message msg of size len to host from host_addr to cse_addr.
 * Returns 1 on success and 0 otherwise.
 * In case of error heci_reset() may be requiered.
 */
int
heci_send(const void *msg, size_t len, uint8_t host_addr, uint8_t cse_addr);

/*
 * Sends snd_msg of size snd_sz, and reads message into buffer pointed by
 * rcv_msg of size rcv_sz
 * Returns 0 on failure and 1 on success.
 */
int heci_send_receive(const void *snd_msg, size_t snd_sz, void *rcv_msg, size_t *rcv_sz);

/*
 * Attempt device reset. This is useful and perhaps only thing left to do when
 * CPU and CSE are out of sync or CSE fails to respond.
 * Returns 0 on failure and 1 on success.
 */
int heci_reset(void);
/* Disable HECI using Sideband interface communication */
void heci_disable(void);

/* Reads config value from a specified offset in the CSE PCI Config space. */
uint32_t me_read_config32(int offset);

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
 * CSE's boot partition as per Chrome OS boot modes. In normal mode, the function allows CSE to
 * boot from RW and triggers recovery mode if CSE fails to jump to RW.
 * In software triggered recovery mode, the function allows CSE to boot from whatever is
 * currently selected partition.
 */
void cse_fw_sync(void *unused);

/* Perform a board-specific reset sequence for CSE RO<->RW jump */
void cse_board_reset(void);

#endif // SOC_INTEL_COMMON_CSE_H
