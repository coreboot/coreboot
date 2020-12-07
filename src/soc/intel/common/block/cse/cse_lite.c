/* SPDX-License-Identifier: GPL-2.0-only */
#include <bootstate.h>
#include <console/console.h>
#include <boot_device.h>
#include <cbfs.h>
#include <commonlib/cbfs.h>
#include <commonlib/region.h>
#include <fmap.h>
#include <intelblocks/cse.h>
#include <security/vboot/vboot_common.h>
#include <security/vboot/misc.h>
#include <soc/intel/common/reset.h>

/* Converts bp index to boot partition string */
#define GET_BP_STR(bp_index) (bp_index ? "RW" : "RO")

/* CSE RW boot partition signature */
#define CSE_RW_SIGNATURE	0x000055aa

/* CSE RW boot partition signature size */
#define CSE_RW_SIGN_SIZE	sizeof(uint32_t)

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
	/* RO(BP1) contains recovery/minimal boot firmware */
	RO = 0,

	/* RW(BP2) contains fully functional CSE firmware */
	RW = 1
};

/* CSE recovery sub-error codes */
enum csme_failure_reason {

	/* No error */
	CSE_LITE_SKU_NO_ERROR = 0,

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
	CSE_LITE_SKU_DATA_WIPE_ERROR = 7,

	/* CSE RW is not found */
	CSE_LITE_SKU_RW_BLOB_NOT_FOUND = 8,

	/* CSE CBFS RW SHA-256 mismatch with the provided SHA */
	CSE_LITE_SKU_RW_BLOB_SHA256_MISMATCH = 9,

	/* CSE CBFS RW metadata is not found */
	CSE_LITE_SKU_RW_METADATA_NOT_FOUND = 10,

	/* CSE CBFS RW blob layout is not correct */
	CSE_LITE_SKU_LAYOUT_MISMATCH_ERROR = 11,
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

