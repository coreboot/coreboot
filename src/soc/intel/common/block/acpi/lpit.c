/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <acpi/acpi.h>
#include <cpu/intel/msr.h>
#include <soc/iomap.h>
#include <soc/pmc.h>
#include <stdint.h>

unsigned long acpi_fill_lpit(unsigned long current)
{
	uint16_t uid = 0;
	acpi_lpi_desc_ncst_t *pkg_counter;
	acpi_lpi_desc_ncst_t *sys_counter;

	/*
	 * Package C10 (PC10) residency counter
	 */
	pkg_counter = (void *)current;
	current += acpi_create_lpi_desc_ncst((void *)current, uid++);

	/* MWAIT LPI state entry trigger */
	pkg_counter->entry_trigger.addrl		= 0x60; /* MWAIT(6,0) / HW C10 */
	pkg_counter->entry_trigger.bit_offset		= ACPI_FFIXEDHW_CLASS_MWAIT;
	pkg_counter->entry_trigger.bit_width		= ACPI_FFIXEDHW_VENDOR_INTEL;
	pkg_counter->entry_trigger.space_id		= ACPI_ADDRESS_SPACE_FIXED;
	pkg_counter->entry_trigger.access_size		= ACPI_ACCESS_SIZE_UNDEFINED;

	/* PC10 residency counter */
	pkg_counter->residency_counter.addrl		= MSR_PKG_C10_RESIDENCY;
	pkg_counter->residency_counter.bit_offset	=  0;
	pkg_counter->residency_counter.bit_width	= 64;
	pkg_counter->residency_counter.space_id		= ACPI_ADDRESS_SPACE_FIXED;
	pkg_counter->residency_counter.access_size	= ACPI_ACCESS_SIZE_UNDEFINED;
	pkg_counter->counter_frequency			= ACPI_LPIT_CTR_FREQ_TSC;

	/* Min. residency and worst-case latency (from FSP and vendor dumps) */
	pkg_counter->min_residency			= 30000; /* break-even: 30 ms */
	pkg_counter->max_latency			=  3000; /* worst-case latency: 3 ms */

	/*
	 * System (Slp_S0) residency counter
	 */
	sys_counter = (void *)current;
	current += acpi_create_lpi_desc_ncst((void *)current, uid++);

	/* MWAIT LPI state entry trigger */
	sys_counter->entry_trigger.addrl		= 0x60; /* MWAIT(6,0) / HW C10 */
	sys_counter->entry_trigger.bit_offset		= ACPI_FFIXEDHW_CLASS_MWAIT;
	sys_counter->entry_trigger.bit_width		= ACPI_FFIXEDHW_VENDOR_INTEL;
	sys_counter->entry_trigger.space_id		= ACPI_ADDRESS_SPACE_FIXED;
	sys_counter->entry_trigger.access_size		= ACPI_ACCESS_SIZE_UNDEFINED;

	/* Slp_S0 residency counter */
	sys_counter->residency_counter.addrl		= PCH_PWRM_BASE_ADDRESS + SLP_S0_RES;
	sys_counter->residency_counter.bit_offset	=  0;
	sys_counter->residency_counter.bit_width	= 32;
	sys_counter->residency_counter.space_id		= ACPI_ADDRESS_SPACE_MEMORY;
	sys_counter->residency_counter.access_size	= ACPI_ACCESS_SIZE_DWORD_ACCESS;
	sys_counter->counter_frequency			= ACPI_LPIT_CTR_FREQ_TSC;

	/* Min. residency and worst-case latency (from FSP and vendor dumps) */
	sys_counter->min_residency			= 30000; /* break-even: 30 ms */
	sys_counter->max_latency			=  3000; /* worst-case latency: 3 ms */

	return current;
}
