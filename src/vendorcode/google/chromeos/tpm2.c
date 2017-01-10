/*
 * This file is part of the coreboot project.
 *
 * Copyright 2017 Google Inc.
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

#include <bootstate.h>
#include <console/console.h>
#include <tpm_lite/tlcl.h>
#include <vb2_api.h>

static void disable_platform_hierarchy(void *unused)
{
	int ret;

	if (!IS_ENABLED(CONFIG_TPM2))
		return;

	if (!IS_ENABLED(CONFIG_RESUME_PATH_SAME_AS_BOOT))
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