	/*
	 * This value is returned when unexpected issues are detected in CSE Data area
	 * and CSE TCB-SVN downgrade scenario.
	 */
	BP_STATUS_DATA_FAILURE = 3,
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

static void cse_log_status_registers(void)
{
	printk(BIOS_DEBUG, "cse_lite: CSE status registers: HFSTS1: 0x%x, HFSTS2: 0x%x "
			"HFSTS3: 0x%x\n", me_read_config32(PCI_ME_HFSTS1),
			me_read_config32(PCI_ME_HFSTS2), me_read_config32(PCI_ME_HFSTS3));
}

static void cse_trigger_recovery(uint8_t rec_sub_code)
{
	/* Log CSE Firmware Status Registers to help debugging */
	cse_log_status_registers();
	if (CONFIG(VBOOT)) {
		struct vb2_context *ctx = vboot_get_context();
		if (ctx == NULL)
			goto failure;
		vb2api_fail(ctx, VB2_RECOVERY_INTEL_CSE_LITE_SKU, rec_sub_code);
		vboot_save_data(ctx);
		vboot_reboot();
	}
failure:
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
 * The prerequisite check should be handled in cse_get_bp_info() and
 * cse_set_next_boot_partition() since the CSE's current operation mode is changed between these
 * cmd handler calls.
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

static bool cse_data_clear_request(const struct cse_bp_info *cse_bp_info)
{
	struct data_clr_request {
		struct mkhi_hdr hdr;
		uint8_t reserved[4];
	} __packed;

	struct data_clr_request data_clr_rq = {
		.hdr.group_id = MKHI_GROUP_ID_BUP_COMMON,
		.hdr.command = MKHI_BUP_COMMON_DATA_CLEAR,
		.reserved = {0},
	};

	if (!cse_is_hfs1_cws_normal() || !cse_is_hfs1_com_soft_temp_disable() ||
			cse_get_current_bp(cse_bp_info) != RO) {
		printk(BIOS_ERR, "cse_lite: CSE doesn't meet DATA CLEAR cmd prerequisites\n");
		return false;
	}

	printk(BIOS_DEBUG, "cse_lite: Sending DATA CLEAR HECI command\n");

	struct mkhi_hdr data_clr_rsp;
	size_t data_clr_rsp_sz = sizeof(data_clr_rsp);

	if (!heci_send_receive(&data_clr_rq, sizeof(data_clr_rq), &data_clr_rsp,
				&data_clr_rsp_sz)) {
		return false;
	}

	if (data_clr_rsp.result) {
		printk(BIOS_ERR, "cse_lite: CSE DATA CLEAR command response failed: %d\n",
				data_clr_rsp.result);
		return false;
	}

	return true;
}

__weak void cse_board_reset(void)
{
	/* Default weak implementation, does nothing. */
}

/* Set the CSE's next boot partition and issues system reset */
static bool cse_set_and_boot_from_next_bp(enum boot_partition_id bp)
{
	if (!cse_set_next_boot_partition(bp))
		return false;

	/* Allow the board to perform a reset for CSE RO<->RW jump */
	cse_board_reset();

	/* If board does not perform the reset, then perform global_reset */
	do_global_reset();

	die("cse_lite: Failed to reset the system\n");

	/* Control never reaches here */
	return false;
}

static bool cse_boot_to_rw(const struct cse_bp_info *cse_bp_info)
{
	if (cse_get_current_bp(cse_bp_info) == RW)
		return true;

	return cse_set_and_boot_from_next_bp(RW);
}

/* Check if CSE RW data partition is valid or not */
static bool cse_is_rw_dp_valid(const struct cse_bp_info *cse_bp_info)
{
	const struct cse_bp_entry *rw_bp;

	rw_bp = cse_get_bp_entry(RW, cse_bp_info);
	return rw_bp->status != BP_STATUS_DATA_FAILURE;
}

/*
 * It returns true if RW partition doesn't indicate BP_STATUS_DATA_FAILURE
 * otherwise false if any operation fails.
 */
static bool cse_fix_data_failure_err(const struct cse_bp_info *cse_bp_info)
{
	/*
	 * If RW partition status indicates BP_STATUS_DATA_FAILURE,
	 *  - Send DATA CLEAR HECI command to CSE
	 *  - Send SET BOOT PARTITION INFO(RW) command to set CSE's next partition
	 *  - Issue GLOBAL RESET HECI command.
	 */
	if (cse_is_rw_dp_valid(cse_bp_info))
		return true;

	if (!cse_data_clear_request(cse_bp_info))
		return false;

	return cse_boot_to_rw(cse_bp_info);
}

#if CONFIG(SOC_INTEL_CSE_RW_UPDATE)
static const struct fw_version *cse_get_bp_entry_version(enum boot_partition_id bp,
		const struct cse_bp_info *bp_info)
{
	const struct cse_bp_entry *cse_bp;

	cse_bp = cse_get_bp_entry(bp, bp_info);
	return &cse_bp->fw_ver;
}

static const struct fw_version *cse_get_rw_version(const struct cse_bp_info *cse_bp_info)
{
	return cse_get_bp_entry_version(RW, cse_bp_info);
}

static void cse_get_bp_entry_range(const struct cse_bp_info *cse_bp_info,
		enum boot_partition_id bp, uint32_t *start_offset, uint32_t *end_offset)
{
	const struct cse_bp_entry *cse_bp;

	cse_bp = cse_get_bp_entry(bp, cse_bp_info);

	if (start_offset)
		*start_offset = cse_bp->start_offset;

	if (end_offset)
		*end_offset = cse_bp->end_offset;

}

static bool cse_is_rw_bp_status_valid(const struct cse_bp_info *cse_bp_info)
{
	const struct cse_bp_entry *rw_bp;

	rw_bp = cse_get_bp_entry(RW, cse_bp_info);

	if (rw_bp->status == BP_STATUS_PARTITION_NOT_PRESENT ||
			rw_bp->status == BP_STATUS_GENERAL_FAILURE) {
		printk(BIOS_ERR, "cse_lite: RW BP (status:%u) is not valid\n", rw_bp->status);
		return false;
	}
	return true;
}

static bool cse_boot_to_ro(const struct cse_bp_info *cse_bp_info)
{
	if (cse_get_current_bp(cse_bp_info) == RO)
		return true;

	return cse_set_and_boot_from_next_bp(RO);
}

static bool cse_get_rw_rdev(struct region_device *rdev)
{
	if (fmap_locate_area_as_rdev_rw(CONFIG_SOC_INTEL_CSE_FMAP_NAME, rdev) < 0) {
		printk(BIOS_ERR, "cse_lite: Failed to locate %s in FMAP\n",
				CONFIG_SOC_INTEL_CSE_FMAP_NAME);
		return false;
	}

	return true;
}

static bool cse_is_rw_bp_sign_valid(const struct region_device *target_rdev)
{
	uint32_t cse_bp_sign;

	if (rdev_readat(target_rdev, &cse_bp_sign, 0, CSE_RW_SIGN_SIZE) != CSE_RW_SIGN_SIZE) {
		printk(BIOS_ERR, "cse_lite: Failed to read RW boot partition signature\n");
		return false;
	}

	return cse_bp_sign == CSE_RW_SIGNATURE;
}

static bool cse_get_target_rdev(const struct cse_bp_info *cse_bp_info,
		struct region_device *target_rdev)
{
	struct region_device cse_region_rdev;
	size_t size;
	uint32_t start_offset;
	uint32_t end_offset;

	if (!cse_get_rw_rdev(&cse_region_rdev))
		return false;

	cse_get_bp_entry_range(cse_bp_info, RW, &start_offset, &end_offset);
	size = end_offset + 1 - start_offset;

	if (rdev_chain(target_rdev, &cse_region_rdev, start_offset, size))
		return false;

	printk(BIOS_DEBUG, "cse_lite: CSE RW partition: offset = 0x%x, size = 0x%x\n",
			(uint32_t)start_offset, (uint32_t) size);

	return true;
}

static const char *cse_get_source_rdev_fmap(void)
{
	struct vb2_context *ctx = vboot_get_context();
	if (ctx == NULL)
		return NULL;

	if (vboot_is_firmware_slot_a(ctx))
		return CONFIG_SOC_INTEL_CSE_RW_A_FMAP_NAME;

	return CONFIG_SOC_INTEL_CSE_RW_B_FMAP_NAME;
}

static bool cse_get_source_rdev(struct region_device *rdev)
{
	const char *reg_name;
	uint32_t cbfs_type = CBFS_TYPE_RAW;
	struct cbfsf fh;

	reg_name = cse_get_source_rdev_fmap();

	if (reg_name == NULL)
		return false;

	if (cbfs_locate_file_in_region(&fh, reg_name, CONFIG_SOC_INTEL_CSE_RW_CBFS_NAME,
				&cbfs_type) < 0)
		return false;

	cbfs_file_data(rdev, &fh);

	return true;
}

/*
 * Compare versions of CSE CBFS RW and CSE RW partition
 * If ver_cmp_status = 0, no update is required
 * If ver_cmp_status < 0, coreboot downgrades CSE RW region
 * If ver_cmp_status > 0, coreboot upgrades CSE RW region
 */
static int cse_check_version_mismatch(const struct cse_bp_info *cse_bp_info,
	const struct cse_rw_metadata *source_metadata)
{
	const struct fw_version *cse_rw_ver;

	printk(BIOS_DEBUG, "cse_lite: CSE CBFS RW version : %d.%d.%d.%d\n",
			source_metadata->version.major,
			source_metadata->version.minor,
			source_metadata->version.hotfix,
			source_metadata->version.build);

	cse_rw_ver = cse_get_rw_version(cse_bp_info);

	if (source_metadata->version.major != cse_rw_ver->major)
		return source_metadata->version.major - cse_rw_ver->major;
	else if (source_metadata->version.minor != cse_rw_ver->minor)
		return source_metadata->version.minor - cse_rw_ver->minor;
	else if (source_metadata->version.hotfix != cse_rw_ver->hotfix)
		return source_metadata->version.hotfix - cse_rw_ver->hotfix;
	else
		return source_metadata->version.build - cse_rw_ver->build;
}

/* The function calculates SHA-256 of CSE RW blob and compares it with the provided SHA value */
static bool cse_verify_cbfs_rw_sha256(const uint8_t *expected_rw_blob_sha,
		const void *rw_blob, const size_t rw_blob_sz)

{
	uint8_t rw_comp_sha[VB2_SHA256_DIGEST_SIZE];

	if (vb2_digest_buffer(rw_blob, rw_blob_sz, VB2_HASH_SHA256, rw_comp_sha,
				VB2_SHA256_DIGEST_SIZE)) {
		printk(BIOS_ERR, "cse_lite: CSE CBFS RW's SHA-256 calculation has failed\n");
		return false;
	}

	if (memcmp(expected_rw_blob_sha, rw_comp_sha, VB2_SHA256_DIGEST_SIZE)) {
		printk(BIOS_ERR, "cse_lite: Computed CBFS RW's SHA-256 does not match with"
				"the provided SHA in the metadata\n");
		return false;
	}
	printk(BIOS_SPEW, "cse_lite: Computed SHA of CSE CBFS RW Image matches the"
			" provided hash in the metadata\n");
	return true;
}

static bool cse_erase_rw_region(const struct region_device *target_rdev)
{
	if (rdev_eraseat(target_rdev, 0, region_device_sz(target_rdev)) < 0) {
		printk(BIOS_ERR, "cse_lite: CSE RW partition could not be erased\n");
		return false;
	}
	return true;
}

static bool cse_copy_rw(const struct region_device *target_rdev, const void *buf,
		size_t offset, size_t size)
{
	if (rdev_writeat(target_rdev, buf, offset, size) < 0) {
		printk(BIOS_ERR, "cse_lite: Failed to update CSE firmware\n");
		return false;
	}

	return true;
}

static bool cse_is_rw_version_latest(const struct cse_bp_info *cse_bp_info,
		const struct cse_rw_metadata *source_metadata)
{
	return !cse_check_version_mismatch(cse_bp_info, source_metadata);
}

static bool cse_is_downgrade_instance(const struct cse_bp_info *cse_bp_info,
		const struct cse_rw_metadata *source_metadata)
{
	return cse_check_version_mismatch(cse_bp_info, source_metadata) < 0;
}

static bool cse_is_update_required(const struct cse_bp_info *cse_bp_info,
		const struct cse_rw_metadata *source_metadata,
		struct region_device *target_rdev)
{
	return (!cse_is_rw_bp_sign_valid(target_rdev) ||
			!cse_is_rw_version_latest(cse_bp_info, source_metadata));
}

static bool cse_write_rw_region(const struct region_device *target_rdev,
		const void *cse_cbfs_rw, const size_t cse_cbfs_rw_sz)
{
	/* Points to CSE CBFS RW image after boot partition signature */
	uint8_t *cse_cbfs_rw_wo_sign = (uint8_t *)cse_cbfs_rw + CSE_RW_SIGN_SIZE;

	/* Size of CSE CBFS RW image without boot partition signature */
	uint32_t cse_cbfs_rw_wo_sign_sz = cse_cbfs_rw_sz - CSE_RW_SIGN_SIZE;

	/* Update except CSE RW signature */
	if (!cse_copy_rw(target_rdev, cse_cbfs_rw_wo_sign, CSE_RW_SIGN_SIZE,
				cse_cbfs_rw_wo_sign_sz))
		return false;

	/* Update CSE RW signature to indicate update is complete */
	if (!cse_copy_rw(target_rdev, (void *)cse_cbfs_rw, 0, CSE_RW_SIGN_SIZE))
		return false;

	printk(BIOS_INFO, "cse_lite: CSE RW Update Successful\n");
	return true;
}

static enum csme_failure_reason cse_update_rw(const struct cse_bp_info *cse_bp_info,
		const void *cse_cbfs_rw, const size_t cse_blob_sz,
		struct region_device *target_rdev)
{
	if (region_device_sz(target_rdev) < cse_blob_sz) {
		printk(BIOS_ERR, "RW update does not fit. CSE RW flash region size: %zx, Update blob size:%zx\n",
				region_device_sz(target_rdev), cse_blob_sz);
		return CSE_LITE_SKU_LAYOUT_MISMATCH_ERROR;
	}

	if (!cse_erase_rw_region(target_rdev))
		return CSE_LITE_SKU_FW_UPDATE_ERROR;

	if (!cse_write_rw_region(target_rdev, cse_cbfs_rw, cse_blob_sz))
		return CSE_LITE_SKU_FW_UPDATE_ERROR;

	return CSE_LITE_SKU_NO_ERROR;
}

static bool cse_prep_for_rw_update(const struct cse_bp_info *cse_bp_info,
		const struct cse_rw_metadata *source_metadata)
{
	/*
	 * To set CSE's operation mode to HMRFPO mode:
	 * 1. Ensure CSE to boot from RO(BP1)
	 * 2. Send HMRFPO_ENABLE command to CSE
	 */
	if (!cse_boot_to_ro(cse_bp_info))
		return false;

	if (cse_is_downgrade_instance(cse_bp_info, source_metadata) &&
			!cse_data_clear_request(cse_bp_info)) {
		printk(BIOS_ERR, "cse_lite: CSE FW downgrade is aborted\n");
		return false;
	}

	return cse_hmrfpo_enable();
}

static enum csme_failure_reason cse_trigger_fw_update(const struct cse_bp_info *cse_bp_info,
		const struct cse_rw_metadata *source_metadata,
		struct region_device *target_rdev)
{
	struct region_device source_rdev;
	enum csme_failure_reason rv;

	if (!cse_get_source_rdev(&source_rdev))
		return CSE_LITE_SKU_RW_BLOB_NOT_FOUND;

	void *cse_cbfs_rw = rdev_mmap_full(&source_rdev);

	if (!cse_cbfs_rw) {
		printk(BIOS_ERR, "cse_lite: CSE CBFS RW blob could not be mapped\n");
		return CSE_LITE_SKU_RW_BLOB_NOT_FOUND;
	}

	if (!cse_verify_cbfs_rw_sha256(source_metadata->sha256, cse_cbfs_rw,
				region_device_sz(&source_rdev))) {
		rv = CSE_LITE_SKU_RW_BLOB_SHA256_MISMATCH;
		goto error_exit;
	}

	if (!cse_prep_for_rw_update(cse_bp_info, source_metadata)) {
		rv = CSE_LITE_SKU_COMMUNICATION_ERROR;
		goto error_exit;
	}

	rv = cse_update_rw(cse_bp_info, cse_cbfs_rw, region_device_sz(&source_rdev),
			target_rdev);

error_exit:
	rdev_munmap(&source_rdev, cse_cbfs_rw);
	return rv;
}

static uint8_t cse_fw_update(const struct cse_bp_info *cse_bp_info)
{
	struct region_device target_rdev;
	struct cse_rw_metadata source_metadata;

	/* Read CSE CBFS RW metadata */
	if (cbfs_load(CONFIG_SOC_INTEL_CSE_RW_METADATA_CBFS_NAME, &source_metadata,
			sizeof(source_metadata)) != sizeof(source_metadata)) {
		printk(BIOS_ERR, "cse_lite: Failed to get CSE CBFS RW metadata\n");
		return CSE_LITE_SKU_RW_METADATA_NOT_FOUND;
	}

	if (!cse_get_target_rdev(cse_bp_info, &target_rdev)) {
		printk(BIOS_ERR, "cse_lite: Failed to get CSE RW Partition\n");
		return CSE_LITE_SKU_RW_ACCESS_ERROR;
	}

	if (cse_is_update_required(cse_bp_info, &source_metadata, &target_rdev)) {
		printk(BIOS_DEBUG, "cse_lite: CSE RW update is initiated\n");
		return cse_trigger_fw_update(cse_bp_info, &source_metadata, &target_rdev);
	}

	if (!cse_is_rw_bp_status_valid(cse_bp_info))
		return CSE_LITE_SKU_RW_JUMP_ERROR;

	return 0;
}
#endif

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

	if (!cse_fix_data_failure_err(&cse_bp_info.bp_info))
		cse_trigger_recovery(CSE_LITE_SKU_DATA_WIPE_ERROR);

	/*
	 * If SOC_INTEL_CSE_RW_UPDATE is defined , then trigger CSE firmware update. The driver
	 * triggers recovery if CSE CBFS RW metadata or CSE CBFS RW blob is not available.
	 */
#if CONFIG(SOC_INTEL_CSE_RW_UPDATE)
	uint8_t rv;
	rv = cse_fw_update(&cse_bp_info.bp_info);
	if (rv)
		cse_trigger_recovery(rv);
#endif

	if (!cse_boot_to_rw(&cse_bp_info.bp_info)) {
		printk(BIOS_ERR, "cse_lite: Failed to switch to RW\n");
		cse_trigger_recovery(CSE_LITE_SKU_RW_SWITCH_ERROR);
	}
}

BOOT_STATE_INIT_ENTRY(BS_PRE_DEVICE, BS_ON_ENTRY, cse_fw_sync, NULL);
