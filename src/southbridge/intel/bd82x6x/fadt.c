/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/pci_ops.h>
#include <acpi/acpi.h>
#include <southbridge/intel/common/pmutil.h>
#include "chip.h"

void acpi_fill_fadt(acpi_fadt_t *fadt)
{
	struct device *dev = pcidev_on_root(0x1f, 0);
	struct southbridge_intel_bd82x6x_config *chip = dev->chip_info;
	u16 pmbase = pci_read_config16(dev, 0x40) & 0xfffe;


	fadt->pm1a_evt_blk = pmbase;
	fadt->pm1a_cnt_blk = pmbase + PM1_CNT;
	fadt->pm2_cnt_blk = pmbase + PM2_CNT;
	fadt->pm_tmr_blk = pmbase + PM1_TMR;
	fadt->gpe0_blk = pmbase + GPE0_STS;

	fadt->pm1_evt_len = 4;
	fadt->pm1_cnt_len = 2;
	fadt->pm2_cnt_len = 1;
	fadt->pm_tmr_len = 4;
	fadt->gpe0_blk_len = 16;

	fill_fadt_extended_pm_io(fadt);

	fadt->iapc_boot_arch = ACPI_FADT_LEGACY_DEVICES | ACPI_FADT_8042;

	fadt->flags |= ACPI_FADT_WBINVD |
			ACPI_FADT_C1_SUPPORTED |
			ACPI_FADT_SLEEP_BUTTON |
			ACPI_FADT_SEALED_CASE |
			ACPI_FADT_S4_RTC_WAKE |
			ACPI_FADT_PLATFORM_CLOCK;

	if (chip->docking_supported) {
		fadt->flags |= ACPI_FADT_DOCKING_SUPPORTED;
	}
}
