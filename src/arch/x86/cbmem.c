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
#include <cbmem.h>
#include <compiler.h>
#include <arch/acpi.h>

#if IS_ENABLED(CONFIG_LATE_CBMEM_INIT)

void __weak backup_top_of_low_cacheable(uintptr_t ramtop)
{
	/* Do nothing. Chipset may have implementation to save ramtop in NVRAM.
	 */
}

uintptr_t __weak restore_top_of_low_cacheable(void)
{
	return 0;
}

#endif /* LATE_CBMEM_INIT */

#if IS_ENABLED(CONFIG_CBMEM_TOP_BACKUP)

static void *cbmem_top_backup;

void set_late_cbmem_top(uintptr_t ramtop)
{
	backup_top_of_low_cacheable(ramtop);
	if (ENV_RAMSTAGE)
		cbmem_top_backup = (void *)ramtop;
}

/* Top of CBMEM is at highest usable DRAM address below 4GiB. */
uintptr_t __weak restore_cbmem_top(void)
{
	if (IS_ENABLED(CONFIG_LATE_CBMEM_INIT) && ENV_ROMSTAGE)
		if (!acpi_is_wakeup_s3())
			return 0;

	return restore_top_of_low_cacheable();
}

void *cbmem_top(void)
{
	uintptr_t top_backup;

	if (ENV_RAMSTAGE && cbmem_top_backup != NULL)
		return cbmem_top_backup;

	top_backup = restore_cbmem_top();

	if (ENV_RAMSTAGE)
		cbmem_top_backup = (void *)top_backup;

	return (void *)top_backup;
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
