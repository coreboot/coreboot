/*
 * This file is part of the coreboot project.
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


#include <security/tpm/tspi.h>
#include <vb2_api.h>
#include <security/vboot/tpm_common.h>

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

	switch (which_digest) {
	/* SHA1 of (devmode|recmode|keyblock) bits */
	case BOOT_MODE_PCR:
		return tpm_extend_pcr(pcr, VB2_HASH_SHA1, buffer, size,
				      TPM_PCR_BOOT_MODE);
	 /* SHA256 of HWID */
	case HWID_DIGEST_PCR:
		return tpm_extend_pcr(pcr, VB2_HASH_SHA256, buffer,
					  size, TPM_PCR_GBB_HWID_NAME);
	default:
		return VB2_ERROR_UNKNOWN;
	}
}
