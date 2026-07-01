/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <acpi/acpi.h>
#include <amdblocks/acpi.h>
#include <amdblocks/acpimmio.h>
#include <amdblocks/smm.h>
#include <console/console.h>
#include <types.h>

void fch_power_button_handler(void)
{
	u16 pm1cnt;

	pm1cnt = acpi_read16(MMIO_ACPI_PM1_CNT_BLK);
	/* The power-button source can be pending while OSPM takes ownership. */
	if (pm1cnt & ACPI_PM1_CNT_SCIEN) {
		printk(BIOS_DEBUG, "SMI#: Power button with OSPM active, ignoring\n");
		return;
	}

	printk(BIOS_DEBUG, "SMI#: Power button, entering S5\n");

	acpi_clear_pm_gpe_status();
	set_pm1cnt_s5();

	fch_slp_typ_handler();
}
