/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <bootstate.h>
#include <cbfs.h>
#include <commonlib/region.h>
#include <console/console.h>
#include <cpu/cpu.h>
#include <crc_byte.h>
#include <elog.h>
#include <fmap.h>
#include <intelbasecode/debug_feature.h>
#include <intelblocks/cse.h>
#include <intelblocks/cse_layout.h>
#include <intelblocks/cse_lite.h>
#include <intelblocks/spi.h>
#include <security/vboot/misc.h>
#include <security/vboot/vboot_common.h>
#include <soc/intel/common/reset.h>
#include <timestamp.h>

#include "cse_lite_cmos.h"

static struct get_bp_info_rsp cse_bp_info_rsp;

enum cse_fw_state {
	/* The CMOS and CBMEM have the current fw version. */
	CSE_FW_WARM_BOOT,

	/* The CMOS has the current fw version, and the CBMEM is wiped out. */
	CSE_FW_COLD_BOOT,

	/* The CMOS and CBMEM are not initialized or not same as running firmware version.*/
	CSE_FW_INVALID,
};

static const char * const cse_regions[] = {"RO", "RW"};

static struct cse_specific_info cse_info;

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


static const struct cse_bp_info *cse_get_bp_info_from_rsp(void)
{
	return &cse_bp_info_rsp.bp_info;
}

static uint8_t cse_get_current_bp(void)
{
	const struct cse_bp_info *cse_bp_info = cse_get_bp_info_from_rsp();
	return cse_bp_info->current_bp;
}

static const struct cse_bp_entry *cse_get_bp_entry(enum boot_partition_id bp)
{
	const struct cse_bp_info *cse_bp_info = cse_get_bp_info_from_rsp();
	return &cse_bp_info->bp_entries[bp];
}

static bool is_cse_fpt_info_valid(const struct cse_specific_info *info)
{
	uint32_t crc = ~CRC(info, offsetof(struct cse_specific_info, crc), crc32_byte);

	/*
	 * Authenticate the CBMEM persistent data.
	 *
	 * The underlying assumption is that an event (i.e., CSE upgrade/downgrade) which
	 * could change the values stored in this region has to also trigger the global
	 * reset. Hence, CBMEM persistent data won't be available any time after such
	 * event (global reset or cold reset) being initiated.
	 *
	 * During warm boot scenarios CBMEM contents remain persistent hence, we don't
	 * want to override the existing data in CBMEM to avoid any additional boot latency.
	 */
	if (info->crc != crc)
		return false;

	return true;
}

static void store_cse_info_crc(struct cse_specific_info *info)
{
	info->crc = ~CRC(info, offsetof(struct cse_specific_info, crc), crc32_byte);
}

static enum cse_fw_state get_cse_state(const struct fw_version *cur_cse_fw_ver,
	struct fw_version *cmos_cse_fw_ver, const struct fw_version *cbmem_cse_fw_ver)
{
	enum cse_fw_state state = CSE_FW_WARM_BOOT;
	size_t size = sizeof(struct fw_version);
	/*
	 * Compare if stored CSE version (from the previous boot) is same as current
	 * running CSE version.
	 */
	if (memcmp(cmos_cse_fw_ver, cur_cse_fw_ver, size)) {
		/*
		 * CMOS CSE versioin is invalid, possibly two scenarios
		 * 1.  CSE FW update
		 * 2.  First boot
		 */
		state = CSE_FW_INVALID;
	} else {
		/*
		 * Check if current running CSE version is same as previous stored CSE
		 * version aka CBMEM region is still valid.
		 */
		if (memcmp(cbmem_cse_fw_ver, cur_cse_fw_ver, size))
			state = CSE_FW_COLD_BOOT;
	}
	return state;
}

/*
 * Helper function that stores current CSE firmware version to CBMEM memory,
 * except during recovery mode.
 */
static void cse_store_rw_fw_version(void)
{
	const struct cse_bp_entry *cse_bp;
	cse_bp = cse_get_bp_entry(RW);

	if (vboot_recovery_mode_enabled())
		return;

	if (CONFIG(SOC_INTEL_CSE_LITE_SYNC_IN_ROMSTAGE)) {
		/* update current CSE version and return */
		memcpy(&(cse_info.cse_fwp_version.cur_cse_fw_version),
		 &(cse_bp->fw_ver), sizeof(struct fw_version));
		return;
	}

	struct cse_specific_info *cse_info_in_cbmem = cbmem_add(CBMEM_ID_CSE_INFO,
		 sizeof(*cse_info_in_cbmem));
	if (!cse_info_in_cbmem)
		return;

	/* Avoid CBMEM update if CBMEM already has persistent data */
	if (is_cse_fpt_info_valid(cse_info_in_cbmem))
		return;

	struct cse_specific_info cse_info_in_cmos;
	cmos_read_fw_partition_info(&cse_info_in_cmos);

	/* Get current cse firmware state */
	enum cse_fw_state fw_state = get_cse_state(&(cse_bp->fw_ver),
		 &(cse_info_in_cmos.cse_fwp_version.cur_cse_fw_version),
		 &(cse_info_in_cbmem->cse_fwp_version.cur_cse_fw_version));

	/* Reset CBMEM data and update current CSE version */
	memset(cse_info_in_cbmem, 0, sizeof(*cse_info_in_cbmem));
	memcpy(&(cse_info_in_cbmem->cse_fwp_version.cur_cse_fw_version),
		 &(cse_bp->fw_ver), sizeof(struct fw_version));

	/* Update the CRC */
	store_cse_info_crc(cse_info_in_cbmem);

	if (fw_state == CSE_FW_INVALID) {
		/*
		 * Current CMOS data is outdated, which could be due to CSE update or
		 * rollback, hence, need to update CMOS with current CSE FPT versions.
		 */
		cmos_write_fw_partition_info(cse_info_in_cbmem);
	}
}

