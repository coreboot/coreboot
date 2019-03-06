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
#include <arch/acpi.h>

#if CONFIG(CBMEM_TOP_BACKUP)

void *cbmem_top(void)
{
	static void *cbmem_top_backup;
	void *top_backup;

	if (ENV_RAMSTAGE && cbmem_top_backup != NULL)
		return cbmem_top_backup;

	/* Top of CBMEM is at highest usable DRAM address below 4GiB. */
	top_backup = (void *)restore_top_of_low_cacheable();

	if (ENV_RAMSTAGE)
		cbmem_top_backup = top_backup;

	return top_backup;
}

#endif /* CBMEM_TOP_BACKUP */

/* Something went wrong, our high memory area got wiped */
void cbmem_fail_resume(void)
{
#if !defined(__PRE_RAM__) && CONFIG(HAVE_ACPI_RESUME)
	/* ACPI resume needs to be cleared in the fail-to-recover case, but that
	 * condition is only handled during ramstage. */
	acpi_fail_wakeup();
#endif
}
