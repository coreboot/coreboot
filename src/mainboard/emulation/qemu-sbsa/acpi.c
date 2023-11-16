/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <mainboard/addressmap.h>


void acpi_fill_fadt(acpi_fadt_t *fadt)
{
	fadt->ARM_boot_arch |= ACPI_FADT_ARM_PSCI_COMPLIANT;
}

unsigned long acpi_fill_madt(unsigned long current)
{
	return current;
}

uintptr_t platform_get_gicd_base(void)
{
	return SBSA_GIC_DIST;
}

uintptr_t platform_get_gicr_base(void)
{
	return SBSA_GIC_REDIST;
}

#define SEC_EL1_TIMER_GISV 0x1d
#define NONSEC_EL1_TIMER_GSIV 0x1e
#define VIRTUAL_TIMER_GSIV 0x1b
#define NONSEC_EL2_TIMER_GSIV 0x1a

#define SBSA_TIMER_FLAGS (ACPI_GTDT_INTERRUPT_POLARITY | ACPI_GTDT_ALWAYS_ON)

void acpi_soc_fill_gtdt(acpi_gtdt_t *gtdt)
{
	/* This value is optional if the system implements EL3 (Security
	   Extensions). If not provided, this field must be 0xFFFFFFFFFFFFFFFF. */
	gtdt->counter_block_address = UINT64_MAX;
	gtdt->secure_el1_interrupt = SEC_EL1_TIMER_GISV;
	gtdt->secure_el1_flags = SBSA_TIMER_FLAGS;
	gtdt->non_secure_el1_interrupt = NONSEC_EL1_TIMER_GSIV;
	gtdt->non_secure_el1_flags = SBSA_TIMER_FLAGS;
	gtdt->virtual_timer_interrupt = VIRTUAL_TIMER_GSIV;
	gtdt->virtual_timer_flags = SBSA_TIMER_FLAGS;
	gtdt->non_secure_el2_interrupt = NONSEC_EL2_TIMER_GSIV;
	gtdt->non_secure_el2_flags = SBSA_TIMER_FLAGS;
	/* This value is optional if the system implements EL3
	   (Security Extensions). If not provided, this field must be
	   0xFFFFFFFFFFFFFFF. */
	gtdt->counter_read_block_address = UINT64_MAX;
}

#define WD_TIMER_GSIV 0x30

unsigned long acpi_soc_gtdt_add_timers(uint32_t *count, unsigned long current)
{
	(*count)++;
	return acpi_gtdt_add_watchdog(current, SBSA_GWDT_REFRESH, SBSA_GWDT_CONTROL,
				      WD_TIMER_GSIV, 0);
}
