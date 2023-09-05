/* SPDX-License-Identifier: GPL-2.0-only */

#if CONFIG(TPM)

/* Start of the root of trust */
tpm_result_t vboot_setup_tpm(struct vb2_context *ctx);

/* vboot_extend_pcr function for vb2 context */
tpm_result_t vboot_extend_pcr(struct vb2_context *ctx, int pcr,
			     enum vb2_pcr_digest which_digest);

#else

#define vboot_setup_tpm(ctx) TPM_SUCCESS

#define vboot_extend_pcr(ctx, pcr, which_digest) TPM_SUCCESS

#endif
