/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <bootstate.h>
#include <cbfs.h>
#include <commonlib/region.h>
#include <console/console.h>
#include <cpu/cpu.h>
#include <fmap.h>
#include <intelbasecode/debug_feature.h>
#include <intelblocks/cse.h>
#include <intelblocks/cse_layout.h>
#include <intelblocks/spi.h>
#include <security/vboot/misc.h>
#include <security/vboot/vboot_common.h>
#include <soc/intel/common/reset.h>
#include <timestamp.h>

#define BPDT_HEADER_SZ		sizeof(struct bpdt_header)
#define BPDT_ENTRY_SZ		sizeof(struct bpdt_entry)
#define SUBPART_HEADER_SZ	sizeof(struct subpart_hdr)
#define SUBPART_ENTRY_SZ	sizeof(struct subpart_entry)
#define SUBPART_MANIFEST_HDR_SZ	sizeof(struct subpart_entry_manifest_header)

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
 * +------------+    +----+------+----+    +-----+------+-----+
 * | CSE REGION | => | RO | DATA | RW | => | BP1 | DATA | BP2 |
 * +------------+    +----+------+----+    +-----+------+-----+
 */
#define CSE_MAX_BOOT_PARTITIONS 3

/* CSE Lite SKU's valid bootable partition identifiers */
enum boot_partition_id {
	/* RO(BP1) contains recovery/minimal boot firmware */
	RO = 0,

	/* RW(BP2) contains fully functional CSE firmware */
	RW = 1
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

static const char * const cse_regions[] = {"RO", "RW"};

void cse_log_ro_write_protection_info(bool mfg_mode)
{
	bool cse_ro_wp_en = is_spi_wp_cse_ro_en();

	printk(BIOS_DEBUG, "ME: WP for RO is enabled        : %s\n",
			cse_ro_wp_en ? "YES" : "NO");

	if (cse_ro_wp_en) {
		uint32_t base, limit;
		spi_get_wp_cse_ro_range(&base, &limit);
		printk(BIOS_DEBUG, "ME: RO write protection scope - Start=0x%X, End=0x%X\n",
				base, limit);
	}

	/*
	 * If manufacturing mode is disabled, but CSE RO is not write protected,
	 * log error.
	 */
	if (!mfg_mode && !cse_ro_wp_en)
		printk(BIOS_ERR, "ME: Write protection for CSE RO is not enabled\n");
}

enum cb_err cse_get_boot_performance_data(struct cse_boot_perf_rsp *boot_perf_rsp)
{
	struct cse_boot_perf_req {
		struct mkhi_hdr hdr;
		uint32_t reserved;
	} __packed;

	struct cse_boot_perf_req req = {
		.hdr.group_id = MKHI_GROUP_ID_BUP_COMMON,
		.hdr.command = MKHI_BUP_COMMON_GET_BOOT_PERF_DATA,
		.reserved = 0,
	};

	size_t resp_size = sizeof(struct cse_boot_perf_rsp);

	if (heci_send_receive(&req, sizeof(req), boot_perf_rsp, &resp_size,
									HECI_MKHI_ADDR)) {
		printk(BIOS_ERR, "cse_lite: Could not get boot performance data\n");
		return CB_ERR;
	}

	if (boot_perf_rsp->hdr.result) {
		printk(BIOS_ERR, "cse_lite: Get boot performance data resp failed: %d\n",
				boot_perf_rsp->hdr.result);
		return CB_ERR;
	}

	return CB_SUCCESS;
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

static enum cb_err cse_get_bp_info(struct get_bp_info_rsp *bp_info_rsp)
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
		return CB_ERR;
	}

	size_t resp_size = sizeof(struct get_bp_info_rsp);

	if (heci_send_receive(&info_req, sizeof(info_req), bp_info_rsp, &resp_size,
									HECI_MKHI_ADDR)) {
		printk(BIOS_ERR, "cse_lite: Could not get partition info\n");
		return CB_ERR;
	}

	if (bp_info_rsp->hdr.result) {
		printk(BIOS_ERR, "cse_lite: Get partition info resp failed: %d\n",
				bp_info_rsp->hdr.result);
		return CB_ERR;
	}

	cse_print_boot_partition_info(&bp_info_rsp->bp_info);

	return CB_SUCCESS;
}
/*
 * It sends HECI command to notify CSE about its next boot partition. When coreboot wants
 * CSE to boot from certain partition (BP1 <RO> or BP2 <RW>), then this command can be used.
 * The CSE's valid bootable partitions are BP1(RO) and BP2(RW).
 * This function must be used before EOP.
 * Returns false on failure and true on success.
 */