#if CONFIG(SOC_INTEL_CSE_LITE_SYNC_IN_ROMSTAGE)
/* Function to copy PRERAM CSE specific info to pertinent CBMEM. */
static void preram_cse_info_sync_to_cbmem(int is_recovery)
{
	if (CONFIG(SOC_INTEL_CSE_LITE_SYNC_BY_PAYLOAD))
		return;

	if (vboot_recovery_mode_enabled() || !CONFIG(SOC_INTEL_STORE_CSE_FW_VERSION))
		return;

	struct cse_specific_info *cse_info_in_cbmem = cbmem_add(CBMEM_ID_CSE_INFO,
		 sizeof(*cse_info_in_cbmem));
	if (!cse_info_in_cbmem)
		return;

	/* Warm Reboot: Avoid sync into CBMEM if CBMEM already has persistent data */
	if (is_cse_fpt_info_valid(cse_info_in_cbmem))
		return;

	/* Update CBMEM with PRERAM CSE specific info and update the CRC */
	memcpy(cse_info_in_cbmem, &cse_info, sizeof(struct cse_specific_info));
	store_cse_info_crc(cse_info_in_cbmem);

	struct cse_specific_info cse_info_in_cmos;
	cmos_read_fw_partition_info(&cse_info_in_cmos);

	if (!memcmp(&(cse_info_in_cmos.cse_fwp_version.cur_cse_fw_version),
		 &(cse_info_in_cbmem->cse_fwp_version.cur_cse_fw_version),
		 sizeof(struct fw_version))) {
		/* Cold Reboot: Avoid sync into CMOS if CMOS already has persistent data */
		if (is_cse_fpt_info_valid(&cse_info_in_cmos))
			return;
	}

	/*
	 * Current CMOS data is outdated, which could be due to CSE update or
	 * rollback, hence, need to update CMOS with current CSE FPT versions.
	 */
	cmos_write_fw_partition_info(cse_info_in_cbmem);
}

CBMEM_CREATION_HOOK(preram_cse_info_sync_to_cbmem);
#endif

