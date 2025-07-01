/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/psp.h>
#include <console/console.h>
#include <device/mmio.h>

#include "psp_def.h"

uintptr_t psp_ftpm_base_address(void)
{
	const uintptr_t psp_base = get_psp_mmio_base();
	if (!psp_base)
		return 0;

	/* TPM MMIO space starts 0xA0 bytes before MBOX */
	return psp_base + CONFIG_PSPV2_MBOX_CMD_OFFSET - 0xA0;
}

/*
 * psp_ftpm_is_active
 *
 * Checks that fTPM CRB interface is available and active.
 */
bool psp_ftpm_is_active(void)
{
	uint32_t caps = 0;

	/* PSP must report fTPM capability support */
	if (psp_get_ftpm_capabilties(&caps) != CB_SUCCESS)
		return false;

	if (!(caps & MBOX_FTPM_CAP_TPM_SUPPORTED))
		return false;

	/* Must have valid MMIO base address */
	if (!psp_ftpm_base_address())
		return false;

	return true;
}

/*
 * psp_ftpm_needs_recovery
 *
 * Checks the PSP and returns the necessary recovery actions
 * to be taken. Recovery actions involve erasing parts of the SPI flash
 * and might possibly erase TPM secrets, preventing unsealing of
 * the OS.
 *
 * @psp_rpmc_nvram The PSP_RPMC_NVRAM FMAP regions must be cleared
 * @psp_nvram The PSP_NVRAM FMAP regions must be cleared
 * @psp_dir The fTPM driver in the PSP directory is missing or corrupted
 */
void psp_ftpm_needs_recovery(bool *psp_rpmc_nvram,
			     bool *psp_nvram,
			     bool *psp_dir)
{
	uint32_t psp_caps;
	uint32_t caps;

	*psp_rpmc_nvram = false;
	*psp_nvram = false;
	*psp_dir = false;

	if (psp_get_psp_capabilities(&psp_caps) != CB_SUCCESS) {
		printk(BIOS_ERR, "FTPM: Failed to get PSP capabilities\n");
		return;
	}

	if (psp_get_ftpm_capabilties(&caps) != CB_SUCCESS) {
		printk(BIOS_ERR, "FTPM: Failed to get fTPM capabilities\n");
		return;
	}

	if ((psp_caps & 0x5) == 0x4) {
		printk(BIOS_WARNING, "FTPM: PSP_RPMC_NVRAM region corrupted.\n");
		*psp_rpmc_nvram = true;
		*psp_nvram = true;
	}

	if (caps & MBOX_FTPM_CAP_TPM_REQ_FACTORY_RESET) {
		printk(BIOS_WARNING, "FTPM: PSP_NVRAM region corrupted.\n");
		*psp_nvram = true;
	}

	if (caps & MBOX_FTPM_CAP_FTPM_NEED_RECOVER)
		*psp_dir = true;
}