static enum cb_err cse_set_next_boot_partition(enum boot_partition_id bp)
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
		return CB_ERR_ARG;
	}

	printk(BIOS_INFO, "cse_lite: Set Boot Partition Info Command (%s)\n", GET_BP_STR(bp));

	if (!cse_is_bp_cmd_info_possible()) {
		printk(BIOS_ERR, "cse_lite: CSE does not meet prerequisites\n");
		return CB_ERR;
	}

	struct mkhi_hdr switch_resp;
	size_t sw_resp_sz = sizeof(struct mkhi_hdr);

	if (heci_send_receive(&switch_req, sizeof(switch_req), &switch_resp, &sw_resp_sz,
									HECI_MKHI_ADDR))
		return CB_ERR;

	if (switch_resp.result) {
		printk(BIOS_ERR, "cse_lite: Set Boot Partition Info Response Failed: %d\n",
				switch_resp.result);
		return CB_ERR;
	}

	return CB_SUCCESS;
}

static enum cb_err cse_data_clear_request(const struct cse_bp_info *cse_bp_info)
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
		return CB_ERR;
	}

	printk(BIOS_DEBUG, "cse_lite: Sending DATA CLEAR HECI command\n");

	struct mkhi_hdr data_clr_rsp;
	size_t data_clr_rsp_sz = sizeof(data_clr_rsp);

	if (heci_send_receive(&data_clr_rq, sizeof(data_clr_rq), &data_clr_rsp,
				&data_clr_rsp_sz, HECI_MKHI_ADDR)) {
		return CB_ERR;
	}

	if (data_clr_rsp.result) {
		printk(BIOS_ERR, "cse_lite: CSE DATA CLEAR command response failed: %d\n",
				data_clr_rsp.result);
		return CB_ERR;
	}

	return CB_SUCCESS;
}

__weak void cse_board_reset(void)
{
	/* Default weak implementation, does nothing. */
}

__weak void cse_fw_update_misc_oper(void)
{
	/* Default weak implementation, does nothing. */
}

/* Set the CSE's next boot partition and issues system reset */
static enum cb_err cse_set_and_boot_from_next_bp(enum boot_partition_id bp)
{
	if (cse_set_next_boot_partition(bp) != CB_SUCCESS)
		return CB_ERR;

	/* Allow the board to perform a reset for CSE RO<->RW jump */
	cse_board_reset();

	/* If board does not perform the reset, then perform global_reset */
	do_global_reset();

	die("cse_lite: Failed to reset the system\n");

	/* Control never reaches here */
	return CB_ERR;
}

static enum cb_err cse_boot_to_rw(const struct cse_bp_info *cse_bp_info)
{
	if (cse_get_current_bp(cse_bp_info) == RW)
		return CB_SUCCESS;

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
static enum cb_err cse_fix_data_failure_err(const struct cse_bp_info *cse_bp_info)
{
	/*
	 * If RW partition status indicates BP_STATUS_DATA_FAILURE,
	 *  - Send DATA CLEAR HECI command to CSE
	 *  - Send SET BOOT PARTITION INFO(RW) command to set CSE's next partition
	 *  - Issue GLOBAL RESET HECI command.
	 */
	if (cse_is_rw_dp_valid(cse_bp_info))
		return CB_SUCCESS;

	if (cse_data_clear_request(cse_bp_info) != CB_SUCCESS)
		return CB_ERR;

	return cse_boot_to_rw(cse_bp_info);
}

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

static enum cb_err cse_boot_to_ro(const struct cse_bp_info *cse_bp_info)
{
	if (cse_get_current_bp(cse_bp_info) == RO)
		return CB_SUCCESS;

	return cse_set_and_boot_from_next_bp(RO);
}

static enum cb_err cse_get_rw_rdev(struct region_device *rdev)
{
	if (fmap_locate_area_as_rdev_rw(CONFIG_SOC_INTEL_CSE_FMAP_NAME, rdev) < 0) {
		printk(BIOS_ERR, "cse_lite: Failed to locate %s in FMAP\n",
				CONFIG_SOC_INTEL_CSE_FMAP_NAME);
		return CB_ERR;
	}

	return CB_SUCCESS;
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

static enum cb_err cse_get_target_rdev(const struct cse_bp_info *cse_bp_info,
		struct region_device *target_rdev)
{
	struct region_device cse_region_rdev;
	size_t size;
	uint32_t start_offset;
	uint32_t end_offset;

	if (cse_get_rw_rdev(&cse_region_rdev) != CB_SUCCESS)
		return CB_ERR;

	cse_get_bp_entry_range(cse_bp_info, RW, &start_offset, &end_offset);
	size = end_offset + 1 - start_offset;