static void cse_print_boot_partition_info(void)
{
	const struct cse_bp_entry *cse_bp;
	const struct cse_bp_info *cse_bp_info = cse_get_bp_info_from_rsp();

	printk(BIOS_DEBUG, "cse_lite: Number of partitions = %d\n",
			cse_bp_info->total_number_of_bp);
	printk(BIOS_DEBUG, "cse_lite: Current partition = %s\n",
			GET_BP_STR(cse_bp_info->current_bp));
	printk(BIOS_DEBUG, "cse_lite: Next partition = %s\n", GET_BP_STR(cse_bp_info->next_bp));
	printk(BIOS_DEBUG, "cse_lite: Flags = 0x%x\n", cse_bp_info->flags);

	/* Log version info of RO & RW partitions */
	cse_bp = cse_get_bp_entry(RO);
	if (cse_bp->status == BP_STATUS_SUCCESS)
		printk(BIOS_DEBUG, "cse_lite: %s version = %d.%d.%d.%d (Start=0x%x, End=0x%x)\n",
			GET_BP_STR(RO), cse_bp->fw_ver.major, cse_bp->fw_ver.minor,
			cse_bp->fw_ver.hotfix, cse_bp->fw_ver.build,
			cse_bp->start_offset, cse_bp->end_offset);
	else
		printk(BIOS_ERR, "cse_lite: %s status=0x%x\n", GET_BP_STR(RO), cse_bp->status);

	cse_bp = cse_get_bp_entry(RW);
	if (cse_bp->status == BP_STATUS_SUCCESS)
		printk(BIOS_DEBUG, "cse_lite: %s version = %d.%d.%d.%d (Start=0x%x, End=0x%x)\n",
			GET_BP_STR(RW), cse_bp->fw_ver.major, cse_bp->fw_ver.minor,
			cse_bp->fw_ver.hotfix, cse_bp->fw_ver.build,
			cse_bp->start_offset, cse_bp->end_offset);
	else
		printk(BIOS_ERR, "cse_lite: %s status=0x%x\n", GET_BP_STR(RW), cse_bp->status);
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

static struct get_bp_info_rsp *sync_cse_bp_info_to_cbmem(void)
{
	struct get_bp_info_rsp *cse_bp_info_in_cbmem = cbmem_find(CBMEM_ID_CSE_BP_INFO);

	if (cse_bp_info_in_cbmem != NULL)
		return cse_bp_info_in_cbmem;

	cse_bp_info_in_cbmem = cbmem_add(CBMEM_ID_CSE_BP_INFO,
		sizeof(struct get_bp_info_rsp));

	if (!cse_bp_info_in_cbmem) {
		printk(BIOS_ERR, "Unable to store Boot Parition Info in cbmem\n");
		return NULL;
	}

	/* Copy the CSE Boot Partition Info command response to cbmem */
	memcpy(cse_bp_info_in_cbmem, &cse_bp_info_rsp, sizeof(struct get_bp_info_rsp));

	return cse_bp_info_in_cbmem;
}

static bool is_cse_bp_info_valid(struct get_bp_info_rsp *bp_info_rsp)
{
	/*
	 * In case the cse_bp_info_rsp header group ID, command is incorrect or is_resp is 0,
	 * then return false to indicate cse_bp_info is not valid.
	 */
	return (bp_info_rsp->hdr.group_id != MKHI_GROUP_ID_BUP_COMMON ||
		bp_info_rsp->hdr.command != MKHI_BUP_COMMON_GET_BOOT_PARTITION_INFO ||
		!bp_info_rsp->hdr.is_resp) ? false : true;
}

static enum cb_err cse_get_bp_info(void)
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

	/*
	 * If SOC_INTEL_CSE_LITE_SYNC_IN_RAMSTAGE config is selected and memory has been
	 * initialized, check if there is cse bp info response stored in cbmem. Once the data
	 * is validated, copy it to the global cse_bp_info_rsp so that it can be used by other
	 * functions. In case, data is not available in cbmem or invalid, continue to send the
	 * GET_BOOT_PARTITION_INFO command, else return.
	 */
	if (CONFIG(SOC_INTEL_CSE_LITE_SYNC_IN_RAMSTAGE) && cbmem_online()) {
		struct get_bp_info_rsp *cse_bp_info_in_cbmem = sync_cse_bp_info_to_cbmem();
		if (cse_bp_info_in_cbmem) {
			if (is_cse_bp_info_valid(cse_bp_info_in_cbmem)) {
				memcpy(&cse_bp_info_rsp, cse_bp_info_in_cbmem,
					sizeof(struct get_bp_info_rsp));
				return CB_SUCCESS;
			}
		}
	} else {
		/*
		 * If SOC_INTEL_CSE_LITE_SYNC_IN_ROMSTAGE config is selected, check if the
		 * global cse bp info response stored in global cse_bp_info_rsp is valid.
		 * In case, it is not valid, continue to send the GET_BOOT_PARTITION_INFO
		 * command, else return.
		 */
		if (is_cse_bp_info_valid(&cse_bp_info_rsp))
			return CB_SUCCESS;
	}

	if (!cse_is_bp_cmd_info_possible()) {
		printk(BIOS_ERR, "cse_lite: CSE does not meet prerequisites\n");
		return CB_ERR;
	}

	size_t resp_size = sizeof(struct get_bp_info_rsp);

	if (heci_send_receive(&info_req, sizeof(info_req), &cse_bp_info_rsp, &resp_size,
									HECI_MKHI_ADDR)) {
		printk(BIOS_ERR, "cse_lite: Could not get partition info\n");
		return CB_ERR;
	}

	if (cse_bp_info_rsp.hdr.result) {
		printk(BIOS_ERR, "cse_lite: Get partition info resp failed: %d\n",
				cse_bp_info_rsp.hdr.result);
		return CB_ERR;
	}

	cse_print_boot_partition_info();
	return CB_SUCCESS;
}

void cse_fill_bp_info(void)
{
	if (CONFIG(SOC_INTEL_CSE_LITE_SYNC_BY_PAYLOAD))
		return;

	if (vboot_recovery_mode_enabled())
		return;

	if (cse_get_bp_info() != CB_SUCCESS)
		cse_trigger_vboot_recovery(CSE_COMMUNICATION_ERROR);
}

/* Function to copy PRERAM CSE BP info to pertinent CBMEM. */
static void preram_cse_bp_info_sync_to_cbmem(int is_recovery)
{
	if (CONFIG(SOC_INTEL_CSE_LITE_SYNC_BY_PAYLOAD))
		return;

	if (vboot_recovery_mode_enabled())
		return;

	sync_cse_bp_info_to_cbmem();
}

CBMEM_CREATION_HOOK(preram_cse_bp_info_sync_to_cbmem);

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

