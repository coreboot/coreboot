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

#if !CONFIG_DYNAMIC_CBMEM
void get_cbmem_table(uint64_t *base, uint64_t *size)
{
	uint64_t top_of_ram = get_top_of_ram();

	if (top_of_ram >= HIGH_MEMORY_SIZE) {
		*base = top_of_ram - HIGH_MEMORY_SIZE;
		*size = HIGH_MEMORY_SIZE;
	} else {
		*base = 0;
		*size = 0;
	}
}

#if !defined(__PRE_RAM__)
void __attribute__((weak)) backup_top_of_ram(uint64_t ramtop)
{
	/* Do nothing. Chipset may have implementation to save ramtop in NVRAM. */
}

/* This is for compatibility with old boards only. Any new chipset and board
 * must implement get_top_of_ram() for both romstage and ramstage to support
 * early features like COLLECT_TIMESTAMPS and CBMEM_CONSOLE.
 */
void set_top_of_ram(uint64_t ramtop)
{
	backup_top_of_ram(ramtop);
	cbmem_late_set_table(ramtop - HIGH_MEMORY_SIZE, HIGH_MEMORY_SIZE);
}
#endif /* !__PRE_RAM__ */

#if CONFIG_BROKEN_CAR_MIGRATE || !defined(__PRE_RAM__)
unsigned long __attribute__((weak)) get_top_of_ram(void)
{
	printk(BIOS_WARNING, "WARNING: you need to define get_top_of_ram() for your chipset\n");
	return 0;
}
#endif

#else

void *cbmem_top(void)
{
	/* Top of cbmem is at lowest usable DRAM address below 4GiB. */
	return (void *)get_top_of_ram();
}

#endif /* DYNAMIC_CBMEM */

#if !defined(__PRE_RAM__)

/* ACPI resume needs to be cleared in the fail-to-recover case, but that
 * condition is only handled during ramstage. */
void cbmem_fail_resume(void)
{
#if CONFIG_HAVE_ACPI_RESUME
	/* Something went wrong, our high memory area got wiped */
	acpi_fail_wakeup();
#endif
}

#endif /* !__PRE_RAM__ */