	if (rdev_chain(target_rdev, &cse_region_rdev, start_offset, size))
		return CB_ERR;

	printk(BIOS_DEBUG, "cse_lite: CSE RW partition: offset = 0x%x, size = 0x%x\n",
			(uint32_t)start_offset, (uint32_t)size);

	return CB_SUCCESS;
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

/*
 * Compare versions of CSE CBFS sub-component and CSE sub-component partition
 * In case of CSE component comparison:
 * If ver_cmp_status = 0, no update is required
 * If ver_cmp_status < 0, coreboot downgrades CSE RW region
 * If ver_cmp_status > 0, coreboot upgrades CSE RW region
 */
static int cse_compare_sub_part_version(const struct fw_version *a, const struct fw_version *b)
{
	if (a->major != b->major)
		return a->major - b->major;
	else if (a->minor != b->minor)
		return a->minor - b->minor;
	else if (a->hotfix != b->hotfix)
		return a->hotfix - b->hotfix;
	else
		return a->build - b->build;
}

/* The function calculates SHA-256 of CSE RW blob and compares it with the provided SHA value */
static bool cse_verify_cbfs_rw_sha256(const uint8_t *expected_rw_blob_sha,
		const void *rw_blob, const size_t rw_blob_sz)

{
	struct vb2_hash calculated;

	if (vb2_hash_calculate(vboot_hwcrypto_allowed(), rw_blob, rw_blob_sz,
			       VB2_HASH_SHA256, &calculated)) {
		printk(BIOS_ERR, "cse_lite: CSE CBFS RW's SHA-256 calculation has failed\n");
		return false;
	}

	if (memcmp(expected_rw_blob_sha, calculated.sha256, sizeof(calculated.sha256))) {
		printk(BIOS_ERR, "cse_lite: Computed CBFS RW's SHA-256 does not match with"
				"the provided SHA in the metadata\n");
		return false;
	}
	printk(BIOS_SPEW, "cse_lite: Computed SHA of CSE CBFS RW Image matches the"
			" provided hash in the metadata\n");
	return true;
}

static enum cb_err cse_erase_rw_region(const struct region_device *target_rdev)
{
	if (rdev_eraseat(target_rdev, 0, region_device_sz(target_rdev)) < 0) {
		printk(BIOS_ERR, "cse_lite: CSE RW partition could not be erased\n");
		return CB_ERR;
	}
	return CB_SUCCESS;
}

static enum cb_err cse_copy_rw(const struct region_device *target_rdev, const void *buf,
		size_t offset, size_t size)
{
	if (rdev_writeat(target_rdev, buf, offset, size) < 0) {
		printk(BIOS_ERR, "cse_lite: Failed to update CSE firmware\n");
		return CB_ERR;
	}

	return CB_SUCCESS;
}

enum cse_update_status {
	CSE_UPDATE_NOT_REQUIRED,
	CSE_UPDATE_UPGRADE,
	CSE_UPDATE_DOWNGRADE,
	CSE_UPDATE_CORRUPTED,
	CSE_UPDATE_METADATA_ERROR,
};

static bool read_ver_field(const char *start, char **curr, size_t size, uint16_t *ver_field)
{
	if ((*curr - start) >= size) {
		printk(BIOS_ERR, "cse_lite: Version string read overflow!\n");
		return false;
	}

	*ver_field = skip_atoi(curr);
	(*curr)++;
	return true;
}

static enum cse_update_status cse_check_update_status(const struct cse_bp_info *cse_bp_info,
						      struct region_device *target_rdev)
{
	int ret;
	struct fw_version cbfs_rw_version;
	char *version_str, *ptr;
	size_t size;

	if (!cse_is_rw_bp_sign_valid(target_rdev))
		return CSE_UPDATE_CORRUPTED;

	ptr = version_str = cbfs_map(CONFIG_SOC_INTEL_CSE_RW_VERSION_CBFS_NAME, &size);
	if (!version_str) {
		printk(BIOS_ERR, "cse_lite: Failed to get %s\n",
		       CONFIG_SOC_INTEL_CSE_RW_VERSION_CBFS_NAME);
		return CSE_UPDATE_METADATA_ERROR;
	}

	if (!read_ver_field(version_str, &ptr, size, &cbfs_rw_version.major) ||
	    !read_ver_field(version_str, &ptr, size, &cbfs_rw_version.minor) ||
	    !read_ver_field(version_str, &ptr, size, &cbfs_rw_version.hotfix) ||
	    !read_ver_field(version_str, &ptr, size, &cbfs_rw_version.build)) {
		cbfs_unmap(version_str);
		return CSE_UPDATE_METADATA_ERROR;
	}

