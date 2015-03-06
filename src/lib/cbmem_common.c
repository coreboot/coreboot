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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */
#include <console/console.h>
#include <cbmem.h>
#include <bootstate.h>
#include <rules.h>
#if IS_ENABLED(CONFIG_ARCH_X86) && !IS_ENABLED(CONFIG_EARLY_CBMEM_INIT)
#include <arch/acpi.h>
#endif

/* FIXME: Remove after CBMEM_INIT_HOOKS. */
#include <console/cbmem_console.h>
#include <timestamp.h>


/* FIXME: Replace with CBMEM_INIT_HOOKS API. */
#if !IS_ENABLED(CONFIG_ARCH_X86)
void cbmem_run_init_hooks(void)
{
	/* Relocate CBMEM console. */
	cbmemc_reinit();

	/* Relocate timestamps stash. */
	timestamp_reinit();
}

void __attribute__((weak)) cbmem_fail_resume(void)
{
}
#endif

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
