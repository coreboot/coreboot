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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA, 02110-1301 USA
 */

#include <stdlib.h>
#include <console/console.h>
#include <cbmem.h>
#include <arch/acpi.h>

/* FIXME: Remove after CBMEM_INIT_HOOKS. */
#include <arch/early_variables.h>
#include <cpu/x86/gdt.h>
#include <console/cbmem_console.h>
#include <timestamp.h>

#if IS_ENABLED(CONFIG_LATE_CBMEM_INIT)

#if !defined(__PRE_RAM__)
void __attribute__((weak)) backup_top_of_ram(uint64_t ramtop)
{
	/* Do nothing. Chipset may have implementation to save ramtop in NVRAM. */
}

void set_top_of_ram(uint64_t ramtop)
{
	backup_top_of_ram(ramtop);
	cbmem_set_top((void*)(uintptr_t)ramtop);
}
#endif /* !__PRE_RAM__ */

unsigned long __attribute__((weak)) get_top_of_ram(void)
{
	printk(BIOS_WARNING, "WARNING: you need to define get_top_of_ram() for your chipset\n");
	return 0;
}

void *cbmem_top(void)
{
	/* Top of cbmem is at lowest usable DRAM address below 4GiB. */
	return (void *)get_top_of_ram();
}

#endif /* LATE_CBMEM_INIT */

void cbmem_run_init_hooks(void)
{
	/* Migrate car.global_data. */
	car_migrate_variables();

#if !defined(__PRE_RAM__)
	/* Relocate CBMEM console. */
	cbmemc_reinit();

	/* Relocate timestamps stash. */
	timestamp_reinit();

	move_gdt();
#endif
}

/* Something went wrong, our high memory area got wiped */
void cbmem_fail_resume(void)
{
#if !defined(__PRE_RAM__) && IS_ENABLED(CONFIG_HAVE_ACPI_RESUME)
	/* ACPI resume needs to be cleared in the fail-to-recover case, but that
	 * condition is only handled during ramstage. */
	acpi_fail_wakeup();
#endif
}
