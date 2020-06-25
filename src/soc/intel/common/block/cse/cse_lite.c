/* SPDX-License-Identifier: GPL-2.0-only */
#include <bootstate.h>
#include <console/console.h>
#include <soc/intel/common/reset.h>
#include <intelblocks/cse.h>
#include <security/vboot/vboot_common.h>
#include <security/vboot/misc.h>
#include <vb2_api.h>

/* Converts bp index to boot partition string */
#define GET_BP_STR(bp_index) (bp_index ? "RW" : "RO")

/*
 * CSE Firmware supports 3 boot partitions. For CSE Lite SKU, only 2 boot partitions are
 * used and 3rd boot partition is set to BP_STATUS_PARTITION_NOT_PRESENT.
 * CSE Lite SKU Image Layout:
 * -------------    -------------------    ---------------------
 * |CSE REGION | => | RO |  RW | DATA | => | BP1 | BP2  | DATA |
 *  -------------   -------------------    ---------------------
 */
#define CSE_MAX_BOOT_PARTITIONS 3

/* CSE Lite SKU's valid bootable partition identifiers */
enum boot_partition_id {
	/* RO(BP1) contains recovery/minimal boot FW */
	RO = 0,

	/* RW(BP2) contains fully functional CSE Firmware */
	RW = 1
};

/* CSE recovery sub-error codes */
enum csme_failure_reason {
	/* Unspecified error */
	CSE_LITE_SKU_UNSPECIFIED = 1,

	/* CSE fails to boot from RW */
	CSE_LITE_SKU_RW_JUMP_ERROR = 2,

	/* CSE RW boot partition access error */
	CSE_LITE_SKU_RW_ACCESS_ERROR = 3,

	/* Fails to set next boot partition as RW */
	CSE_LITE_SKU_RW_SWITCH_ERROR = 4,

	/* CSE firmware update failure */
	CSE_LITE_SKU_FW_UPDATE_ERROR = 5,

	/* Fails to communicate with CSE */
	CSE_LITE_SKU_COMMUNICATION_ERROR = 6,

	/* Fails to wipe CSE runtime data */
	CSE_LITE_SKU_DATA_WIPE_ERROR = 7
};

/*
 * Boot partition status.
 * The status is returned in response to MKHI_BUP_COMMON_GET_BOOT_PARTITION_INFO cmd.
 */
enum bp_status {
	/* This value is returned when a partition has no errors */
	BP_STATUS_SUCCESS = 0,

	/*
	 * This value is returned when a partition should be present based on layout, but it is
	 * not valid.
	 */
	BP_STATUS_GENERAL_FAILURE = 1,

	/* This value is returned when a partition is not present per initial image layout */
	BP_STATUS_PARTITION_NOT_PRESENT = 2,

};

/*
 * Boot Partition Info Flags
 * The flags are returned in response to MKHI_BUP_COMMON_GET_BOOT_PARTITION_INFO cmd.
 */
enum bp_info_flags {

	/* Redundancy Enabled: It indicates CSE supports RO(BP1) and RW(BP2) regions */
	BP_INFO_REDUNDANCY_EN = 1 << 0,

	/* It indicates RO(BP1) supports Minimal Recovery Mode */
	BP_INFO_MIN_RECOV_MODE_EN = 1 << 1,

	/*
	 * Read-only Config Enabled: It indicates HW protection to CSE RO region is enabled.
	 * The option is relevant only if the BP_INFO_MIN_RECOV_MODE_EN flag is enabled.
	 */
	BP_INFO_READ_ONLY_CFG = 1 << 2,
};

/* Boot Partition FW Version */
struct fw_version {
	uint16_t major;
	uint16_t minor;
	uint16_t hotfix;
	uint16_t build;
} __packed;

/* CSE boot partition entry info */
struct cse_bp_entry {
	/* Boot partition version */
	struct fw_version fw_ver;

	/* Boot partition status */
	uint32_t status;

	/* Starting offset of the partition within CSE region */
	uint32_t start_offset;

	/* Ending offset of the partition within CSE region */
	uint32_t end_offset;
	uint8_t reserved[12];
} __packed;

/* CSE boot partition info */
struct cse_bp_info {
	/* Number of boot partitions */
	uint8_t total_number_of_bp;

	/* Current boot partition */
	uint8_t current_bp;

	/* Next boot partition */
	uint8_t next_bp;

	/* Boot Partition Info Flags */
	uint8_t flags;

	/* Boot Partition Entry Info */
	struct cse_bp_entry bp_entries[CSE_MAX_BOOT_PARTITIONS];
} __packed;

struct get_bp_info_rsp {
	struct mkhi_hdr hdr;
	struct cse_bp_info bp_info;
} __packed;

