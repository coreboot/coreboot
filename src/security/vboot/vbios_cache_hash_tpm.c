/* SPDX-License-Identifier: GPL-2.0-only */

#include <security/vboot/antirollback.h>
#include <program_loading.h>
#include <vb2_api.h>
#include <security/tpm/tss.h>
#include <security/vboot/misc.h>
#include <security/vboot/vbios_cache_hash_tpm.h>
#include <console/console.h>
#include <string.h>

void vbios_cache_update_hash(const uint8_t *data, size_t size)
{
	struct vb2_hash hash;

	/* Initialize TPM driver. */
	if (tlcl_lib_init() != VB2_SUCCESS) {
		printk(BIOS_ERR, "VBIOS_CACHE: TPM driver initialization failed.\n");
		return;
	}

	/* Calculate hash of vbios data. */
	if (vb2_hash_calculate(vboot_hwcrypto_allowed(), data, size,
			       VB2_HASH_SHA256, &hash)) {
		printk(BIOS_ERR, "VBIOS_CACHE: SHA-256 calculation failed for data; "
		       "clearing TPM hash space.\n");
		/*
		 * Since data is being updated in vbios cache, the hash
		 * currently stored in TPM hash space is no longer
		 * valid. If we are not able to calculate hash of the
		 * data being updated, reset all the bits in TPM hash
		 * space to zero to invalidate it.
		 */
		memset(hash.raw, 0, VB2_SHA256_DIGEST_SIZE);
	}

	/* Write hash of data to TPM space. */
	if (antirollback_write_space_vbios_hash(hash.sha256, sizeof(hash.sha256))
			!= TPM_SUCCESS) {
		printk(BIOS_ERR, "VBIOS_CACHE: Could not save hash to TPM.\n");
		return;
	}

	printk(BIOS_INFO, "VBIOS_CACHE: TPM NV idx 0x%x updated successfully.\n",
			VBIOS_CACHE_NV_INDEX);
}

enum cb_err vbios_cache_verify_hash(const uint8_t *data, size_t size)
{
	struct vb2_hash tpm_hash = { .algo = VB2_HASH_SHA256 };

	/* Initialize TPM driver. */
	if (tlcl_lib_init() != VB2_SUCCESS) {
		printk(BIOS_ERR, "VBIOS_CACHE: TPM driver initialization failed.\n");
		return CB_ERR;
	}

	/* Read hash of VBIOS data saved in TPM. */
	if (antirollback_read_space_vbios_hash(tpm_hash.sha256,	sizeof(tpm_hash.sha256))
			!= TPM_SUCCESS) {
		printk(BIOS_ERR, "VBIOS_CACHE: Could not read hash from TPM.\n");
		return CB_ERR;
	}

	/* Calculate hash of data read from VBIOS FMAP CACHE and compare. */
	if (vb2_hash_verify(vboot_hwcrypto_allowed(), data, size, &tpm_hash)) {
		printk(BIOS_ERR, "VBIOS_CACHE: Hash comparison failed.\n");
		return CB_ERR;
	}

	printk(BIOS_INFO, "VBIOS_CACHE: Hash idx 0x%x comparison successful.\n",
			VBIOS_CACHE_NV_INDEX);

	return CB_SUCCESS;
}