	printk(BIOS_DEBUG, "cse_lite: CSE CBFS RW version : %d.%d.%d.%d\n",
			cbfs_rw_version.major,
			cbfs_rw_version.minor,
			cbfs_rw_version.hotfix,
			cbfs_rw_version.build);

	cbfs_unmap(version_str);

	ret = cse_compare_sub_part_version(&cbfs_rw_version, cse_get_rw_version(cse_bp_info));
	if (ret == 0)
		return CSE_UPDATE_NOT_REQUIRED;
	else if (ret < 0)
		return CSE_UPDATE_DOWNGRADE;
	else
		return CSE_UPDATE_UPGRADE;
}

static enum cb_err cse_write_rw_region(const struct region_device *target_rdev,
		const void *cse_cbfs_rw, const size_t cse_cbfs_rw_sz)
{
	/* Points to CSE CBFS RW image after boot partition signature */
	uint8_t *cse_cbfs_rw_wo_sign = (uint8_t *)cse_cbfs_rw + CSE_RW_SIGN_SIZE;

	/* Size of CSE CBFS RW image without boot partition signature */
	uint32_t cse_cbfs_rw_wo_sign_sz = cse_cbfs_rw_sz - CSE_RW_SIGN_SIZE;

	/* Update except CSE RW signature */
	if (cse_copy_rw(target_rdev, cse_cbfs_rw_wo_sign, CSE_RW_SIGN_SIZE,
				cse_cbfs_rw_wo_sign_sz) != CB_SUCCESS)
		return CB_ERR;

	/* Update CSE RW signature to indicate update is complete */
	if (cse_copy_rw(target_rdev, (void *)cse_cbfs_rw, 0, CSE_RW_SIGN_SIZE) != CB_SUCCESS)
		return CB_ERR;

	printk(BIOS_INFO, "cse_lite: CSE RW Update Successful\n");
	return CB_SUCCESS;
}

static bool is_cse_fw_update_enabled(void)
{
	if (!CONFIG(SOC_INTEL_CSE_RW_UPDATE))
		return false;

	if (CONFIG(SOC_INTEL_COMMON_BASECODE_DEBUG_FEATURE))
		return !is_debug_cse_fw_update_disable();

	return true;
}

static enum csme_failure_reason cse_update_rw(const struct cse_bp_info *cse_bp_info,
		const void *cse_cbfs_rw, const size_t cse_blob_sz,
		struct region_device *target_rdev)
{
	if (region_device_sz(target_rdev) < cse_blob_sz) {
		printk(BIOS_ERR, "RW update does not fit. CSE RW flash region size: %zx,"
			"Update blob size:%zx\n", region_device_sz(target_rdev), cse_blob_sz);
		return CSE_LITE_SKU_LAYOUT_MISMATCH_ERROR;
	}

	if (cse_erase_rw_region(target_rdev) != CB_SUCCESS)
		return CSE_LITE_SKU_FW_UPDATE_ERROR;

	if (cse_write_rw_region(target_rdev, cse_cbfs_rw, cse_blob_sz) != CB_SUCCESS)
		return CSE_LITE_SKU_FW_UPDATE_ERROR;

	return CSE_NO_ERROR;
}

static enum cb_err cse_prep_for_rw_update(const struct cse_bp_info *cse_bp_info,
				   enum cse_update_status status)
{
	/*
	 * To set CSE's operation mode to HMRFPO mode:
	 * 1. Ensure CSE to boot from RO(BP1)
	 * 2. Send HMRFPO_ENABLE command to CSE
	 */
	if (cse_boot_to_ro(cse_bp_info) != CB_SUCCESS)
		return CB_ERR;

	if ((status == CSE_UPDATE_DOWNGRADE) || (status == CSE_UPDATE_CORRUPTED)) {
		if (cse_data_clear_request(cse_bp_info) != CB_SUCCESS) {
			printk(BIOS_ERR, "cse_lite: CSE data clear failed!\n");
			return CB_SUCCESS;
		}
	}

	return cse_hmrfpo_enable();
}

static enum csme_failure_reason cse_trigger_fw_update(const struct cse_bp_info *cse_bp_info,
						      enum cse_update_status status,
						      struct region_device *target_rdev)
{
	enum csme_failure_reason rv;
	uint8_t *cbfs_rw_hash;
	void *cse_cbfs_rw = NULL;
	size_t size;

	const char *area_name = cse_get_source_rdev_fmap();
	if (!area_name)
		return CSE_LITE_SKU_RW_BLOB_NOT_FOUND;

