/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <device/pci_ops.h>

void acpi_fill_fadt(acpi_fadt_t *fadt)
{
	u16 pmbase = pci_read_config16(pcidev_on_root(0x1f, 0), 0x40) & 0xfffe;


	fadt->pm1a_evt_blk = pmbase;
	fadt->pm1a_cnt_blk = pmbase + 0x4;
	fadt->pm_tmr_blk = pmbase + 0x8;
	fadt->gpe0_blk = pmbase + 0x28;

	fadt->pm1_evt_len = 4;
	fadt->pm1_cnt_len = 2;
	fadt->pm_tmr_len = 4;
	fadt->gpe0_blk_len = 8;

	fill_fadt_extended_pm_io(fadt);

	fadt->iapc_boot_arch = ACPI_FADT_LEGACY_DEVICES | ACPI_FADT_8042;

	fadt->flags |= ACPI_FADT_WBINVD | ACPI_FADT_C1_SUPPORTED | ACPI_FADT_SLEEP_BUTTON |
			ACPI_FADT_S4_RTC_WAKE | ACPI_FADT_PLATFORM_CLOCK;
}
