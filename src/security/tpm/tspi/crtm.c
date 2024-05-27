/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <fmap.h>
#include <bootstate.h>
#include <cbfs.h>
#include <symbols.h>
#include "crtm.h"
#include <stdio.h>
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
 * returns on success TPM_SUCCESS, else a TPM error.
 */
static tpm_result_t tspi_init_crtm(void)
{
	tpm_result_t rc = TPM_SUCCESS;
	/* Initialize TPM PRERAM log. */
	if (!tpm_log_available()) {
		tpm_preram_log_clear();
		tpm_log_initialized = 1;
	} else {
		printk(BIOS_WARNING, "TSPI: CRTM already initialized!\n");
		return TPM_SUCCESS;
	}

	struct region_device fmap;
	if (fmap_locate_area_as_rdev("FMAP", &fmap) == 0) {
		rc = tpm_measure_region(&fmap, CONFIG_PCR_SRTM, "FMAP: FMAP");
		if (rc) {
			printk(BIOS_ERR,
			       "TSPI: Couldn't measure FMAP into CRTM! rc %#x\n", rc);
			return rc;
		}
	} else {
		printk(BIOS_ERR, "TSPI: Could not find FMAP!\n");
	}

	/* measure bootblock from RO */
	if (!CONFIG(ARCH_X86)) {
		struct region_device bootblock_fmap;
		if (fmap_locate_area_as_rdev("BOOTBLOCK", &bootblock_fmap) == 0) {
			rc = tpm_measure_region(&bootblock_fmap,
					CONFIG_PCR_SRTM,
					"FMAP: BOOTBLOCK");
			if (rc)
				return rc;
		}
	} else if (CONFIG(BOOTBLOCK_IN_CBFS)) {
		/* Mapping measures the file. We know we can safely map here because
		   bootblock-as-a-file is only used on x86, where we don't need cache to map. */
		enum cbfs_type type = CBFS_TYPE_BOOTBLOCK;
		void *mapping = cbfs_ro_type_map("bootblock", NULL, &type);
		if (!mapping) {
			printk(BIOS_INFO,
			       "TSPI: Couldn't measure bootblock into CRTM!\n");
			return TPM_CB_FAIL;
		}
		cbfs_unmap(mapping);
	} else {
		/* Since none of the above conditions are met let the SOC code measure the
		 * bootblock. This accomplishes for cases where the bootblock is treated
		 * in a special way (e.g. part of IFWI or located in a different CBFS). */
		if (tspi_soc_measure_bootblock(CONFIG_PCR_SRTM)) {
			printk(BIOS_INFO,
			       "TSPI: Couldn't measure bootblock into CRTM on SoC level!\n");
			return TPM_CB_FAIL;
		}
	}

	return TPM_SUCCESS;
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

tpm_result_t tspi_cbfs_measurement(const char *name, uint32_t type, const struct vb2_hash *hash)
{
	uint32_t pcr_index;
	tpm_result_t rc = TPM_SUCCESS;
	char tpm_log_metadata[TPM_CB_LOG_PCR_HASH_NAME];

	if (!tpm_log_available()) {
		rc = tspi_init_crtm();
		if (rc) {
			printk(BIOS_WARNING,
			       "Initializing CRTM failed!\n");
			return rc;
		}
		printk(BIOS_DEBUG, "CRTM initialized.\n");
	}

	switch (type) {
	case CBFS_TYPE_MRC_CACHE:
		pcr_index = CONFIG_PCR_RUNTIME_DATA;
		break;
	/*
	 * mrc.bin is code executed on CPU, so it
	 * should not be considered runtime data
	 */
	case CBFS_TYPE_MRC:
	case CBFS_TYPE_STAGE:
	case CBFS_TYPE_SELF:
	case CBFS_TYPE_FIT_PAYLOAD:
		pcr_index = CONFIG_PCR_SRTM;
		break;
	default:
		if (is_runtime_data(name))
			pcr_index = CONFIG_PCR_RUNTIME_DATA;
		else
			pcr_index = CONFIG_PCR_SRTM;
		break;
	}

	snprintf(tpm_log_metadata, TPM_CB_LOG_PCR_HASH_NAME, "CBFS: %s", name);

	return tpm_extend_pcr(pcr_index, hash->algo, hash->raw, vb2_digest_size(hash->algo),
			      tpm_log_metadata);
}

void *tpm_log_init(void)
{
	static void *tclt;

	/* We are dealing here with pre CBMEM environment.
	 * If cbmem isn't available use CAR or SRAM */
	if (!ENV_HAS_CBMEM && !CONFIG(VBOOT_RETURN_FROM_VERSTAGE))
		return _tpm_log;
	else if (ENV_CREATES_CBMEM
		 && !CONFIG(VBOOT_RETURN_FROM_VERSTAGE)) {
		tclt = tpm_log_cbmem_init();
		if (!tclt)
			return _tpm_log;
	} else {
		tclt = tpm_log_cbmem_init();
	}

	return tclt;
}

tpm_result_t tspi_measure_cache_to_pcr(void)
{
	int i;
	int pcr;
	const char *event_name;
	const uint8_t *digest_data;
	enum vb2_hash_algorithm digest_algo;

	/* This means the table is empty. */
	if (!tpm_log_available())
		return TPM_SUCCESS;

	if (tpm_log_init() == NULL) {
		printk(BIOS_WARNING, "TPM LOG: log non-existent!\n");
		return TPM_CB_FAIL;
	}

	printk(BIOS_DEBUG, "TPM: Write digests cached in TPM log to PCR\n");
	i = 0;
	while (!tpm_log_get(i++, &pcr, &digest_data, &digest_algo, &event_name)) {
		printk(BIOS_DEBUG, "TPM: Write digest for %s into PCR %d\n", event_name, pcr);
		tpm_result_t rc = tlcl_extend(pcr, digest_data, digest_algo);
		if (rc != TPM_SUCCESS) {
			printk(BIOS_ERR,
			       "TPM: Writing digest of %s into PCR failed with error %d\n",
				event_name, rc);
			return rc;
		}
	}

	return TPM_SUCCESS;
}

#if !CONFIG(VBOOT_RETURN_FROM_VERSTAGE)
static void recover_tpm_log(int is_recovery)
{
	const void *preram_log = _tpm_log;
	void *ram_log = tpm_log_cbmem_init();

	if (tpm_log_get_size(preram_log) > MAX_PRERAM_TPM_LOG_ENTRIES) {
		printk(BIOS_WARNING, "TPM LOG: pre-RAM log is too full, possible corruption\n");
		return;
	}

	if (ram_log == NULL) {
		printk(BIOS_WARNING, "TPM LOG: CBMEM not available, something went wrong\n");
		return;
	}

	tpm_log_copy_entries(_tpm_log, ram_log);
}
CBMEM_CREATION_HOOK(recover_tpm_log);
#endif

BOOT_STATE_INIT_ENTRY(BS_PAYLOAD_BOOT, BS_ON_ENTRY, tpm_log_dump, NULL);