static enum cb_err cse_data_clear_request(void)
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
			cse_get_current_bp() != RO) {
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

static enum cb_err cse_boot_to_rw(void)
{
	if (cse_get_current_bp() == RW)
		return CB_SUCCESS;

	return cse_set_and_boot_from_next_bp(RW);
}

/* Check if CSE RW data partition is valid or not */
static bool cse_is_rw_dp_valid(void)
{
	const struct cse_bp_entry *rw_bp;

	rw_bp = cse_get_bp_entry(RW);
	return rw_bp->status != BP_STATUS_DATA_FAILURE;
}

/*
 * It returns true if RW partition doesn't indicate BP_STATUS_DATA_FAILURE
 * otherwise false if any operation fails.
 */
static enum cb_err cse_fix_data_failure_err(void)
{
	/*
	 * If RW partition status indicates BP_STATUS_DATA_FAILURE,
	 *  - Send DATA CLEAR HECI command to CSE
	 *  - Send SET BOOT PARTITION INFO(RW) command to set CSE's next partition
	 *  - Issue GLOBAL RESET HECI command.
	 */
	if (cse_is_rw_dp_valid())
		return CB_SUCCESS;

	if (cse_data_clear_request() != CB_SUCCESS)
		return CB_ERR;

	return cse_boot_to_rw();
}

static const struct fw_version *cse_get_bp_entry_version(enum boot_partition_id bp)
{
	const struct cse_bp_entry *cse_bp;

	cse_bp = cse_get_bp_entry(bp);
	return &cse_bp->fw_ver;
}

static const struct fw_version *cse_get_rw_version(void)
{
	return cse_get_bp_entry_version(RW);
}

static void cse_get_bp_entry_range(enum boot_partition_id bp, uint32_t *start_offset,
		uint32_t *end_offset)
{
	const struct cse_bp_entry *cse_bp;

	cse_bp = cse_get_bp_entry(bp);

	if (start_offset)
		*start_offset = cse_bp->start_offset;

	if (end_offset)
		*end_offset = cse_bp->end_offset;
}

static bool cse_is_rw_bp_status_valid(void)
{
	const struct cse_bp_entry *rw_bp;

	rw_bp = cse_get_bp_entry(RW);

	if (rw_bp->status == BP_STATUS_PARTITION_NOT_PRESENT ||
			rw_bp->status == BP_STATUS_GENERAL_FAILURE) {
		printk(BIOS_ERR, "cse_lite: RW BP (status:%u) is not valid\n", rw_bp->status);
		return false;
	}
	return true;
}

static enum cb_err cse_boot_to_ro(void)
{
	if (cse_get_current_bp() == RO)
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

static enum cb_err cse_get_target_rdev(struct region_device *target_rdev)
{
	struct region_device cse_region_rdev;
	size_t size;
	uint32_t start_offset;
	uint32_t end_offset;

	if (cse_get_rw_rdev(&cse_region_rdev) != CB_SUCCESS)
		return CB_ERR;

	cse_get_bp_entry_range(RW, &start_offset, &end_offset);
	size = end_offset + 1 - start_offset;

	if (rdev_chain(target_rdev, &cse_region_rdev, start_offset, size))
		return CB_ERR;

	printk(BIOS_DEBUG, "cse_lite: CSE RW partition: offset = 0x%x, size = 0x%x\n",
			(uint32_t)start_offset, (uint32_t)size);

	return CB_SUCCESS;
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

static enum cb_err get_cse_ver_from_cbfs(struct fw_version *cbfs_rw_version)
{
	char *version_str, *cbfs_ptr;
	size_t size;

	if (cbfs_rw_version == NULL)
		return CB_ERR;

	cbfs_ptr = cbfs_map(CONFIG_SOC_INTEL_CSE_RW_VERSION_CBFS_NAME, &size);
	version_str = cbfs_ptr;
	if (!version_str) {
		printk(BIOS_ERR, "cse_lite: Failed to get %s\n",
			  CONFIG_SOC_INTEL_CSE_RW_VERSION_CBFS_NAME);
		return CB_ERR;
	}

	if (!read_ver_field(version_str, &cbfs_ptr, size, &cbfs_rw_version->major) ||
	    !read_ver_field(version_str, &cbfs_ptr, size, &cbfs_rw_version->minor) ||
		!read_ver_field(version_str, &cbfs_ptr, size, &cbfs_rw_version->hotfix) ||
		!read_ver_field(version_str, &cbfs_ptr, size, &cbfs_rw_version->build)) {
		cbfs_unmap(version_str);
		return CB_ERR;
	}

	cbfs_unmap(version_str);
	return CB_SUCCESS;
}

static bool is_cse_sync_enforced(void)
{
	/*
	 * Force test CSE firmware update scenario if below conditions are being met:
	 *  - VB2_GBB_FLAG_FORCE_CSE_SYNC flag is set
	 *  - CSE FW is in RO
	 */
	struct vb2_context *ctx = vboot_get_context();
	if ((vb2api_gbb_get_flags(ctx) & VB2_GBB_FLAG_FORCE_CSE_SYNC) &&
		 cse_get_current_bp() == RO) {
		return true;
	}
	return false;
}

static enum cse_update_status cse_check_update_status(struct region_device *target_rdev)
{
	int ret;
	struct fw_version cbfs_rw_version;

	if (!cse_is_rw_bp_sign_valid(target_rdev))
		return CSE_UPDATE_CORRUPTED;

	if (get_cse_ver_from_cbfs(&cbfs_rw_version) == CB_ERR)
		return CSE_UPDATE_METADATA_ERROR;

	printk(BIOS_DEBUG, "cse_lite: CSE CBFS RW version : %d.%d.%d.%d\n",
			cbfs_rw_version.major,
			cbfs_rw_version.minor,
			cbfs_rw_version.hotfix,
			cbfs_rw_version.build);

	ret = cse_compare_sub_part_version(&cbfs_rw_version, cse_get_rw_version());
	if (ret == 0) {
		if (is_cse_sync_enforced()) {
			printk(BIOS_WARNING, "Force CSE Firmware upgrade for Autotest\n");
			return CSE_UPDATE_UPGRADE;
		}
		return CSE_UPDATE_NOT_REQUIRED;
	} else {
		if (ret < 0)
			return CSE_UPDATE_DOWNGRADE;
		else
			return CSE_UPDATE_UPGRADE;
	}
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

	if (CONFIG(SOC_INTEL_CSE_LITE_SYNC_BY_PAYLOAD))
		return false;

	if (CONFIG(SOC_INTEL_COMMON_BASECODE_DEBUG_FEATURE))
		return !is_debug_cse_fw_update_disable();

	return true;
}

static enum csme_failure_reason cse_update_rw(const void *cse_cbfs_rw, const size_t cse_blob_sz,
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

static enum cb_err cse_prep_for_rw_update(enum cse_update_status status)
{
	if (status == CSE_UPDATE_CORRUPTED)
		elog_add_event(ELOG_TYPE_PSR_DATA_LOST);
	/*
	 * To set CSE's operation mode to HMRFPO mode:
	 * 1. Ensure CSE to boot from RO(BP1)
	 * 2. Send HMRFPO_ENABLE command to CSE
	 */
	if (cse_boot_to_ro() != CB_SUCCESS)
		return CB_ERR;

	if ((status == CSE_UPDATE_DOWNGRADE) || (status == CSE_UPDATE_CORRUPTED)) {
		/* Reset the PSR backup command status in CMOS */
		if (CONFIG(SOC_INTEL_CSE_LITE_PSR))
			update_psr_backup_status(PSR_BACKUP_PENDING);

		if (cse_data_clear_request() != CB_SUCCESS) {
			printk(BIOS_ERR, "cse_lite: CSE data clear failed!\n");
			return CB_ERR;
		}
	}

	return cse_hmrfpo_enable();
}

static enum csme_failure_reason cse_trigger_fw_update(enum cse_update_status status,
		struct region_device *target_rdev)
{
	enum csme_failure_reason rv;
	void *cse_cbfs_rw = NULL;
	size_t size;

	if (CONFIG(SOC_INTEL_CSE_LITE_COMPRESS_ME_RW)) {
		cse_cbfs_rw = cbfs_cbmem_alloc(CONFIG_SOC_INTEL_CSE_RW_CBFS_NAME,
			CBMEM_ID_CSE_UPDATE, &size);
	} else {
		cse_cbfs_rw = cbfs_map(CONFIG_SOC_INTEL_CSE_RW_CBFS_NAME, &size);
	}

	if (!cse_cbfs_rw) {
		printk(BIOS_ERR, "cse_lite: CSE CBFS RW blob could not be mapped\n");
		return CSE_LITE_SKU_RW_BLOB_NOT_FOUND;
	}

	if (cse_prep_for_rw_update(status) != CB_SUCCESS) {
		rv = CSE_COMMUNICATION_ERROR;
		goto error_exit;
	}

	cse_fw_update_misc_oper();
	rv = cse_update_rw(cse_cbfs_rw, size, target_rdev);

error_exit:
	cbfs_unmap(cse_cbfs_rw);
	return rv;
}

static bool is_psr_data_backed_up(void)
{
	/* Track PSR backup status in CMOS */
	return (get_psr_backup_status() == PSR_BACKUP_DONE);
}

static bool is_psr_supported(void)
{
	uint32_t feature_status;

	/*
	 * Check if SoC has support for PSR feature typically PSR feature
	 * is only supported by vpro SKU
	 *
	 */
	if (cse_get_fw_feature_state(&feature_status) != CB_SUCCESS) {
		printk(BIOS_ERR, "cse_get_fw_feature_state command failed !\n");
		return false;
	}

	if (!(feature_status & ME_FW_FEATURE_PSR)) {
		printk(BIOS_DEBUG, "PSR is not supported in this SKU !\n");
		return false;
	}

	return true;
}

/*
 * PSR data needs to be backed up prior to downgrade. So switch the CSE boot mode to RW, send
 * PSR back-up command to CSE and update the PSR back-up state in CMOS.
 */
static void backup_psr_data(void)
{
	printk(BIOS_DEBUG, "cse_lite: Initiate PSR data backup flow\n");
	/* Switch CSE to RW to send PSR_HECI_FW_DOWNGRADE_BACKUP command */
	if (cse_boot_to_rw() != CB_SUCCESS) {
		elog_add_event(ELOG_TYPE_PSR_DATA_LOST);
		goto update_and_exit;
	}
	/*
	 * The function to check for PSR feature support can only be called after
	 * switching to RW partition. The command MKHI_FWCAPS_GET_FW_FEATURE_STATE
	 * that gives feature state is supported by a process that is loaded only
	 * when CSE boots from RW.
	 *
	 */
	if (!is_psr_supported())
		goto update_and_exit;

	/*
	 * Prerequisites:
	 * 1) HFSTS1 Current Working State is Normal
	 * 2) HFSTS1 Current Operation Mode is Normal
	 */
	if (!cse_is_hfs1_cws_normal() || !cse_is_hfs1_com_normal()) {
		printk(BIOS_DEBUG, "cse_lite: PSR_HECI_FW_DOWNGRADE_BACKUP command "
		       "prerequisites not met!\n");
		elog_add_event(ELOG_TYPE_PSR_DATA_LOST);
		goto update_and_exit;
	}

	/* Send PSR_HECI_FW_DOWNGRADE_BACKUP command */
	struct psr_heci_fw_downgrade_backup_req {
		struct psr_heci_header header;
	} __packed;

	struct psr_heci_fw_downgrade_backup_req req = {
		.header.command = PSR_HECI_FW_DOWNGRADE_BACKUP,
	};

	struct psr_heci_fw_downgrade_backup_res {
		struct psr_heci_header header;
		uint32_t status;
	} __packed;

	struct psr_heci_fw_downgrade_backup_res backup_psr_resp;
	size_t resp_size = sizeof(backup_psr_resp);

	printk(BIOS_DEBUG, "cse_lite: Send PSR_HECI_FW_DOWNGRADE_BACKUP command\n");
	if (heci_send_receive(&req, sizeof(req),
		&backup_psr_resp, &resp_size, HECI_PSR_ADDR)) {
		printk(BIOS_ERR, "cse_lite: could not backup PSR data\n");
		elog_add_event_byte(ELOG_TYPE_PSR_DATA_BACKUP, ELOG_PSR_DATA_BACKUP_FAILED);
	} else {
		if (backup_psr_resp.status != PSR_STATUS_SUCCESS) {
			printk(BIOS_ERR, "cse_lite: PSR_HECI_FW_DOWNGRADE_BACKUP command "
			       "returned %u\n", backup_psr_resp.status);
			elog_add_event_byte(ELOG_TYPE_PSR_DATA_BACKUP,
						ELOG_PSR_DATA_BACKUP_FAILED);
		} else {
			elog_add_event_byte(ELOG_TYPE_PSR_DATA_BACKUP,
						ELOG_PSR_DATA_BACKUP_SUCCESS);
		}
	}

update_and_exit:
	/*
	 * An attempt to send PSR back-up command has been made. Update this info in CMOS and
	 * send success once backup_psr_data() has been called. We do not want to put the system
	 * into recovery for PSR data backup command pre-requisites not being met.
	 * We cannot do much if CSE fails to backup the PSR data, except create an event log.
	 */
	update_psr_backup_status(PSR_BACKUP_DONE);
}

static void initiate_psr_data_backup(void)
{
	if (is_psr_data_backed_up())
		return;

	backup_psr_data();
}

/*
 * Check if a CSE Firmware update is required
 * returns true if an update is required, false otherwise
 */
bool is_cse_fw_update_required(void)
{
	struct fw_version cbfs_rw_version;

	if (!is_cse_fw_update_enabled())
		return false;

	/*
	 * First, check if cse_bp_info_rsp global structure is populated.
	 * If not, it implies that cse_fill_bp_info() function is not called.
	 */
	if (!is_cse_bp_info_valid(&cse_bp_info_rsp))
		return false;

	if (get_cse_ver_from_cbfs(&cbfs_rw_version) == CB_ERR)
		return false;

	/* Check if CSE sync is enforced */
	if (is_cse_sync_enforced()) {
		return true;
	}
	return !!cse_compare_sub_part_version(&cbfs_rw_version, cse_get_rw_version());
}

bool is_cse_boot_to_rw(void)
{
	if (cse_get_bp_info() != CB_SUCCESS) {
		printk(BIOS_ERR, "cse_lite: Failed to get CSE boot partition info\n");
		return false;
	}

	if (cse_get_current_bp() == RW)
		return true;

	return false;
}

static uint8_t cse_fw_update(void)
{
	struct region_device target_rdev;
	enum cse_update_status status;

	if (cse_get_target_rdev(&target_rdev) != CB_SUCCESS) {
		printk(BIOS_ERR, "cse_lite: Failed to get CSE RW Partition\n");
		return CSE_LITE_SKU_RW_ACCESS_ERROR;
	}

	status = cse_check_update_status(&target_rdev);
	if (status == CSE_UPDATE_NOT_REQUIRED)
		return CSE_NO_ERROR;
	if (status == CSE_UPDATE_METADATA_ERROR)
		return CSE_LITE_SKU_RW_METADATA_NOT_FOUND;
	if (CONFIG(SOC_INTEL_CSE_LITE_PSR) && status == CSE_UPDATE_DOWNGRADE)
		initiate_psr_data_backup();

	printk(BIOS_DEBUG, "cse_lite: CSE RW update is initiated\n");
	return cse_trigger_fw_update(status, &target_rdev);
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

static enum cb_err cse_locate_area_as_rdev_rw(size_t bp, struct region_device  *cse_rdev)
{
	struct region_device cse_region_rdev;
	uint32_t size;
	uint32_t start_offset;
	uint32_t end_offset;

	if (cse_get_rw_rdev(&cse_region_rdev) != CB_SUCCESS)
		return CB_ERR;

	if (!strcmp(cse_regions[bp], "RO"))
		cse_get_bp_entry_range(RO, &start_offset, &end_offset);
	else
		cse_get_bp_entry_range(RW, &start_offset, &end_offset);

	size = end_offset + 1 - start_offset;

	if (rdev_chain(cse_rdev, &cse_region_rdev, start_offset, size))
		return CB_ERR;

	printk(BIOS_DEBUG, "cse_lite: CSE %s  partition: offset = 0x%x, size = 0x%x\n",
			cse_regions[bp], start_offset, size);
	return CB_SUCCESS;
}

static enum cb_err cse_sub_part_get_target_rdev(struct region_device *target_rdev, size_t bp,
						enum bpdt_entry_type type)
{
	struct bpdt_header bpdt_hdr;
	struct region_device cse_rdev;
	struct bpdt_entry bpdt_entries[MAX_SUBPARTS];
	uint8_t i;

	if (cse_locate_area_as_rdev_rw(bp, &cse_rdev) != CB_SUCCESS) {
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

static enum cb_err cse_prep_for_component_update(void)
{
	/*
	 * To set CSE's operation mode to HMRFPO mode:
	 * 1. Ensure CSE to boot from RO(BP1)
	 * 2. Send HMRFPO_ENABLE command to CSE
	 */
	if (cse_boot_to_ro() != CB_SUCCESS)
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
		const char *name)
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
		if (cse_sub_part_get_target_rdev(&target_rdev, bp, type) != CB_SUCCESS) {
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

		if (cse_prep_for_component_update() != CB_SUCCESS) {
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

static enum csme_failure_reason cse_sub_part_fw_update(void)
{
	if (skip_cse_sub_part_update()) {
		printk(BIOS_INFO, "CSE Sub-partition update not required\n");
		return CSE_LITE_SKU_SUB_PART_UPDATE_NOT_REQ;
	}

	enum csme_failure_reason rv;
	rv = cse_sub_part_fw_component_update(IOM_FW, CONFIG_SOC_INTEL_CSE_IOM_CBFS_NAME);

	handle_cse_sub_part_fw_update_rv(rv);

	rv = cse_sub_part_fw_component_update(NPHY_FW, CONFIG_SOC_INTEL_CSE_NPHY_CBFS_NAME);

	return handle_cse_sub_part_fw_update_rv(rv);
}

static void do_cse_fw_sync(void)
{
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

	if (cse_get_bp_info() != CB_SUCCESS) {
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

	/* Store the CSE RW Firmware Version into CBMEM */
	if (CONFIG(SOC_INTEL_STORE_CSE_FW_VERSION))
		cse_store_rw_fw_version();

	/*
	 * If system is in recovery mode, CSE Lite update has to be skipped but CSE
	 * sub-partitions like NPHY and IOM have to be updated. If CSE sub-partition update
	 * fails during recovery, just continue to boot.
	 */
	if (CONFIG(SOC_INTEL_CSE_SUB_PART_UPDATE) && vboot_recovery_mode_enabled()) {
		if (cse_sub_part_fw_update() == CSE_LITE_SKU_PART_UPDATE_SUCCESS) {
			cse_board_reset();
			do_global_reset();
			die("ERROR: GLOBAL RESET Failed to reset the system\n");
		}

		return;
	}

	if (cse_fix_data_failure_err() != CB_SUCCESS)
		cse_trigger_vboot_recovery(CSE_LITE_SKU_DATA_WIPE_ERROR);

	/*
	 * CSE firmware update is skipped if SOC_INTEL_CSE_RW_UPDATE is not defined and
	 * runtime debug control flag is not enabled. The driver triggers recovery if CSE CBFS
	 * RW metadata or CSE CBFS RW blob is not available.
	 */
	if (is_cse_fw_update_enabled()) {
		uint8_t rv;
		rv = cse_fw_update();
		if (rv)
			cse_trigger_vboot_recovery(rv);
	}

	if (CONFIG(SOC_INTEL_CSE_SUB_PART_UPDATE))
		cse_sub_part_fw_update();

	if (!cse_is_rw_bp_status_valid())
		cse_trigger_vboot_recovery(CSE_LITE_SKU_RW_JUMP_ERROR);

	if (cse_boot_to_rw() != CB_SUCCESS) {
		printk(BIOS_ERR, "cse_lite: Failed to switch to RW\n");
		cse_trigger_vboot_recovery(CSE_LITE_SKU_RW_SWITCH_ERROR);
	}
}

void cse_fw_sync(void)
{
	if (CONFIG(SOC_INTEL_CSE_LITE_SYNC_BY_PAYLOAD))
		return;

	timestamp_add_now(TS_CSE_FW_SYNC_START);
	do_cse_fw_sync();
	timestamp_add_now(TS_CSE_FW_SYNC_END);
}

static enum cb_err send_get_fpt_partition_info_cmd(enum fpt_partition_id id,
	struct fw_version_resp *resp)
{
	enum cse_tx_rx_status ret;
	struct fw_version_msg {
		struct mkhi_hdr hdr;
		enum fpt_partition_id partition_id;
	} __packed msg = {
		.hdr = {
			.group_id = MKHI_GROUP_ID_GEN,
			.command = GEN_GET_IMAGE_FW_VERSION,
		},
		.partition_id = id,
	};

	/*
	 * Prerequisites:
	 * 1) HFSTS1 CWS is Normal
	 * 2) HFSTS1 COM is Normal
	 * 3) Only sent after DID (accomplished by compiling this into ramstage)
	 */

	if (cse_is_hfs1_com_soft_temp_disable() || !cse_is_hfs1_cws_normal() ||
		!cse_is_hfs1_com_normal()) {
		printk(BIOS_ERR,
			"HECI: Prerequisites not met for Get Image Firmware Version command\n");
		return CB_ERR;
	}

	size_t resp_size = sizeof(struct fw_version_resp);
	ret = heci_send_receive(&msg, sizeof(msg), resp, &resp_size, HECI_MKHI_ADDR);

	if (ret || resp->hdr.result) {
		printk(BIOS_ERR, "CSE: Failed to get partition information for %d: 0x%x\n",
			id, resp->hdr.result);
		return CB_ERR;
	}

	return CB_SUCCESS;
}

static enum cb_err cse_get_fpt_partition_info(enum fpt_partition_id id,
		 struct fw_version_resp *resp)
{
	if (vboot_recovery_mode_enabled()) {
		printk(BIOS_WARNING,
			"CSE: Skip sending Get Image Info command during recovery mode!\n");
		return CB_ERR;
	}

	if (id == FPT_PARTITION_NAME_ISHC && !CONFIG(DRIVERS_INTEL_ISH)) {
		printk(BIOS_WARNING, "CSE: Info request denied, no ISH partition\n");
		return CB_ERR;
	}

	return send_get_fpt_partition_info_cmd(id, resp);
}

static bool is_ish_version_valid(struct cse_fw_ish_version_info *version)
{
	const struct fw_version invalid_fw = {0, 0, 0, 0};
	if (!memcmp(&version->cur_ish_fw_version, &invalid_fw, sizeof(struct fw_version)))
		return false;
	return true;
}

/*
 * Helper function to read ISH version from CSE FPT using HECI command.
 *
 * The HECI command only be executed after memory has been initialized.
 * This is because the command relies on resources that are not available
 * until DRAM initialization command has been sent.
 */
static void store_ish_version(void)
{
	if (CONFIG(SOC_INTEL_CSE_LITE_SYNC_BY_PAYLOAD))
		return;

	if (!ENV_RAMSTAGE)
		return;

	if (vboot_recovery_mode_enabled())
		return;

	struct cse_specific_info *cse_info_in_cbmem = cbmem_find(CBMEM_ID_CSE_INFO);
	if (cse_info_in_cbmem == NULL)
		return;

	struct cse_specific_info cse_info_in_cmos;
	cmos_read_fw_partition_info(&cse_info_in_cmos);

	struct cse_fw_partition_info *cbmem_version = &(cse_info_in_cbmem->cse_fwp_version);
	struct cse_fw_partition_info *cmos_version = &(cse_info_in_cmos.cse_fwp_version);

	/* Get current cse firmware state */
	enum cse_fw_state fw_state = get_cse_state(
		 &(cbmem_version->cur_cse_fw_version),
		 &(cmos_version->ish_partition_info.prev_cse_fw_version),
		 &(cbmem_version->ish_partition_info.prev_cse_fw_version));

	if (fw_state == CSE_FW_WARM_BOOT) {
		return;
	} else {
		if (fw_state == CSE_FW_COLD_BOOT &&
			 is_ish_version_valid(&(cmos_version->ish_partition_info))) {
			/* CMOS data is persistent across cold boots */
			memcpy(&(cse_info_in_cbmem->cse_fwp_version.ish_partition_info),
				&(cse_info_in_cmos.cse_fwp_version.ish_partition_info),
				sizeof(struct cse_fw_ish_version_info));
			store_cse_info_crc(cse_info_in_cbmem);
		} else {
			/*
			 * Current running CSE version is different than previous stored CSE version
			 * which could be due to CSE update or rollback, hence, need to send ISHC
			 * partition info cmd to know the currently running ISH version.
			 */
			struct fw_version_resp resp;
			if (cse_get_fpt_partition_info(FPT_PARTITION_NAME_ISHC,
				 &resp) == CB_SUCCESS) {
				/* Update stored CSE version with current cse version */
				memcpy(&(cbmem_version->ish_partition_info.prev_cse_fw_version),
				 &(cbmem_version->cur_cse_fw_version),  sizeof(struct fw_version));

				/* Retrieve and update current ish version */
				memcpy(&(cbmem_version->ish_partition_info.cur_ish_fw_version),
				 &(resp.manifest_data.version), sizeof(struct fw_version));

				/* Update the CRC */
				store_cse_info_crc(cse_info_in_cbmem);

				/* Update CMOS with current CSE FPT versions.*/
				cmos_write_fw_partition_info(cse_info_in_cbmem);
			}
		}
	}
}

static void ramstage_cse_misc_ops(void *unused)
{
	if (acpi_get_sleep_type() == ACPI_S3)
		return;

	if (CONFIG(SOC_INTEL_CSE_LITE_SYNC_IN_RAMSTAGE))
		cse_fw_sync();

	/*
	 * Store the ISH RW Firmware Version into CBMEM if ISH partition
	 * is available
	 */
	if (!CONFIG(DRIVER_INTEL_ISH_HAS_MAIN_FW) && soc_is_ish_partition_enabled())
		store_ish_version();
}

BOOT_STATE_INIT_ENTRY(BS_PRE_DEVICE, BS_ON_EXIT, ramstage_cse_misc_ops, NULL);
