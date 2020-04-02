/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

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
