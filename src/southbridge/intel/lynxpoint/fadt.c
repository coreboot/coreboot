/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/pci_ops.h>
#include <acpi/acpi.h>
#include <southbridge/intel/lynxpoint/pch.h>
#include "chip.h"

void acpi_fill_fadt(acpi_fadt_t *fadt)
{
	struct device *dev = pcidev_on_root(0x1f, 0);
	struct southbridge_intel_lynxpoint_config *cfg = dev->chip_info;
	u16 pmbase = get_pmbase();


	fadt->pm1a_evt_blk = pmbase + PM1_STS;
	fadt->pm1a_cnt_blk = pmbase + PM1_CNT;
	fadt->pm2_cnt_blk = pmbase + PM2_CNT;
	fadt->pm_tmr_blk = pmbase + PM1_TMR;
	if (pch_is_lp())
		fadt->gpe0_blk = pmbase + LP_GPE0_STS_1;
	else
		fadt->gpe0_blk = pmbase + GPE0_STS;

	/*
	 * Some of the lengths here are doubled. This is because they describe
	 * blocks containing two registers, where the size of each register
	 * is found by halving the block length. See Table 5-34 and section
	 * 4.8.3 of the ACPI specification for details.
	 */
	fadt->pm1_evt_len = 2 * 2;
	fadt->pm1_cnt_len = 2;
	fadt->pm2_cnt_len = 1;
	fadt->pm_tmr_len = 4;
	if (pch_is_lp())
		fadt->gpe0_blk_len = 2 * 16;
	else
		fadt->gpe0_blk_len = 2 * 8;

	fill_fadt_extended_pm_io(fadt);

	fadt->iapc_boot_arch = ACPI_FADT_LEGACY_DEVICES | ACPI_FADT_8042;

	fadt->flags |= ACPI_FADT_WBINVD |
		       ACPI_FADT_C1_SUPPORTED |
		       ACPI_FADT_SLEEP_BUTTON |
		       ACPI_FADT_SEALED_CASE |
		       ACPI_FADT_S4_RTC_WAKE |
		       ACPI_FADT_PLATFORM_CLOCK;

	if (cfg && cfg->docking_supported)
		fadt->flags |= ACPI_FADT_DOCKING_SUPPORTED;
}
