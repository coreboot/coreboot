/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2018 Facebook Inc.
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

#include <types.h>
#include <stddef.h>
#include <bootstate.h>
#include <security/tpm/tspi.h>

#if IS_ENABLED(CONFIG_ARCH_X86)
#include <arch/acpi.h>
#endif

static void init_tpm_dev(void *unused)
{
#if IS_ENABLED(CONFIG_ARCH_X86)
	int s3resume = acpi_is_wakeup_s3();
	tpm_setup(s3resume);
#else
	tpm_setup(false);
#endif
}

BOOT_STATE_INIT_ENTRY(BS_DEV_INIT, BS_ON_ENTRY, init_tpm_dev, NULL);
