/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * ACPI - create the Fixed ACPI Description Tables (FADT)
 */

#include <console/console.h>
#include <acpi/acpi.h>
#include <acpi/acpigen.h>
#include <device/pci_ops.h>
#include <arch/ioapic.h>
#include <arch/smp/mpspec.h>
#include <cpu/amd/cpuid.h>
#include <device/device.h>
#include <device/pci.h>
#include <gpio.h>
#include <amdblocks/acpimmio.h>
#include <amdblocks/acpi.h>
#include <amdblocks/chip.h>
#include <amdblocks/cpu.h>
#include <amdblocks/ioapic.h>
#include <soc/pci_devs.h>
#include <soc/southbridge.h>
#include "chip.h"

/*
 * Reference section 5.2.9 Fixed ACPI Description Table (FADT)
 * in the ACPI 3.0b specification.
 */
void acpi_fill_fadt(acpi_fadt_t *fadt)
{
	const struct soc_amd_common_config *cfg = soc_get_common_config();

	printk(BIOS_DEBUG, "pm_base: 0x%04x\n", ACPI_IO_BASE);


	fadt->pm1a_evt_blk = ACPI_PM_EVT_BLK;
	fadt->pm1a_cnt_blk = ACPI_PM1_CNT_BLK;
	fadt->pm_tmr_blk = ACPI_PM_TMR_BLK;
	fadt->gpe0_blk = ACPI_GPE0_BLK;

	fadt->pm1_evt_len = 4;	/* 32 bits */
	fadt->pm1_cnt_len = 2;	/* 16 bits */
	fadt->pm_tmr_len = 4;	/* 32 bits */
	fadt->gpe0_blk_len = 8;	/* 64 bits */

	fill_fadt_extended_pm_io(fadt);

	fadt->iapc_boot_arch = cfg->fadt_boot_arch; /* legacy free default */
	fadt->flags |=	ACPI_FADT_WBINVD | /* See table 5-34 ACPI 6.3 spec */
			ACPI_FADT_C1_SUPPORTED |
			ACPI_FADT_S4_RTC_WAKE |
			ACPI_FADT_32BIT_TIMER |
			ACPI_FADT_PCI_EXPRESS_WAKE |
			ACPI_FADT_PLATFORM_CLOCK |
			ACPI_FADT_S4_RTC_VALID |
			ACPI_FADT_REMOTE_POWER_ON;
	fadt->flags |= cfg->fadt_flags; /* additional board-specific flags */
}

unsigned long soc_acpi_write_tables(const struct device *device, unsigned long current,
				    acpi_rsdp_t *rsdp)
{
	/* CRAT */
	current = acpi_add_crat_table(current, rsdp);

	/* IVRS */
	current = acpi_add_ivrs_table(current, rsdp);

	/* Add SRAT, MSCT, SLIT if needed in the future */

	if (CONFIG(PLATFORM_USES_FSP2_0))
		current = acpi_add_fsp_tables(current, rsdp);

	return current;
}

const acpi_cstate_t cstate_cfg_table[] = {
	[0] = {
		.ctype = 1,
		.latency = 1,
		.power = 0,
	},
	[1] = {
		.ctype = 2,
		.latency = 400,
		.power = 0,
	},
};

const acpi_cstate_t *get_cstate_config_data(size_t *size)
{
	*size = ARRAY_SIZE(cstate_cfg_table);
	return cstate_cfg_table;
}
