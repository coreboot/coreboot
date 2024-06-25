/* SPDX-License-Identifier: GPL-2.0-only */

#include <commonlib/helpers.h>
#include <soc/dimm_slot.h>

/*
 * TODO: add the rest of DIMM slots
 */
static const struct dimm_slot_config dimm_slot_config_table[] = {
	/* socket, channel, dimm, dev_locator, bank_locator, asset_tag */
	{0, 0, 0, "CPU0_DIMM_A1", "BANK 0", "CPU0_DIMM_A1_AssetTag"},
};

const struct dimm_slot_config *get_dimm_slot_config_table(int *size)
{
	*size = ARRAY_SIZE(dimm_slot_config_table);
	return dimm_slot_config_table;
}
