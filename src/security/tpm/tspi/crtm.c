/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <fmap.h>
#include <cbfs.h>
#include "crtm.h"
#include <string.h>

/*
 * This function sets the TCPA log namespace
 * for the cbfs file (region) lookup.
 */
static int create_tcpa_metadata(const struct region_device *rdev,
		const char *cbfs_name, char log_string[TCPA_PCR_HASH_NAME])
{
	int i;
	struct region_device fmap;
	static const char *const fmap_cbfs_names[] = {
		"COREBOOT",
		"FW_MAIN_A",
		"FW_MAIN_B",
		"RW_LEGACY"
	};

	for (i = 0; i < ARRAY_SIZE(fmap_cbfs_names); i++) {
		if (fmap_locate_area_as_rdev(fmap_cbfs_names[i], &fmap) == 0) {
			if (region_is_subregion(region_device_region(&fmap),
				region_device_region(rdev))) {
				snprintf(log_string, TCPA_PCR_HASH_NAME,
					"FMAP: %s CBFS: %s",
					fmap_cbfs_names[i], cbfs_name);
				return 0;
			}
		}
	}

	return -1;
}

static int tcpa_log_initialized;
static inline int tcpa_log_available(void)
{
	if (ENV_BOOTBLOCK)
		return tcpa_log_initialized;

	return 1;
}

uint32_t tspi_init_crtm(void)
{
	struct prog bootblock = PROG_INIT(PROG_BOOTBLOCK, "bootblock");

	/* Initialize TCPA PRERAM log. */
	if (!tcpa_log_available()) {
		tcpa_preram_log_clear();
		tcpa_log_initialized = 1;
	} else {
		printk(BIOS_WARNING, "TSPI: CRTM already initialized!\n");
		return VB2_SUCCESS;
	}

	/* measure bootblock from RO */
	struct cbfsf bootblock_data;
	struct region_device bootblock_fmap;
	if (fmap_locate_area_as_rdev("BOOTBLOCK", &bootblock_fmap) == 0) {
		if (tpm_measure_region(&bootblock_fmap,
				       TPM_CRTM_PCR,
				       "FMAP: BOOTBLOCK"))
			return VB2_ERROR_UNKNOWN;
	} else {
		if (cbfs_boot_locate(&bootblock_data,
			prog_name(&bootblock), NULL)) {
			/*
			 * measurement is done in
			 * tspi_measure_cbfs_hook()
			 */
			printk(BIOS_INFO,
			       "TSPI: Couldn't measure bootblock into CRTM!\n");
			return VB2_ERROR_UNKNOWN;
		}
	}

	return VB2_SUCCESS;
}

static bool is_runtime_data(const char *name)
{
	const char *allowlist = CONFIG_TPM_MEASURED_BOOT_RUNTIME_DATA;
	size_t allowlist_len = sizeof(CONFIG_TPM_MEASURED_BOOT_RUNTIME_DATA) - 1;
	size_t name_len = strlen(name);
	const char *end;

	if (!allowlist_len || !name_len)
		return false;

	while ((end = strchr(allowlist, ' '))) {
		if (end - allowlist == name_len && !strncmp(allowlist, name, name_len))
			return true;
		allowlist = end + 1;
	}

	return !strcmp(allowlist, name);
}

uint32_t tspi_measure_cbfs_hook(const struct region_device *rdev, const char *name,
				uint32_t cbfs_type)
{
	uint32_t pcr_index;
	char tcpa_metadata[TCPA_PCR_HASH_NAME];

	if (!tcpa_log_available()) {
		if (tspi_init_crtm() != VB2_SUCCESS) {
			printk(BIOS_WARNING,
			       "Initializing CRTM failed!\n");
			return 0;
		}
		printk(BIOS_DEBUG, "CRTM initialized.\n");
	}

	switch (cbfs_type) {
	case CBFS_TYPE_MRC_CACHE:
		pcr_index = TPM_RUNTIME_DATA_PCR;
		break;
	/*
	 * mrc.bin is code executed on CPU, so it
	 * should not be considered runtime data
	 */
	case CBFS_TYPE_MRC:
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

	if (create_tcpa_metadata(rdev, name, tcpa_metadata) < 0)
		return VB2_ERROR_UNKNOWN;

	return tpm_measure_region(rdev, pcr_index, tcpa_metadata);
}

int tspi_measure_cache_to_pcr(void)
{
	int i;
	enum vb2_hash_algorithm hash_alg;
	struct tcpa_table *tclt = tcpa_log_init();

	if (!tclt) {
		printk(BIOS_WARNING, "TCPA: Log non-existent!\n");
		return VB2_ERROR_UNKNOWN;
	}
	if (CONFIG(TPM1)) {
		hash_alg = VB2_HASH_SHA1;
	} else { /* CONFIG_TPM2 */
		hash_alg = VB2_HASH_SHA256;
	}

	printk(BIOS_DEBUG, "TPM: Write digests cached in TCPA log to PCR\n");
	for (i = 0; i < tclt->num_entries; i++) {
		struct tcpa_entry *tce = &tclt->entries[i];
		if (tce) {
			printk(BIOS_DEBUG, "TPM: Write digest for"
			       " %s into PCR %d\n",
			       tce->name, tce->pcr);
			int result = tlcl_extend(tce->pcr,
						 tce->digest,
						 NULL);
			if (result != TPM_SUCCESS) {
				printk(BIOS_ERR, "TPM: Writing digest"
				       " of %s into PCR failed with error"
				       " %d\n",
				       tce->name, result);
				return VB2_ERROR_UNKNOWN;
			}
		}
	}

	return VB2_SUCCESS;
}