	if (CONFIG(SOC_INTEL_CSE_LITE_COMPRESS_ME_RW)) {
		cse_cbfs_rw = cbfs_unverified_area_cbmem_alloc(area_name,
			CONFIG_SOC_INTEL_CSE_RW_CBFS_NAME, CBMEM_ID_CSE_UPDATE, &size);
	} else {
		cse_cbfs_rw = cbfs_unverified_area_map(area_name,
			CONFIG_SOC_INTEL_CSE_RW_CBFS_NAME, &size);
	}
	if (!cse_cbfs_rw) {
		printk(BIOS_ERR, "cse_lite: CSE CBFS RW blob could not be mapped\n");
		return CSE_LITE_SKU_RW_BLOB_NOT_FOUND;
	}

	cbfs_rw_hash = cbfs_map(CONFIG_SOC_INTEL_CSE_RW_HASH_CBFS_NAME, NULL);
	if (!cbfs_rw_hash) {
		printk(BIOS_ERR, "cse_lite: Failed to get %s\n",
		       CONFIG_SOC_INTEL_CSE_RW_HASH_CBFS_NAME);
		rv = CSE_LITE_SKU_RW_METADATA_NOT_FOUND;
		goto error_exit;
	}

	if (!cse_verify_cbfs_rw_sha256(cbfs_rw_hash, cse_cbfs_rw, size)) {
		rv = CSE_LITE_SKU_RW_BLOB_SHA256_MISMATCH;
		goto error_exit;
	}

	if (cse_prep_for_rw_update(cse_bp_info, status) != CB_SUCCESS) {
		rv = CSE_COMMUNICATION_ERROR;
		goto error_exit;
	}

	cse_fw_update_misc_oper();
	rv = cse_update_rw(cse_bp_info, cse_cbfs_rw, size, target_rdev);

error_exit:
	cbfs_unmap(cbfs_rw_hash);
	cbfs_unmap(cse_cbfs_rw);
	return rv;
}

static uint8_t cse_fw_update(const struct cse_bp_info *cse_bp_info)
{
	struct region_device target_rdev;
	enum cse_update_status status;

	if (cse_get_target_rdev(cse_bp_info, &target_rdev) != CB_SUCCESS) {
		printk(BIOS_ERR, "cse_lite: Failed to get CSE RW Partition\n");
		return CSE_LITE_SKU_RW_ACCESS_ERROR;
	}

	status = cse_check_update_status(cse_bp_info, &target_rdev);
	if (status == CSE_UPDATE_NOT_REQUIRED)
		return CSE_NO_ERROR;
	if (status == CSE_UPDATE_METADATA_ERROR)
		return CSE_LITE_SKU_RW_METADATA_NOT_FOUND;

	printk(BIOS_DEBUG, "cse_lite: CSE RW update is initiated\n");
	return cse_trigger_fw_update(cse_bp_info, status, &target_rdev);
}

static const char *cse_sub_part_str(enum bpdt_entry_type type)
{
	switch (type) {
	case IOM_FW:
		return "IOM";
	case NPHY_FW:
		return "NPHY";
	default:
		return "Unknown";
	}
}

static enum cb_err cse_locate_area_as_rdev_rw(const struct cse_bp_info *cse_bp_info,
		size_t bp, struct region_device  *cse_rdev)
{
	struct region_device cse_region_rdev;
	uint32_t size;
	uint32_t start_offset;
	uint32_t end_offset;

	if (cse_get_rw_rdev(&cse_region_rdev) != CB_SUCCESS)
		return CB_ERR;

	if (!strcmp(cse_regions[bp], "RO"))
		cse_get_bp_entry_range(cse_bp_info, RO, &start_offset, &end_offset);
	else
		cse_get_bp_entry_range(cse_bp_info, RW, &start_offset, &end_offset);

	size = end_offset + 1 - start_offset;

	if (rdev_chain(cse_rdev, &cse_region_rdev, start_offset, size))
		return CB_ERR;

	printk(BIOS_DEBUG, "cse_lite: CSE %s  partition: offset = 0x%x, size = 0x%x\n",
			cse_regions[bp], start_offset, size);
	return CB_SUCCESS;
}

static enum cb_err cse_sub_part_get_target_rdev(const struct cse_bp_info *cse_bp_info,
	struct region_device *target_rdev, size_t bp, enum bpdt_entry_type type)
{
	struct bpdt_header bpdt_hdr;
	struct region_device cse_rdev;
	struct bpdt_entry bpdt_entries[MAX_SUBPARTS];
	uint8_t i;

