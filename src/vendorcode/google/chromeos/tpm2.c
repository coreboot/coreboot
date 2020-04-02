/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <bootstate.h>
#include <console/console.h>
#include <security/tpm/tss.h>
#include <vb2_api.h>

static void disable_platform_hierarchy(void *unused)
{
	int ret;

	if (!CONFIG(TPM2))
		return;

	if (!CONFIG(RESUME_PATH_SAME_AS_BOOT))
		return;

	ret = tlcl_lib_init();

	if (ret != VB2_SUCCESS) {
		printk(BIOS_ERR, "tlcl_lib_init() failed: %x\n", ret);
		return;
	}

	ret = tlcl_disable_platform_hierarchy();
	if (ret != TPM_SUCCESS)
		printk(BIOS_ERR, "Platform hierarchy disablement failed: %x\n",
			ret);
}

BOOT_STATE_INIT_ENTRY(BS_OS_RESUME, BS_ON_ENTRY, disable_platform_hierarchy,
			NULL);
