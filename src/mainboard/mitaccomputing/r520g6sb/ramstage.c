/* SPDX-License-Identifier: GPL-2.0-only */

#include <smbios.h>
#include <soc/dimm_slot.h>
#include <soc/ramstage.h>

void mainboard_silicon_init_params(FSPS_UPD *params)
{

}

/*
 * TODO: SMBIOS info in src/lib/smbios_defaults.c could be overridden based on needs
 */
smbios_wakeup_type smbios_system_wakeup_type(void)
{
	return SMBIOS_WAKEUP_TYPE_POWER_SWITCH;
}

u8 smbios_mainboard_feature_flags(void)
{
	return SMBIOS_FEATURE_FLAGS_HOSTING_BOARD | SMBIOS_FEATURE_FLAGS_REPLACEABLE;
}

smbios_enclosure_type smbios_mainboard_enclosure_type(void)
{
	return SMBIOS_ENCLOSURE_RACK_MOUNT_CHASSIS;
}

void smbios_fill_dimm_locator(const struct dimm_info *dimm, struct smbios_type17 *t)
{
	int size;
	const struct dimm_slot_config *dimm_slot_config_table = get_dimm_slot_config_table(&size);

	for (int i = 0; i < size; i++) {
		if (DIMM_SLOT_EQUAL(dimm_slot_config_table[i],
		    dimm->soc_num, dimm->channel_num, dimm->dimm_num)) {
			const char *locator = dimm_slot_config_table[i].dev_locator;
			t->device_locator = smbios_add_string(t->eos, locator);
			locator = dimm_slot_config_table[i].bank_locator;
			t->bank_locator = smbios_add_string(t->eos, locator);
		}
	}
}

void smbios_fill_dimm_asset_tag(const struct dimm_info *dimm, struct smbios_type17 *t)
{
	int size;
	const struct dimm_slot_config *dimm_slot_config_table = get_dimm_slot_config_table(&size);

	for (int i = 0; i < size; i++) {
		if (DIMM_SLOT_EQUAL(dimm_slot_config_table[i],
		    dimm->soc_num, dimm->channel_num, dimm->dimm_num)) {
			const char *asset_tag = dimm_slot_config_table[i].asset_tag;
			t->asset_tag = smbios_add_string(t->eos, asset_tag);
		}
	}
}
