/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <fmap.h>
#include <cbfs.h>
#include "crtm.h"
#include <string.h>

static int tpm_log_initialized;
static inline int tpm_log_available(void)
{
	if (ENV_BOOTBLOCK)
		return tpm_log_initialized;

	return 1;
}

/*
 * Initializes the Core Root of Trust for Measurements
 * in coreboot. The initial code in a chain of trust must measure
 * itself.
 *
 * Summary:
 *  + Measures the FMAP FMAP partition.
 *  + Measures bootblock in CBFS or BOOTBLOCK FMAP partition.
 *  + If vboot starts in romstage, it measures the romstage
 *    in CBFS.
 *  + Measure the verstage if it is compiled as separate
 *    stage.
 *
 * Takes the current vboot context as parameter for s3 checks.
 * returns on success VB2_SUCCESS, else a vboot error.
 */
static uint32_t tspi_init_crtm(void)
{
	/* Initialize TPM PRERAM log. */
	if (!tpm_log_available()) {
		tpm_preram_log_clear();
		tpm_log_initialized = 1;
	} else {
		printk(BIOS_WARNING, "TSPI: CRTM already initialized!\n");
		return VB2_SUCCESS;
	}

	struct region_device fmap;
	if (fmap_locate_area_as_rdev("FMAP", &fmap) == 0) {
		if (tpm_measure_region(&fmap, TPM_CRTM_PCR, "FMAP: FMAP")) {
			printk(BIOS_ERR,
			       "TSPI: Couldn't measure FMAP into CRTM!\n");
			return VB2_ERROR_UNKNOWN;
		}
	} else {
		printk(BIOS_ERR, "TSPI: Could not find FMAP!\n");
	}

	/* measure bootblock from RO */
	if (!CONFIG(ARCH_X86)) {
		struct region_device bootblock_fmap;
		if (fmap_locate_area_as_rdev("BOOTBLOCK", &bootblock_fmap) == 0) {
			if (tpm_measure_region(&bootblock_fmap,
					TPM_CRTM_PCR,
					"FMAP: BOOTBLOCK"))
				return VB2_ERROR_UNKNOWN;
		}
	} else if (CONFIG(BOOTBLOCK_IN_CBFS)){
		/* Mapping measures the file. We know we can safely map here because
		   bootblock-as-a-file is only used on x86, where we don't need cache to map. */
		enum cbfs_type type = CBFS_TYPE_BOOTBLOCK;
		void *mapping = cbfs_ro_type_map("bootblock", NULL, &type);
		if (!mapping) {
			printk(BIOS_INFO,
			       "TSPI: Couldn't measure bootblock into CRTM!\n");
			return VB2_ERROR_UNKNOWN;
		}
		cbfs_unmap(mapping);
	} else {
		/* Since none of the above conditions are met let the SOC code measure the
		 * bootblock. This accomplishes for cases where the bootblock is treated
		 * in a special way (e.g. part of IFWI or located in a different CBFS). */
		if (tspi_soc_measure_bootblock(TPM_CRTM_PCR)) {
			printk(BIOS_INFO,
			       "TSPI: Couldn't measure bootblock into CRTM on SoC level!\n");
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

uint32_t tspi_cbfs_measurement(const char *name, uint32_t type, const struct vb2_hash *hash)
{
	uint32_t pcr_index;
	char tpm_log_metadata[TPM_CB_LOG_PCR_HASH_NAME];

	if (!tpm_log_available()) {
		if (tspi_init_crtm() != VB2_SUCCESS) {
			printk(BIOS_WARNING,
			       "Initializing CRTM failed!\n");
			return 0;
		}
		printk(BIOS_DEBUG, "CRTM initialized.\n");
	}

	switch (type) {
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
	case CBFS_TYPE_FIT_PAYLOAD:
		pcr_index = TPM_CRTM_PCR;
		break;
	default:
		if (is_runtime_data(name))
			pcr_index = TPM_RUNTIME_DATA_PCR;
		else
			pcr_index = TPM_CRTM_PCR;
		break;
	}

	snprintf(tpm_log_metadata, TPM_CB_LOG_PCR_HASH_NAME, "CBFS: %s", name);

	return tpm_extend_pcr(pcr_index, hash->algo, hash->raw, vb2_digest_size(hash->algo),
			      tpm_log_metadata);
}

int tspi_measure_cache_to_pcr(void)
{
	int i;
	struct tpm_cb_log_table *tclt = tpm_log_init();

	/* This means the table is empty. */
	if (!tpm_log_available())
		return VB2_SUCCESS;

	if (!tclt) {
		printk(BIOS_WARNING, "TPM LOG: log non-existent!\n");
		return VB2_ERROR_UNKNOWN;
	}

	printk(BIOS_DEBUG, "TPM: Write digests cached in TPM log to PCR\n");
	for (i = 0; i < tclt->num_entries; i++) {
		struct tpm_cb_log_entry *tce = &tclt->entries[i];
		if (tce) {
			printk(BIOS_DEBUG, "TPM: Write digest for"
			       " %s into PCR %d\n",
			       tce->name, tce->pcr);
			int result = tlcl_extend(tce->pcr,
						 tce->digest,
						 TPM_MEASURE_ALGO);
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
