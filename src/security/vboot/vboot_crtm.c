/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2018 Facebook Inc.
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

#include <console/console.h>
#include <fmap.h>
#include <security/vboot/vboot_crtm.h>
#include <security/vboot/misc.h>

uint32_t vboot_init_crtm(void)
{
	struct prog bootblock = PROG_INIT(PROG_BOOTBLOCK, "bootblock");
	struct prog verstage =
		PROG_INIT(PROG_VERSTAGE, CONFIG_CBFS_PREFIX "/verstage");
	struct prog romstage =
		PROG_INIT(PROG_ROMSTAGE, CONFIG_CBFS_PREFIX "/romstage");

	/* measure bootblock from RO */
	struct cbfsf bootblock_data;
	struct region_device bootblock_fmap;
	if (fmap_locate_area_as_rdev("BOOTBLOCK", &bootblock_fmap) == 0) {
		if (tpm_measure_region(&bootblock_fmap,
				       TPM_CRTM_PCR,
				       prog_name(&bootblock)))
			return VB2_ERROR_UNKNOWN;
	} else {
		if (cbfs_boot_locate(&bootblock_data,
			prog_name(&bootblock), NULL) == 0) {
			cbfs_file_data(prog_rdev(&bootblock), &bootblock_data);

			if (tpm_measure_region(prog_rdev(&bootblock),
					       TPM_CRTM_PCR,
					       prog_name(&bootblock)))
				return VB2_ERROR_UNKNOWN;
		} else {
			printk(BIOS_INFO,
			       "VBOOT: Couldn't measure bootblock into CRTM!\n");
			return VB2_ERROR_UNKNOWN;
		}
	}

	if (IS_ENABLED(CONFIG_VBOOT_STARTS_IN_ROMSTAGE)) {
		struct cbfsf romstage_data;
		/* measure romstage from RO */
		if (cbfs_boot_locate(&romstage_data,
			prog_name(&romstage), NULL) == 0) {
			cbfs_file_data(prog_rdev(&romstage), &romstage_data);

			if (tpm_measure_region(prog_rdev(&romstage),
					       TPM_CRTM_PCR,
					       CONFIG_CBFS_PREFIX "/romstage"))
				return VB2_ERROR_UNKNOWN;
		} else {
			printk(BIOS_INFO,
			       "VBOOT: Couldn't measure %s into CRTM!\n",
			       CONFIG_CBFS_PREFIX "/romstage");
			return VB2_ERROR_UNKNOWN;
		}
	}

	if (IS_ENABLED(CONFIG_VBOOT_SEPARATE_VERSTAGE)) {
		struct cbfsf verstage_data;
		/* measure verstage from RO */
		if (cbfs_boot_locate(&verstage_data,
			prog_name(&verstage), NULL) == 0) {
			cbfs_file_data(prog_rdev(&verstage), &verstage_data);

			if (tpm_measure_region(prog_rdev(&verstage),
					       TPM_CRTM_PCR,
					       CONFIG_CBFS_PREFIX "/verstage"))
				return VB2_ERROR_UNKNOWN;
		} else {
			printk(BIOS_INFO,
			       "VBOOT: Couldn't measure %s into CRTM!\n",
			       CONFIG_CBFS_PREFIX "/verstage");
			return VB2_ERROR_UNKNOWN;
		}
	}

	return VB2_SUCCESS;
}

static bool is_runtime_data(const char *name)
{
	const char *whitelist = CONFIG_VBOOT_MEASURED_BOOT_RUNTIME_DATA;
	size_t whitelist_len = sizeof(CONFIG_VBOOT_MEASURED_BOOT_RUNTIME_DATA) - 1;
	size_t name_len = strlen(name);
	int i;

	if (!whitelist_len || !name_len)
		return false;

	for (i = 0; (i + name_len) <= whitelist_len; i++) {
		if (!strcmp(whitelist + i, name))
			return true;
	}

	return false;
}

uint32_t vboot_measure_cbfs_hook(struct cbfsf *fh, const char *name)
{
	uint32_t pcr_index;
	uint32_t cbfs_type;
	struct region_device rdev;

	if (!vb2_logic_executed())
		return 0;

	cbfsf_file_type(fh, &cbfs_type);
	cbfs_file_data(&rdev, fh);

	switch (cbfs_type) {
	case CBFS_TYPE_MRC:
	case CBFS_TYPE_MRC_CACHE:
		pcr_index = TPM_RUNTIME_DATA_PCR;
		break;
	case CBFS_TYPE_STAGE:
	case CBFS_TYPE_SELF:
	case CBFS_TYPE_FIT:
		pcr_index = TPM_CRTM_PCR;
		break;
	default:
		if (is_runtime_data(name))
			pcr_index = TPM_RUNTIME_DATA_PCR;
		else
			pcr_index = TPM_CRTM_PCR;
		break;
	}

	return tpm_measure_region(&rdev, pcr_index,
				  name);
}