	if (cse_locate_area_as_rdev_rw(cse_bp_info, bp, &cse_rdev) != CB_SUCCESS) {
		printk(BIOS_ERR, "cse_lite: Failed to locate %s in the CSE Region\n",
				cse_regions[bp]);
		return CB_ERR;
	}

	if ((rdev_readat(&cse_rdev, &bpdt_hdr, 0, BPDT_HEADER_SZ)) != BPDT_HEADER_SZ) {
		printk(BIOS_ERR, "cse_lite: Failed to read BPDT header from CSE region\n");
		return CB_ERR;
	}

	if ((rdev_readat(&cse_rdev, bpdt_entries, BPDT_HEADER_SZ,
		(bpdt_hdr.descriptor_count * BPDT_ENTRY_SZ))) !=
		(bpdt_hdr.descriptor_count * BPDT_ENTRY_SZ)) {
		printk(BIOS_ERR, "cse_lite: Failed to read BPDT entries from CSE region\n");
		return CB_ERR;
	}

	/* walk through BPDT entries to identify sub-partition's payload offset and size */
	for (i = 0; i < bpdt_hdr.descriptor_count; i++) {
		if (bpdt_entries[i].type == type) {
			printk(BIOS_INFO, "cse_lite: Sub-partition %s- offset = 0x%x,"
				"size = 0x%x\n", cse_sub_part_str(type), bpdt_entries[i].offset,
					bpdt_entries[i].size);

			if (rdev_chain(target_rdev, &cse_rdev, bpdt_entries[i].offset,
				bpdt_entries[i].size))
				return CB_ERR;
			else
				return CB_SUCCESS;
		}
	}

	printk(BIOS_ERR, "cse_lite: Sub-partition %s is not found\n", cse_sub_part_str(type));
	return CB_ERR;
}

static enum cb_err cse_get_sub_part_fw_version(enum bpdt_entry_type type,
					const struct region_device *rdev,
					struct fw_version *fw_ver)
{
	struct subpart_entry subpart_entry;
	struct subpart_entry_manifest_header man_hdr;

	if ((rdev_readat(rdev, &subpart_entry, SUBPART_HEADER_SZ, SUBPART_ENTRY_SZ))
			!= SUBPART_ENTRY_SZ) {
		printk(BIOS_ERR, "cse_lite: Failed to read %s sub partition entry\n",
				cse_sub_part_str(type));
		return CB_ERR;
	}

	if ((rdev_readat(rdev, &man_hdr, subpart_entry.offset_bytes, SUBPART_MANIFEST_HDR_SZ))
			!= SUBPART_MANIFEST_HDR_SZ) {
		printk(BIOS_ERR, "cse_lite: Failed to read %s Sub part entry #0 manifest\n",
				cse_sub_part_str(type));
		return CB_ERR;
	}

	fw_ver->major = man_hdr.binary_version.major;
	fw_ver->minor = man_hdr.binary_version.minor;
	fw_ver->hotfix = man_hdr.binary_version.hotfix;
	fw_ver->build = man_hdr.binary_version.build;

	return CB_SUCCESS;
}

static void cse_sub_part_get_source_fw_version(void *subpart_cbfs_rw, struct fw_version *fw_ver)
{
	uint8_t *ptr = (uint8_t *)subpart_cbfs_rw;
	struct subpart_entry *subpart_entry;
	struct subpart_entry_manifest_header *man_hdr;

	subpart_entry = (struct subpart_entry *)(ptr + SUBPART_HEADER_SZ);
	man_hdr = (struct subpart_entry_manifest_header *)(ptr + subpart_entry->offset_bytes);

	fw_ver->major = man_hdr->binary_version.major;
	fw_ver->minor = man_hdr->binary_version.minor;
	fw_ver->hotfix = man_hdr->binary_version.hotfix;
	fw_ver->build = man_hdr->binary_version.build;
}

static enum cb_err cse_prep_for_component_update(const struct cse_bp_info *cse_bp_info)
{
	/*
	 * To set CSE's operation mode to HMRFPO mode:
	 * 1. Ensure CSE to boot from RO(BP1)
	 * 2. Send HMRFPO_ENABLE command to CSE
	 */
	if (cse_boot_to_ro(cse_bp_info) != CB_SUCCESS)
		return CB_ERR;

	return cse_hmrfpo_enable();
}

static enum csme_failure_reason cse_sub_part_trigger_update(enum bpdt_entry_type type,
		uint8_t bp, const void *subpart_cbfs_rw, const size_t blob_sz,
		struct region_device *target_rdev)
{
	if (region_device_sz(target_rdev) < blob_sz) {
		printk(BIOS_ERR, "cse_lite: %s Target sub-partition size: %zx, "
				"smaller than blob size:%zx, abort update\n",
				cse_sub_part_str(type), region_device_sz(target_rdev), blob_sz);
		return CSE_LITE_SKU_SUB_PART_LAYOUT_MISMATCH_ERROR;
	}

