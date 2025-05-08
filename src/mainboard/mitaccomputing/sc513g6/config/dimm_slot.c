/* SPDX-License-Identifier: GPL-2.0-only */

#include <commonlib/helpers.h>
#include <soc/dimm_slot.h>

static const struct dimm_slot_config dimm_slot_config_table[] = {
	/* socket, channel, dimm, dev_locator, bank_locator, asset_tag */
	{0, 0, 0, "CPU0_DIMM_A", "BANK 0", "CPU0_DIMM_A_AssetTag"},
	{0, 1, 0, "CPU0_DIMM_B", "BANK 1", "CPU0_DIMM_B_AssetTag"},
	{0, 2, 0, "CPU0_DIMM_C", "BANK 2", "CPU0_DIMM_C_AssetTag"},
	{0, 3, 0, "CPU0_DIMM_D", "BANK 3", "CPU0_DIMM_D_AssetTag"},
	{0, 4, 0, "CPU0_DIMM_E", "BANK 4", "CPU0_DIMM_E_AssetTag"},
	{0, 5, 0, "CPU0_DIMM_F", "BANK 5", "CPU0_DIMM_F_AssetTag"},
	{0, 6, 0, "CPU0_DIMM_G", "BANK 6", "CPU0_DIMM_G_AssetTag"},
	{0, 7, 0, "CPU0_DIMM_H", "BANK 7", "CPU0_DIMM_H_AssetTag"},
};

const struct dimm_slot_config *get_dimm_slot_config_table(int *size)
{
	*size = ARRAY_SIZE(dimm_slot_config_table);
	return dimm_slot_config_table;
}
