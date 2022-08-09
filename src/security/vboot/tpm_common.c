/* SPDX-License-Identifier: GPL-2.0-only */

#include <security/tpm/tspi.h>
#include <security/vboot/tpm_common.h>
#include <vb2_api.h>
#include <vb2_sha.h>

#define TPM_PCR_BOOT_MODE "VBOOT: boot mode"
#define TPM_PCR_GBB_HWID_NAME "VBOOT: GBB HWID"

uint32_t vboot_setup_tpm(struct vb2_context *ctx)
{
	uint32_t result;

	result = tpm_setup(ctx->flags & VB2_CONTEXT_S3_RESUME);
	if (result == TPM_E_MUST_REBOOT)
		ctx->flags |= VB2_CONTEXT_SECDATA_WANTS_REBOOT;

	return result;
}

vb2_error_t vboot_extend_pcr(struct vb2_context *ctx, int pcr,
			     enum vb2_pcr_digest which_digest)
{
	uint8_t buffer[VB2_PCR_DIGEST_RECOMMENDED_SIZE];
	uint32_t size = sizeof(buffer);
	vb2_error_t rv;

	rv = vb2api_get_pcr_digest(ctx, which_digest, buffer, &size);
	if (rv != VB2_SUCCESS)
		return rv;
	if (size < TPM_PCR_MINIMUM_DIGEST_SIZE)
		return VB2_ERROR_UNKNOWN;

	/*
	 * On TPM 1.2, all PCRs are intended for use with SHA1. We truncate our
	 * SHA256 HWID hash to 20 bytes to make it fit. On TPM 2.0, we always
	 * want to use the SHA256 banks, even for the boot mode which is
	 * technically a SHA1 value for historical reasons. vboot has already
	 * zero-extended the buffer to 32 bytes for us, so we just take it like
	 * that and pretend it's a SHA256. In practice, this means we never care
	 * about the (*size) value returned from vboot (which indicates how many
	 * significant bytes vboot wrote, although it always extends zeroes up
	 * to the end of the buffer), we always use a hardcoded size instead.
	 */
	_Static_assert(sizeof(buffer) >= VB2_SHA256_DIGEST_SIZE,
		       "Buffer needs to be able to fit at least a SHA256");
	enum vb2_hash_algorithm algo = CONFIG(TPM1) ? VB2_HASH_SHA1 : VB2_HASH_SHA256;

	switch (which_digest) {
	/* SHA1 of (devmode|recmode|keyblock) bits */
	case BOOT_MODE_PCR:
		return tpm_extend_pcr(pcr, algo, buffer, vb2_digest_size(algo),
				      TPM_PCR_BOOT_MODE);
	 /* SHA256 of HWID */
	case HWID_DIGEST_PCR:
		return tpm_extend_pcr(pcr, algo, buffer, vb2_digest_size(algo),
				      TPM_PCR_GBB_HWID_NAME);
	default:
		return VB2_ERROR_UNKNOWN;
	}
}