	/* Erase CSE Lite sub-partition */
	if (cse_erase_rw_region(target_rdev) != CB_SUCCESS)
		return CSE_LITE_SKU_SUB_PART_UPDATE_FAIL;

	/* Update CSE Lite sub-partition */
	if (cse_copy_rw(target_rdev, (void *)subpart_cbfs_rw, 0, blob_sz) != CB_SUCCESS)
		return CSE_LITE_SKU_SUB_PART_UPDATE_FAIL;

	printk(BIOS_INFO, "cse_lite: CSE %s %s Update successful\n", GET_BP_STR(bp),
			cse_sub_part_str(type));

	return CSE_LITE_SKU_PART_UPDATE_SUCCESS;
}

static enum csme_failure_reason handle_cse_sub_part_fw_update_rv(enum csme_failure_reason rv)
{
	switch (rv) {
	case CSE_LITE_SKU_PART_UPDATE_SUCCESS:
	case CSE_LITE_SKU_SUB_PART_UPDATE_NOT_REQ:
		return rv;
	default:
		cse_trigger_vboot_recovery(rv);
	}
	/* Control never reaches here */
	return rv;
}

static enum csme_failure_reason cse_sub_part_fw_component_update(enum bpdt_entry_type type,
		const struct cse_bp_info *cse_bp_info, const char *name)
{
	struct region_device target_rdev;
	struct fw_version target_fw_ver, source_fw_ver;
	enum csme_failure_reason rv;
	size_t size;

	void *subpart_cbfs_rw = cbfs_map(name, &size);
	if (!subpart_cbfs_rw) {
		printk(BIOS_ERR, "cse_lite: Not able to map %s CBFS file\n",
				cse_sub_part_str(type));
		return CSE_LITE_SKU_SUB_PART_BLOB_ACCESS_ERR;
	}

	cse_sub_part_get_source_fw_version(subpart_cbfs_rw, &source_fw_ver);
	printk(BIOS_INFO, "cse_lite: CBFS %s FW Version: %x.%x.%x.%x\n", cse_sub_part_str(type),
			source_fw_ver.major, source_fw_ver.minor, source_fw_ver.hotfix,
			source_fw_ver.build);

	/* Trigger sub-partition update in CSE RO and CSE RW */
	for (size_t bp = 0; bp < ARRAY_SIZE(cse_regions); bp++) {
		if (cse_sub_part_get_target_rdev(cse_bp_info, &target_rdev, bp, type) != CB_SUCCESS) {
			rv = CSE_LITE_SKU_SUB_PART_ACCESS_ERR;
			goto error_exit;
		}

		if (cse_get_sub_part_fw_version(type, &target_rdev, &target_fw_ver) != CB_SUCCESS) {
			rv = CSE_LITE_SKU_SUB_PART_ACCESS_ERR;
			goto error_exit;
		}

		printk(BIOS_INFO, "cse_lite: %s %s FW Version: %x.%x.%x.%x\n", cse_regions[bp],
				cse_sub_part_str(type), target_fw_ver.major,
				target_fw_ver.minor, target_fw_ver.hotfix, target_fw_ver.build);

		if (!cse_compare_sub_part_version(&target_fw_ver, &source_fw_ver)) {
			printk(BIOS_INFO, "cse_lite: %s %s update is not required\n",
					cse_regions[bp], cse_sub_part_str(type));
			rv = CSE_LITE_SKU_SUB_PART_UPDATE_NOT_REQ;
			continue;
		}

		printk(BIOS_INFO, "CSE %s %s Update initiated\n", GET_BP_STR(bp),
				cse_sub_part_str(type));

		if (cse_prep_for_component_update(cse_bp_info) != CB_SUCCESS) {
			rv = CSE_LITE_SKU_SUB_PART_ACCESS_ERR;
			goto error_exit;
		}

		rv = cse_sub_part_trigger_update(type, bp, subpart_cbfs_rw,
				size, &target_rdev);

		if (rv != CSE_LITE_SKU_PART_UPDATE_SUCCESS)
			goto error_exit;
	}
error_exit:
	cbfs_unmap(subpart_cbfs_rw);
	return rv;
}

static enum csme_failure_reason cse_sub_part_fw_update(const struct cse_bp_info *cse_bp_info)
{
	if (skip_cse_sub_part_update()) {
		printk(BIOS_INFO, "CSE Sub-partition update not required\n");
		return CSE_LITE_SKU_SUB_PART_UPDATE_NOT_REQ;
	}

