/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <FspGuids.h>
#include <fsp/amd_misc_data.h>
#include <fsp/amd_misc_data_hob.h>
#include <fsp/util.h>
#include <string.h>
#include <types.h>

static enum cb_err get_amd_misc_data_hob(const struct amd_misc_data **fsp_misc_data, int min_revision)
{
	static const struct amd_misc_data *fsp_misc_data_cache;
	size_t hob_size = 0;
	const struct amd_misc_data *hob;

	if (fsp_misc_data_cache) {
		*fsp_misc_data = fsp_misc_data_cache;
		return CB_SUCCESS;
	}

	hob = fsp_find_extension_hob_by_guid(AMD_MISC_DATA_HOB_GUID.b, &hob_size);

	if (hob == NULL || hob_size < sizeof(struct amd_misc_data)) {
		printk(BIOS_ERR, "Couldn't find fsp misc data HOB.\n");
		return CB_ERR;
	}

	if (hob->version < min_revision) {
		printk(BIOS_ERR, "Unexpected fsp misc data HOB version.\n");
		return CB_ERR;
	}

	fsp_misc_data_cache = hob;
	*fsp_misc_data = fsp_misc_data_cache;
	return CB_SUCCESS;
}

enum cb_err get_amd_smu_reported_tdp(uint32_t *tdp)
{
	const struct amd_misc_data *fsp_misc_data = NULL;

	if (get_amd_misc_data_hob(&fsp_misc_data, AMD_MISC_DATA_VERSION) != CB_SUCCESS)
		return CB_ERR;

	/*
	 * The FSP will return the TDP in the format 0xX0000, where 'X' is the value
	 * we're interested in. For example: 0xF0000 (15W), 0x60000 (6W). Re-interpret
	 * the value so the caller just sees the TDP.
	 */
	printk(BIOS_DEBUG, "fsp_misc_data->smu_power_and_thm_limit = 0x%08X\n",
		fsp_misc_data->smu_power_and_thm_limit);
	*tdp = fsp_misc_data->smu_power_and_thm_limit >> 16;

	return CB_SUCCESS;
}
