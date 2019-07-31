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

#if CONFIG(TPM1) || CONFIG(TPM2)

/* Start of the root of trust */
uint32_t vboot_setup_tpm(struct vb2_context *ctx);

/* vboot_extend_pcr function for vb2 context */
vb2_error_t vboot_extend_pcr(struct vb2_context *ctx, int pcr,
			     enum vb2_pcr_digest which_digest);

#else

#define vboot_setup_tpm(ctx) 0

#define vboot_extend_pcr(ctx, pcr, which_digest) 0

#endif
