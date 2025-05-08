/* SPDX-License-Identifier: GPL-2.0-only */

#include <commonlib/helpers.h>
#include <soc/dimm_slot.h>

static const struct dimm_slot_config dimm_slot_config_table[] = {
	/* socket, channel, dimm, dev_locator, bank_locator, asset_tag */
	{0, 0, 0, "CPU0_DIMM_A1", "BANK 0", "CPU0_DIMM_A1_AssetTag"},
	{0, 0, 1, "CPU0_DIMM_A2", "BANK 1", "CPU0_DIMM_A2_AssetTag"},
	{0, 1, 0, "CPU0_DIMM_B1", "BANK 2", "CPU0_DIMM_B1_AssetTag"},
	{0, 1, 1, "CPU0_DIMM_B2", "BANK 3", "CPU0_DIMM_B2_AssetTag"},
	{0, 2, 0, "CPU0_DIMM_C1", "BANK 4", "CPU0_DIMM_C1_AssetTag"},
	{0, 2, 1, "CPU0_DIMM_C2", "BANK 5", "CPU0_DIMM_C2_AssetTag"},
	{0, 3, 0, "CPU0_DIMM_D1", "BANK 6", "CPU0_DIMM_D1_AssetTag"},
	{0, 3, 1, "CPU0_DIMM_D2", "BANK 7", "CPU0_DIMM_D2_AssetTag"},
	{0, 4, 0, "CPU0_DIMM_E1", "BANK 8", "CPU0_DIMM_E1_AssetTag"},
	{0, 4, 1, "CPU0_DIMM_E2", "BANK 9", "CPU0_DIMM_E2_AssetTag"},
	{0, 5, 0, "CPU0_DIMM_F1", "BANK 10", "CPU0_DIMM_F1_AssetTag"},
	{0, 5, 1, "CPU0_DIMM_F2", "BANK 11", "CPU0_DIMM_F2_AssetTag"},
	{0, 6, 0, "CPU0_DIMM_G1", "BANK 12", "CPU0_DIMM_G1_AssetTag"},
	{0, 6, 1, "CPU0_DIMM_G2", "BANK 13", "CPU0_DIMM_G2_AssetTag"},
	{0, 7, 0, "CPU0_DIMM_H1", "BANK 14", "CPU0_DIMM_H1_AssetTag"},
	{0, 7, 1, "CPU0_DIMM_H2", "BANK 15", "CPU0_DIMM_H2_AssetTag"},
	{1, 0, 0, "CPU1_DIMM_A1", "BANK 16", "CPU1_DIMM_A1_AssetTag"},
	{1, 0, 1, "CPU1_DIMM_A2", "BANK 17", "CPU1_DIMM_A2_AssetTag"},
	{1, 1, 0, "CPU1_DIMM_B1", "BANK 18", "CPU1_DIMM_B1_AssetTag"},
	{1, 1, 1, "CPU1_DIMM_B2", "BANK 19", "CPU1_DIMM_B2_AssetTag"},
	{1, 2, 0, "CPU1_DIMM_C1", "BANK 20", "CPU1_DIMM_C1_AssetTag"},
	{1, 2, 1, "CPU1_DIMM_C2", "BANK 21", "CPU1_DIMM_C2_AssetTag"},
	{1, 3, 0, "CPU1_DIMM_D1", "BANK 22", "CPU1_DIMM_D1_AssetTag"},
	{1, 3, 1, "CPU1_DIMM_D2", "BANK 23", "CPU1_DIMM_D2_AssetTag"},
	{1, 4, 0, "CPU1_DIMM_E1", "BANK 24", "CPU1_DIMM_E1_AssetTag"},
	{1, 4, 1, "CPU1_DIMM_E2", "BANK 25", "CPU1_DIMM_E2_AssetTag"},
	{1, 5, 0, "CPU1_DIMM_F1", "BANK 26", "CPU1_DIMM_F1_AssetTag"},
	{1, 5, 1, "CPU1_DIMM_F2", "BANK 27", "CPU1_DIMM_F2_AssetTag"},
	{1, 6, 0, "CPU1_DIMM_G1", "BANK 28", "CPU1_DIMM_G1_AssetTag"},
	{1, 6, 1, "CPU1_DIMM_G2", "BANK 29", "CPU1_DIMM_G2_AssetTag"},
	{1, 7, 0, "CPU1_DIMM_H1", "BANK 30", "CPU1_DIMM_H1_AssetTag"},
	{1, 7, 1, "CPU1_DIMM_H2", "BANK 31", "CPU1_DIMM_H2_AssetTag"},
};

const struct dimm_slot_config *get_dimm_slot_config_table(int *size)
{
	*size = ARRAY_SIZE(dimm_slot_config_table);
	return dimm_slot_config_table;
}