static void cse_trigger_recovery(uint8_t rec_sub_code)
{
	if (CONFIG(VBOOT)) {
		struct vb2_context *ctx;
		ctx = vboot_get_context();
		vb2api_fail(ctx, VB2_RECOVERY_INTEL_CSE_LITE_SKU, rec_sub_code);
		vboot_save_data(ctx);
		vboot_reboot();
	}

	die("cse_lite: Failed to trigger recovery mode(recovery subcode:%d)\n", rec_sub_code);
}

static uint8_t cse_get_current_bp(const struct cse_bp_info *cse_bp_info)
{
	return cse_bp_info->current_bp;
}

static const struct cse_bp_entry *cse_get_bp_entry(enum boot_partition_id bp,
		const struct cse_bp_info *cse_bp_info)
{
	return &cse_bp_info->bp_entries[bp];
}

static void cse_print_boot_partition_info(const struct cse_bp_info *cse_bp_info)
{
	const struct cse_bp_entry *cse_bp;

	printk(BIOS_DEBUG, "cse_lite: Number of partitions = %d\n",
			cse_bp_info->total_number_of_bp);
	printk(BIOS_DEBUG, "cse_lite: Current partition = %s\n",
			GET_BP_STR(cse_bp_info->current_bp));
	printk(BIOS_DEBUG, "cse_lite: Next partition = %s\n", GET_BP_STR(cse_bp_info->next_bp));
	printk(BIOS_DEBUG, "cse_lite: Flags = 0x%x\n", cse_bp_info->flags);

	/* Log version info of RO & RW partitions */
	cse_bp = cse_get_bp_entry(RO, cse_bp_info);
	printk(BIOS_DEBUG, "cse_lite: %s version = %d.%d.%d.%d (Status=0x%x, Start=0x%x, End=0x%x)\n",
			GET_BP_STR(RO), cse_bp->fw_ver.major, cse_bp->fw_ver.minor,
			cse_bp->fw_ver.hotfix, cse_bp->fw_ver.build,
			cse_bp->status, cse_bp->start_offset,
			cse_bp->end_offset);

	cse_bp = cse_get_bp_entry(RW, cse_bp_info);
	printk(BIOS_DEBUG, "cse_lite: %s version = %d.%d.%d.%d (Status=0x%x, Start=0x%x, End=0x%x)\n",
			GET_BP_STR(RW), cse_bp->fw_ver.major, cse_bp->fw_ver.minor,
			cse_bp->fw_ver.hotfix, cse_bp->fw_ver.build,
			cse_bp->status, cse_bp->start_offset,
			cse_bp->end_offset);
}

/*
 * Checks prerequisites for MKHI_BUP_COMMON_GET_BOOT_PARTITION_INFO and
 * MKHI_BUP_COMMON_SET_BOOT_PARTITION_INFO HECI commands.
 * It allows execution of the Boot Partition commands in below scenarios:
 *	- When CSE boots from RW partition (COM: Normal and CWS: Normal)
 *	- When CSE boots from RO partition (COM: Soft Temp Disable and CWS: Normal)
 *	- After HMRFPO_ENABLE command is issued to CSE (COM: SECOVER_MEI_MSG and CWS: Normal)
 */
static bool cse_is_bp_cmd_info_possible(void)
{
	if (cse_is_hfs1_cws_normal()) {
		if (cse_is_hfs1_com_normal())
			return true;
		if (cse_is_hfs1_com_secover_mei_msg())
			return true;
		if (cse_is_hfs1_com_soft_temp_disable())
			return true;
	}
	return false;
}

static bool cse_get_bp_info(struct get_bp_info_rsp *bp_info_rsp)
{
	struct get_bp_info_req {
		struct mkhi_hdr hdr;
		uint8_t reserved[4];
	} __packed;

	struct get_bp_info_req info_req = {
		.hdr.group_id = MKHI_GROUP_ID_BUP_COMMON,
		.hdr.command = MKHI_BUP_COMMON_GET_BOOT_PARTITION_INFO,
		.reserved = {0},
	};

	if (!cse_is_bp_cmd_info_possible()) {
		printk(BIOS_ERR, "cse_lite: CSE does not meet prerequisites\n");
		return false;
	}

	size_t resp_size = sizeof(struct get_bp_info_rsp);

	if (!heci_send_receive(&info_req, sizeof(info_req), bp_info_rsp, &resp_size)) {
		printk(BIOS_ERR, "cse_lite: Could not get partition info\n");
		return false;
	}

	if (bp_info_rsp->hdr.result) {
		printk(BIOS_ERR, "cse_lite: Get partition info resp failed: %d\n",
				bp_info_rsp->hdr.result);
		return false;
	}

	cse_print_boot_partition_info(&bp_info_rsp->bp_info);

	return true;
}
/*
 * It sends HECI command to notify CSE about its next boot partition. When coreboot wants
 * CSE to boot from certain partition (BP1 <RO> or BP2 <RW>), then this command can be used.
 * The CSE's valid bootable partitions are BP1(RO) and BP2(RW).
 * This function must be used before EOP.
 * Returns false on failure and true on success.
 */
