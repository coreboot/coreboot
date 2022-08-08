/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/cppc.h>
#include <ccx_cppc_data.h>
#include <console/console.h>
#include <FspGuids.h>
#include <fsp/util.h>
#include <string.h>
#include <types.h>


static enum cb_err get_ccx_cppc_data_hob(const struct fsp_ccx_cppc_data **cppc_data)
{
	static const struct fsp_ccx_cppc_data *cppc_data_cache;
	size_t hob_size = 0;
	const struct fsp_ccx_cppc_data *hob;

	if (cppc_data_cache) {
		*cppc_data = cppc_data_cache;
		return CB_SUCCESS;
	}

	hob = fsp_find_extension_hob_by_guid(AMD_FSP_CCX_CPPC_DATA_HOB_GUID.b, &hob_size);

	if (hob == NULL || hob_size < sizeof(struct fsp_ccx_cppc_data)) {
		printk(BIOS_ERR, "Couldn't find CCX CPPC data HOB.\n");
		return CB_ERR;
	}

	if (hob->version != FSP_CCX_CPPC_DATA_VERSION) {
		printk(BIOS_ERR, "Unexpected CCX CPPC data HOB version.\n");
		return CB_ERR;
	}

	cppc_data_cache = hob;
	*cppc_data = cppc_data_cache;
	return CB_SUCCESS;
}

enum cb_err get_ccx_cppc_min_frequency(uint32_t *freq)
{
	const struct fsp_ccx_cppc_data *cppc_data = NULL;

	if (get_ccx_cppc_data_hob(&cppc_data) != CB_SUCCESS)
		return CB_ERR;

	*freq = cppc_data->ccx_cppc_min_speed;
	printk(BIOS_SPEW, "CCX CPPC min speed: %d MHz\n", *freq);

	return CB_SUCCESS;
}

enum cb_err get_ccx_cppc_nom_frequency(uint32_t *freq)
{
	const struct fsp_ccx_cppc_data *cppc_data = NULL;

	if (get_ccx_cppc_data_hob(&cppc_data) != CB_SUCCESS)
		return CB_ERR;

	*freq = cppc_data->ccx_cppc_nom_speed;
	printk(BIOS_SPEW, "CCX CPPC nom speed: %d MHz\n", *freq);

	return CB_SUCCESS;
}
