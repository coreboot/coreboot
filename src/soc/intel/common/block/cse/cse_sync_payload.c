/* SPDX-License-Identifier: GPL-2.0-only */

#include <cbmem.h>
#include <console/console.h>
#include <intelblocks/cse.h>
#include <intelblocks/cse_lite.h>

static void preram_create_cbmem_cse_info_for_payload(int is_recovery)
{
	/*
	 * CBMEM_ID_CSE_INFO will be used by the payload to -
	 * 1. Keep ISH firmware version on consecutive boots.
	 * 2. Track state of PSR data during CSE downgrade operation.
	 */
	void *temp = cbmem_add(CBMEM_ID_CSE_INFO, sizeof(struct cse_specific_info));
	if (!temp)
		printk(BIOS_ERR, "cse_lite: Couldn't create CBMEM_ID_CSE_INFO\n");

	/*
	 * CBMEM_ID_CSE_BP_INFO will be used by the payload to keep CSE
	 * boot partition information on consecutive boots.
	 */
	temp = cbmem_add(CBMEM_ID_CSE_BP_INFO, sizeof(struct get_bp_info_rsp));
	if (!temp)
		printk(BIOS_ERR, "cse_lite: Couldn't create CBMEM_ID_CSE_BP_INFO\n");
}

CBMEM_CREATION_HOOK(preram_create_cbmem_cse_info_for_payload);
