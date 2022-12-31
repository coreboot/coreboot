/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootstate.h>
#include <commonlib/console/post_codes.h>
#include <console/console.h>
#include <ec/google/chromeec/ec.h>
#include <elog.h>
#include <halt.h>
#include <security/tpm/tss.h>
#include <vb2_api.h>
#include <security/vboot/misc.h>
#include <security/vboot/vboot_common.h>
#include <vendorcode/google/chromeos/chromeos.h>
#include <timestamp.h>

#define CR50_RESET_DELAY_MS 1000

void __weak mainboard_prepare_cr50_reset(void) {}

/**
 * Check if the Cr50 TPM state requires a chip reset of the Cr50 device.
 *
 * Returns 0 if the Cr50 TPM state is good or if the TPM_MODE command is
 * unsupported.  Returns 1 if the Cr50 requires a reset.
 */
static int cr50_is_reset_needed(void)
{
	int ret;
	uint8_t tpm_mode;

	ret = tlcl_cr50_get_tpm_mode(&tpm_mode);

	if (ret == TPM_E_NO_SUCH_COMMAND) {
		printk(BIOS_INFO,
		       "Cr50 does not support TPM mode command\n");
		/* Older Cr50 firmware, assume no Cr50 reset is required */
		return 0;
	}

	if (ret == TPM_E_MUST_REBOOT) {
		/*
		 * Cr50 indicated a reboot is required to restore TPM
		 * functionality.
		 */
		return 1;
	} else if (ret != TPM_SUCCESS)	{
		/* TPM command failed, continue booting. */
		printk(BIOS_ERR, "Attempt to get CR50 TPM mode failed: %x\n", ret);
		return 0;
	}

	/*
	 * If the TPM mode is not enabled-tentative, then the TPM mode is locked
	 * and cannot be changed.  Perform a Cr50 reset because vboot may need
	 * to disable TPM as part of booting an untrusted OS.
	 *
	 * This is not an expected state, as the Cr50 always sets the TPM mode
	 * to TPM_MODE_ENABLED_TENTATIVE during any TPM reset action.
	 */
	if (tpm_mode != TPM_MODE_ENABLED_TENTATIVE) {
		printk(BIOS_NOTICE,
		       "NOTICE: Unexpected Cr50 TPM mode (%d). "
		       "A Cr50 reset is required.\n", tpm_mode);
		return 1;
	}

	/* If TPM state is okay, no reset needed. */
	return 0;
}

static void enable_update(void *unused)
{
	int ret;
	int cr50_reset_reqd = 0;
	uint8_t num_restored_headers;

	/**
	 * Never update during manually-triggered recovery to ensure update
	 * cannot interfere. Non-manual VB2_RECOVERY_TRAIN_AND_REBOOT
	 * sometimes used to update in factory.
	 */
	if (vboot_get_context()->flags & VB2_CONTEXT_FORCE_RECOVERY_MODE)
		return;

	ret = tlcl_lib_init();

	if (ret != VB2_SUCCESS) {
		printk(BIOS_ERR, "tlcl_lib_init() failed for CR50 update: %x\n",
		       ret);
		return;
	}

	timestamp_add_now(TS_TPM_ENABLE_UPDATE_START);

	/* Reboot in 1000 ms if necessary. */
	ret = tlcl_cr50_enable_update(CR50_RESET_DELAY_MS,
				      &num_restored_headers);

	if (ret != TPM_SUCCESS) {
		printk(BIOS_ERR, "Attempt to enable CR50 update failed: %x\n",
		       ret);
		return;
	}

	if (!num_restored_headers) {
		/* If no headers were restored there is no reset forthcoming due
		 * to a Cr50 firmware update.  Also check if the Cr50 TPM mode
		 * requires a reset.
		 *
		 * TODO: to eliminate a TPM command during every boot, the
		 * TURN_UPDATE_ON command could be enhanced/replaced in the Cr50
		 * firmware to perform the TPM mode/key-ladder check in addition
		 * to the FW version check.
		 */

		/*
		 * If the Cr50 doesn't requires a reset, continue booting.
		 */
		cr50_reset_reqd = cr50_is_reset_needed();
		if (!cr50_reset_reqd) {
			timestamp_add_now(TS_TPM_ENABLE_UPDATE_END);
			return;
		}

		printk(BIOS_INFO, "Waiting for CR50 reset to enable TPM.\n");
		elog_add_event(ELOG_TYPE_CR50_NEED_RESET);
	} else {
		printk(BIOS_INFO,
		       "Waiting for CR50 reset to pick up update.\n");
		elog_add_event(ELOG_TYPE_CR50_UPDATE);
	}

	/* Give mainboard a chance to take action */
	mainboard_prepare_cr50_reset();

	/* clear current post code avoid chatty eventlog on subsequent boot*/
	post_code(POST_CODE_CLEAR);

	/*
	 * Older Cr50 firmware doesn't support the timeout parameter for the
	 * immediate reset request, so the reset request must be sent after
	 * the mainboard specific code runs.
	 */
	if (cr50_reset_reqd) {
		ret = tlcl_cr50_immediate_reset(CR50_RESET_DELAY_MS);

		if (ret != TPM_SUCCESS) {
			/*
			 * Reset request failed due to TPM error, continue
			 * booting but the current boot will likely end up at
			 * the recovery screen.
			 */
			printk(BIOS_ERR, "Attempt to reset CR50 failed: %x\n",
			       ret);
			return;
		}
	}

	if (CONFIG(POWER_OFF_ON_CR50_UPDATE))
		poweroff();
	halt();
}
BOOT_STATE_INIT_ENTRY(BS_PAYLOAD_LOAD, BS_ON_ENTRY, enable_update, NULL);
