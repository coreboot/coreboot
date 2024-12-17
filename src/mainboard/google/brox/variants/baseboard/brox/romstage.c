/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <cbfs.h>
#include <ec/google/chromeec/ec.h>
#include <intelblocks/cse.h>
#include <security/vboot/vboot_common.h>
#include <security/vboot/misc.h>
#include <soc/romstage.h>

#define FWVER_SIZE 3
#define FWVER_TO_INT(MAJOR, MINOR, PATCH)                                                  \
	((uint32_t)(((MAJOR) & 0xFF) << 16 | ((MINOR) & 0xFF) << 8 | ((PATCH) & 0xFF)))

static bool check_auxfw_ver_mismatch(void)
{
	uint8_t *new_ver;
	size_t new_ver_size;
	struct ec_response_pd_chip_info pd_chip_r = {0};
	const char *fwver_fname = variant_get_auxfw_version_file();
	uint8_t cur_major_ver;
	bool mismatch = false, is_productionfw;
	int ret;

	ret = google_chromeec_get_pd_chip_info(0, 0, &pd_chip_r);
	if (ret < 0) {
		printk(BIOS_INFO, "%s: Cannot get PD port info\n", __func__);
		return mismatch;
	}
	cur_major_ver = (pd_chip_r.fw_version_number >> 16) & 0xFF;
	is_productionfw = !!(cur_major_ver & 0xF0);

	/* find bundled fw hash */
	new_ver = cbfs_map(fwver_fname, &new_ver_size);
	if (new_ver == NULL || new_ver_size != FWVER_SIZE)
		return mismatch;

	/*
	 * Firmware version mismatches and satisfies anti-rollback conditions.
	 * Anti-rollback conditions are one of the following:
	 * 1) Not a production firmware.
	 * 2) New major version is greater than current major version.
	 */
	if ((pd_chip_r.fw_version_number !=
	     FWVER_TO_INT(new_ver[0], new_ver[1], new_ver[2])) &&
	    (!is_productionfw || new_ver[0] >= cur_major_ver)) {
		printk(BIOS_INFO, "%s: Expecting Aux FW update and hence a reset\n", __func__);
		mismatch = true;
	}

	cbfs_unmap(new_ver);
	return mismatch;
}

bool mainboard_expects_another_reset(void)
{
	/* Do not change the order of the check in this function */
	if (vboot_recovery_mode_enabled())
		return false;

	/* If CSE is booting from RO, CSE state switch will issue a reset anyway. */
	if (!is_cse_boot_to_rw())
		return true;

	if (!CONFIG(VBOOT) ||
	    (vboot_is_gbb_flag_set(VB2_GBB_FLAG_DISABLE_EC_SOFTWARE_SYNC) &&
	    vboot_is_gbb_flag_set(VB2_GBB_FLAG_DISABLE_AUXFW_SOFTWARE_SYNC)))
		return false;

	return check_auxfw_ver_mismatch();
}
