/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google, Inc.
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
#include <console/console.h>
#include <cbmem.h>
#include <compiler.h>
#include <bootstate.h>
#include <rules.h>
#include <symbols.h>
#if IS_ENABLED(CONFIG_ARCH_X86) && !IS_ENABLED(CONFIG_EARLY_CBMEM_INIT)
#include <arch/acpi.h>
#endif

void cbmem_run_init_hooks(int is_recovery)
{
	cbmem_init_hook_t *init_hook_ptr =
		(cbmem_init_hook_t *)&_cbmem_init_hooks;
	cbmem_init_hook_t *einit_hook_ptr =
		(cbmem_init_hook_t *)&_ecbmem_init_hooks;

	if (_cbmem_init_hooks_size == 0)
		return;

	while (init_hook_ptr !=  einit_hook_ptr) {
		(*init_hook_ptr)(is_recovery);
		init_hook_ptr++;
	}
}

void __weak cbmem_fail_resume(void)
{
}

#if ENV_RAMSTAGE && !IS_ENABLED(CONFIG_EARLY_CBMEM_INIT)
static void init_cbmem_post_device(void *unused)
{
	if (acpi_is_wakeup())
		cbmem_initialize();
	else
		cbmem_initialize_empty();
}

BOOT_STATE_INIT_ENTRY(BS_POST_DEVICE, BS_ON_ENTRY,
			init_cbmem_post_device, NULL);
#endif