static bool cse_set_next_boot_partition(enum boot_partition_id bp)
{
	struct set_boot_partition_info_req {
		struct mkhi_hdr hdr;
		uint8_t next_bp;
		uint8_t reserved[3];
	} __packed;

	struct set_boot_partition_info_req switch_req = {
		.hdr.group_id = MKHI_GROUP_ID_BUP_COMMON,
		.hdr.command = MKHI_BUP_COMMON_SET_BOOT_PARTITION_INFO,
		.next_bp = bp,
		.reserved = {0},
	};

	if (bp != RO && bp != RW) {
		printk(BIOS_ERR, "cse_lite: Incorrect partition id(%d) is provided", bp);
		return false;
	}

	printk(BIOS_INFO, "cse_lite: Set Boot Partition Info Command (%s)\n", GET_BP_STR(bp));

	if (!cse_is_bp_cmd_info_possible()) {
		printk(BIOS_ERR, "cse_lite: CSE does not meet prerequisites\n");
		return false;
	}

	struct mkhi_hdr switch_resp;
	size_t sw_resp_sz = sizeof(struct mkhi_hdr);

	if (!heci_send_receive(&switch_req, sizeof(switch_req), &switch_resp, &sw_resp_sz))
		return false;

	if (switch_resp.result) {
		printk(BIOS_ERR, "cse_lite: Set Boot Partition Info Response Failed: %d\n",
				switch_resp.result);
		return false;
	}

	return true;
}

static bool cse_boot_to_rw(const struct cse_bp_info *cse_bp_info)
{
	if (cse_get_current_bp(cse_bp_info) == RW)
		return true;

	if (!cse_set_next_boot_partition(RW))
		return false;

	do_global_reset();

	die("cse_lite: Failed to reset system\n");

	/* Control never reaches here */
	return false;
}

static bool cse_is_rw_status_valid(const struct cse_bp_info *cse_bp_info)
{
	const struct cse_bp_entry *rw_bp;

	/* RW(BP2) alone represents RW partition */
	rw_bp = cse_get_bp_entry(RW, cse_bp_info);

	if (rw_bp->status == BP_STATUS_PARTITION_NOT_PRESENT ||
			rw_bp->status == BP_STATUS_GENERAL_FAILURE) {
		printk(BIOS_ERR, "cse_lite: RW BP (status:%u) is not valid\n", rw_bp->status);
		return false;
	}
	return true;
}

static bool cse_is_rw_info_valid(struct cse_bp_info *cse_bp_info)
{
	return cse_is_rw_status_valid(cse_bp_info);
}

void cse_fw_sync(void *unused)
{
	static struct get_bp_info_rsp cse_bp_info;

	if (vboot_recovery_mode_enabled()) {
		printk(BIOS_DEBUG, "cse_lite: Skip switching to RW in the recovery path\n");
		return;
	}

	/* If CSE SKU type is not Lite, skip enabling CSE Lite SKU */
	if (!cse_is_hfs3_fw_sku_lite()) {
		printk(BIOS_ERR, "cse_lite: Not a CSE Lite SKU\n");
		return;
	}

	if (!cse_get_bp_info(&cse_bp_info)) {
		printk(BIOS_ERR, "cse_lite: Failed to get CSE boot partition info\n");
		cse_trigger_recovery(CSE_LITE_SKU_COMMUNICATION_ERROR);
	}

	if (!cse_is_rw_info_valid(&cse_bp_info.bp_info)) {
		printk(BIOS_ERR, "cse_lite: CSE RW partition is not valid\n");
		cse_trigger_recovery(CSE_LITE_SKU_RW_JUMP_ERROR);
	}

	if (!cse_boot_to_rw(&cse_bp_info.bp_info)) {
		printk(BIOS_ERR, "cse_lite: Failed to switch to RW\n");
		cse_trigger_recovery(CSE_LITE_SKU_RW_SWITCH_ERROR);
	}
}

#if CONFIG(SOC_INTEL_TIGERLAKE)
BOOT_STATE_INIT_ENTRY(BS_DEV_INIT_CHIPS, BS_ON_ENTRY, cse_fw_sync, NULL);
#else
BOOT_STATE_INIT_ENTRY(BS_PRE_DEVICE, BS_ON_ENTRY, cse_fw_sync, NULL);
#endif
