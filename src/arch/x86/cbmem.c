/*
 * This file is part of the coreboot project.
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

#include <stdlib.h>
#include <console/console.h>
#include <cbmem.h>
#include <arch/acpi.h>

#if IS_ENABLED(CONFIG_LATE_CBMEM_INIT)

void __attribute__((weak)) backup_top_of_ram(uint64_t ramtop)
{
	/* Do nothing. Chipset may have implementation to save ramtop in NVRAM.
	 */
}

unsigned long __attribute__((weak)) get_top_of_ram(void)
{
	return 0;
}

#endif /* LATE_CBMEM_INIT */

#if IS_ENABLED(CONFIG_CBMEM_TOP_BACKUP)

static void *ramtop_pointer;

void set_top_of_ram(uint64_t ramtop)
{
	backup_top_of_ram(ramtop);
	if (ENV_RAMSTAGE)
		ramtop_pointer = (void *)(uintptr_t)ramtop;
}

void *cbmem_top(void)
{
	/* Top of cbmem is at lowest usable DRAM address below 4GiB. */
	uintptr_t ramtop;

	if (ENV_RAMSTAGE && ramtop_pointer != NULL)
		return ramtop_pointer;

	ramtop = get_top_of_ram();

	if (ENV_RAMSTAGE)
		ramtop_pointer = (void *)ramtop;

	return (void *)ramtop;
}

#endif /* CBMEM_TOP_BACKUP */

/* Something went wrong, our high memory area got wiped */
void cbmem_fail_resume(void)
{
#if !defined(__PRE_RAM__) && IS_ENABLED(CONFIG_HAVE_ACPI_RESUME)
	/* ACPI resume needs to be cleared in the fail-to-recover case, but that
	 * condition is only handled during ramstage. */
	acpi_fail_wakeup();
#endif
}