	enum csme_failure_reason rv;
	rv = cse_sub_part_fw_component_update(IOM_FW, cse_bp_info,
			CONFIG_SOC_INTEL_CSE_IOM_CBFS_NAME);

	handle_cse_sub_part_fw_update_rv(rv);

	rv = cse_sub_part_fw_component_update(NPHY_FW, cse_bp_info,
			CONFIG_SOC_INTEL_CSE_NPHY_CBFS_NAME);

	return handle_cse_sub_part_fw_update_rv(rv);
}

void cse_fw_sync(void)
{
	static struct get_bp_info_rsp cse_bp_info;

	/*
	 * If system is in recovery mode, skip CSE Lite update if CSE sub-partition update
	 * is not enabled and continue to update CSE sub-partitions.
	 */
	if (vboot_recovery_mode_enabled() && !CONFIG(SOC_INTEL_CSE_SUB_PART_UPDATE)) {
		printk(BIOS_DEBUG, "cse_lite: Skip switching to RW in the recovery path\n");
		return;
	}

	/* If CSE SKU type is not Lite, skip enabling CSE Lite SKU */
	if (!cse_is_hfs3_fw_sku_lite()) {
		printk(BIOS_ERR, "cse_lite: Not a CSE Lite SKU\n");
		return;
	}

	if (cse_get_bp_info(&cse_bp_info) != CB_SUCCESS) {
		printk(BIOS_ERR, "cse_lite: Failed to get CSE boot partition info\n");

		 /* If system is in recovery mode, don't trigger recovery again */
		if (!vboot_recovery_mode_enabled()) {
			cse_trigger_vboot_recovery(CSE_COMMUNICATION_ERROR);
		} else {
			printk(BIOS_ERR, "cse_lite: System is already in Recovery Mode, "
					"so no action\n");
			return;
		}
	}

	/*
	 * If system is in recovery mode, CSE Lite update has to be skipped but CSE
	 * sub-partitions like NPHY and IOM have to to be updated. If CSE sub-parition update
	 * fails during recovery, just continue to boot.
	 */
	if (CONFIG(SOC_INTEL_CSE_SUB_PART_UPDATE) && vboot_recovery_mode_enabled()) {
		if (cse_sub_part_fw_update(&cse_bp_info.bp_info) ==
				CSE_LITE_SKU_PART_UPDATE_SUCCESS) {
			cse_board_reset();
			do_global_reset();
			die("ERROR: GLOBAL RESET Failed to reset the system\n");
		}

		return;
	}

	if (cse_fix_data_failure_err(&cse_bp_info.bp_info) != CB_SUCCESS)
		cse_trigger_vboot_recovery(CSE_LITE_SKU_DATA_WIPE_ERROR);

	/*
	 * cse firmware update is skipped if SOC_INTEL_CSE_RW_UPDATE is not defined and
	 * runtime debug control flag is not enabled. The driver triggers recovery if CSE CBFS
	 * RW metadata or CSE CBFS RW blob is not available.
	 */
	if (is_cse_fw_update_enabled()) {
		uint8_t rv;
		rv = cse_fw_update(&cse_bp_info.bp_info);
		if (rv)
			cse_trigger_vboot_recovery(rv);
	}

	if (CONFIG(SOC_INTEL_CSE_SUB_PART_UPDATE))
		cse_sub_part_fw_update(&cse_bp_info.bp_info);

	if (!cse_is_rw_bp_status_valid(&cse_bp_info.bp_info))
		cse_trigger_vboot_recovery(CSE_LITE_SKU_RW_JUMP_ERROR);

	if (cse_boot_to_rw(&cse_bp_info.bp_info) != CB_SUCCESS) {
		printk(BIOS_ERR, "cse_lite: Failed to switch to RW\n");
		cse_trigger_vboot_recovery(CSE_LITE_SKU_RW_SWITCH_ERROR);
	}
}

static void ramstage_cse_fw_sync(void *unused)
{
	bool s3wake;
	s3wake = acpi_get_sleep_type() == ACPI_S3;

	if (CONFIG(SOC_INTEL_CSE_LITE_SYNC_IN_RAMSTAGE) && !s3wake) {
		timestamp_add_now(TS_CSE_FW_SYNC_START);
		cse_fw_sync();
		timestamp_add_now(TS_CSE_FW_SYNC_END);
	}
}

BOOT_STATE_INIT_ENTRY(BS_PRE_DEVICE, BS_ON_EXIT, ramstage_cse_fw_sync, NULL);
