/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef CR50_TSS_STRUCTURES_H_
#define CR50_TSS_STRUCTURES_H_

#include <stdint.h>

/* FIXME: below is not enough to differentiate between vendors commands
   of numerous devices. However, the current tpm2 APIs aren't very amenable
   to extending generically because the marshaling code is assuming all
   knowledge of all commands. */
#define TPM2_CR50_VENDOR_COMMAND ((TPM_CC)(TPM_CC_VENDOR_BIT_MASK | 0))
#define TPM2_CR50_SUB_CMD_IMMEDIATE_RESET (19)
#define TPM2_CR50_SUB_CMD_NVMEM_ENABLE_COMMITS (21)
#define TPM2_CR50_SUB_CMD_TURN_UPDATE_ON (24)
#define TPM2_CR50_SUB_CMD_GET_REC_BTN (29)
#define TPM2_CR50_SUB_CMD_TPM_MODE (40)
#define TPM2_CR50_SUB_CMD_GET_BOOT_MODE (52)

/* Cr50 vendor-specific error codes. */
#define VENDOR_RC_ERR              0x00000500
enum cr50_vendor_rc {
	VENDOR_RC_INTERNAL_ERROR = (VENDOR_RC_ERR | 6),
	VENDOR_RC_NO_SUCH_SUBCOMMAND = (VENDOR_RC_ERR | 8),
	VENDOR_RC_NO_SUCH_COMMAND = (VENDOR_RC_ERR | 127),
};

enum cr50_tpm_mode {
	/*
	 * Default state: TPM is enabled, and may be set to either
	 * TPM_MODE_ENABLED or TPM_MODE_DISABLED.
	 */
	TPM_MODE_ENABLED_TENTATIVE = 0,

	/* TPM is enabled, and mode may not be changed. */
	TPM_MODE_ENABLED = 1,

	/* TPM is disabled, and mode may not be changed. */
	TPM_MODE_DISABLED = 2,

	TPM_MODE_INVALID,
};

/**
 * CR50 specific tpm command to enable nvmem commits before internal timeout
 * expires.
 */
uint32_t tlcl_cr50_enable_nvcommits(void);

/**
 * CR50 specific tpm command to restore header(s) of the dormant RO/RW
 * image(s) and in case there indeed was a dormant image, trigger reboot after
 * the timeout milliseconds. Note that timeout of zero means "NO REBOOT", not
 * "IMMEDIATE REBOOT".
 *
 * Return value indicates success or failure of accessing the TPM; in case of
 * success the number of restored headers is saved in num_restored_headers.
 */
uint32_t tlcl_cr50_enable_update(uint16_t timeout_ms,
				 uint8_t *num_restored_headers);

/**
 * CR50 specific tpm command to get the latched state of the recovery button.
 *
 * Return value indicates success or failure of accessing the TPM; in case of
 * success the recovery button state is saved in recovery_button_state.
 */
uint32_t tlcl_cr50_get_recovery_button(uint8_t *recovery_button_state);

/**
 * CR50 specific TPM command sequence to query the current TPM mode.
 *
 * Returns TPM_SUCCESS if TPM mode command completed, the Cr50 does not need a
 * reboot, and the tpm_mode parameter is set to the current TPM mode.
 * Returns TPM_E_MUST_REBOOT if TPM mode command completed, but the Cr50
 * requires a reboot.
 * Returns TPM_E_NO_SUCH_COMMAND if the Cr50 does not support the command.
 * Other returns value indicate a failure accessing the TPM.
 */
uint32_t tlcl_cr50_get_tpm_mode(uint8_t *tpm_mode);

/**
 * CR50 specific TPM command sequence to query the current boot mode.
 *
 * Returns TPM_SUCCESS if boot mode is successfully retrieved.
 * Returns TPM_E_* for errors.
 */
uint32_t tlcl_cr50_get_boot_mode(uint8_t *boot_mode);

/**
 * CR50 specific TPM command sequence to trigger an immediate reset to the Cr50
 * device after the specified timeout in milliseconds.  A timeout of zero means
 * "IMMEDIATE REBOOT".
 *
 * Return value indicates success or failure of accessing the TPM.
 */
uint32_t tlcl_cr50_immediate_reset(uint16_t timeout_ms);

#endif /* CR50_TSS_STRUCTURES_H_ */
