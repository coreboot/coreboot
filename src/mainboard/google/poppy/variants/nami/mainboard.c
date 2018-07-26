/*
 * This file is part of the coreboot project.
 *
 * Copyright 2018 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <arch/cpu.h>
#include <assert.h>
#include <baseboard/variants.h>
#include <cbfs.h>
#include <chip.h>
#include <commonlib/cbfs_serialized.h>
#include <compiler.h>
#include <device/device.h>
#include <drivers/intel/gma/opregion.h>
#include <ec/google/chromeec/ec.h>
#include <intelblocks/mp_init.h>
#include <smbios.h>
#include <soc/ramstage.h>
#include <string.h>
#include <variant/sku.h>

#define PL2_I7_SKU	44
#define PL2_DEFAULT	29
#define PL2_KBL_R	25

/* Variant for AKALI */
#define AKALI_SA_AC_LOADLINE	1100
#define AKALI_SA_DC_LOADLINE	1028
#define AKALI_IA_AC_LOADLINE	272
#define AKALI_IA_DC_LOADLINE	247
#define AKALI_GT_AC_LOADLINE	314
#define AKALI_GT_DC_LOADLINE	321

/* We only have Akali and Nami default settings so far */
enum project_sku {
	PRJ_AKALI = 1,
};

static const struct {
	enum project_sku sku;
	int ac_loadline[NUM_VR_DOMAINS];
	int dc_loadline[NUM_VR_DOMAINS];
} sku_overwrite_mapping[] = {
	{
		.sku = PRJ_AKALI,
		.ac_loadline = {
			AKALI_SA_AC_LOADLINE,
			AKALI_IA_AC_LOADLINE,
			AKALI_GT_AC_LOADLINE,
			AKALI_GT_AC_LOADLINE
		},
		.dc_loadline = {
			AKALI_SA_DC_LOADLINE,
			AKALI_IA_DC_LOADLINE,
			AKALI_GT_DC_LOADLINE,
			AKALI_GT_DC_LOADLINE
		}
	},
};

static uint32_t get_pl2(uint32_t sku_id)
{
	if ((sku_id == SKU_0_SONA) || (sku_id == SKU_1_SONA)) {
		if (cpuid_eax(1) == CPUID_KABYLAKE_Y0)
			return PL2_DEFAULT;

		return PL2_KBL_R;
	}
	if (cpuid_eax(1) == CPUID_KABYLAKE_Y0)
		return PL2_I7_SKU;

	return PL2_DEFAULT;
}

uint32_t variant_board_sku(void)
{
	static uint32_t sku_id = SKU_UNKNOWN;
	uint32_t id;

	if (sku_id != SKU_UNKNOWN)
		return sku_id;
	if (google_chromeec_cbi_get_sku_id(&id))
		return SKU_UNKNOWN;
	sku_id = id;

	return sku_id;
}

const char *smbios_mainboard_sku(void)
{
	static char sku_str[14]; /* sku{0..4294967295} */

	snprintf(sku_str, sizeof(sku_str), "sku%u", variant_board_sku());

	return sku_str;
}

#define OEM_UNKNOWN	0xff

/*
 * Read OEM ID from EC using cbi commands.
 * Return value:
 * Success = OEM ID read from EC
 * Failure = OEM_UNKNOWN (0xff)
 */
static uint8_t read_oem_id(void)
{
	static uint8_t oem_id = OEM_UNKNOWN;
	uint32_t id;

	if (oem_id != OEM_UNKNOWN)
		return oem_id;

	if (google_chromeec_cbi_get_oem_id(&id))
		return OEM_UNKNOWN;

	if (id > OEM_UNKNOWN) {
		printk(BIOS_ERR, "%s: OEM ID too big %u!\n", __func__, id);
		return OEM_UNKNOWN;
	}

	oem_id = id;
	printk(BIOS_DEBUG, "%s: OEM ID=%d\n", __func__, oem_id);

	return oem_id;
}

/* "oem.bin" in cbfs contains array of records using the following structure. */
struct oem_mapping {
	uint8_t oem_id;
	char oem_name[10];
} __packed;

/* Local buffer to read "oem.bin" */
static char oem_bin_data[200];

const char *smbios_mainboard_manufacturer(void)
{
	uint8_t oem_id = read_oem_id();
	const struct oem_mapping *oem_entry = (void *)&oem_bin_data;
	size_t oem_data_size;
	size_t curr = 0;
	static const char *manuf;

	if (manuf)
		return manuf;

	/* If OEM ID cannot be determined, return default manuf string. */
	if (oem_id == OEM_UNKNOWN)
		return CONFIG_MAINBOARD_SMBIOS_MANUFACTURER;

	oem_data_size = cbfs_boot_load_file("oem.bin", oem_bin_data,
					    sizeof(oem_bin_data),
					    CBFS_TYPE_RAW);

	while ((curr < oem_data_size) &&
	       ((oem_data_size - curr) >= sizeof(*oem_entry))) {
		if (oem_id == oem_entry->oem_id) {
			manuf = oem_entry->oem_name;
			break;
		}
		curr += sizeof(*oem_entry);
		oem_entry++;
	}

	if (manuf == NULL)
		manuf = CONFIG_MAINBOARD_SMBIOS_MANUFACTURER;

	return manuf;
}

const char *mainboard_vbt_filename(void)
{
	uint32_t sku_id = variant_board_sku();

	switch (sku_id) {
	case SKU_0_PANTHEON:
	case SKU_1_PANTHEON:
	case SKU_2_PANTHEON:
		return "vbt-pantheon.bin";
	case SKU_0_VAYNE:
	case SKU_1_VAYNE:
	case SKU_2_VAYNE:
		return "vbt-vayne.bin";
	case SKU_0_AKALI:
	case SKU_1_AKALI:
	case SKU_0_AKALI360:
	case SKU_1_AKALI360:
		return "vbt-akali.bin";
	default:
		return "vbt.bin";
		break;
	}
}

static int find_sku_mapping(const uint8_t oem_id)
{
	/* Check if this OEM ID has a mapping table entry. */
	for (int i = 0; i < ARRAY_SIZE(sku_overwrite_mapping); i++)
		if (oem_id == sku_overwrite_mapping[i].sku)
			return i;

	return -1;
}

/* Override dev tree settings per board */
void variant_devtree_update(void)
{
	uint32_t sku_id = variant_board_sku();
	uint32_t i;
	int oem_index;
	struct device *root = SA_DEV_ROOT;
	config_t *cfg = root->chip_info;

	/* Update PL2 based on SKU. */

	cfg->tdp_pl2_override = get_pl2(sku_id);

	switch (sku_id) {
	case SKU_0_VAYNE:
	case SKU_1_VAYNE:
	case SKU_2_VAYNE:
	case SKU_0_PANTHEON:
	case SKU_1_PANTHEON:
	case SKU_2_PANTHEON:
	case SKU_0_SONA:
	case SKU_1_SONA:
		/* Disable unused port USB port */
		cfg->usb2_ports[5].enable = 0;
		break;
	default:
		break;
	}

	/* Overwrite settings for different projects based on OEM ID*/
	oem_index = find_sku_mapping(read_oem_id());

	/* Return if the OEM ID is not supported or no changes are required */
	if (oem_index < 0)
		return;

	for (i = 0; i < ARRAY_SIZE(cfg->domain_vr_config); i++) {
		cfg->domain_vr_config[i].ac_loadline =
				sku_overwrite_mapping[oem_index].ac_loadline[i];
		cfg->domain_vr_config[i].dc_loadline =
				sku_overwrite_mapping[oem_index].dc_loadline[i];
	}
}
