/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/pci_ops.h>
#include <acpi/acpi.h>
#include <version.h>

/* FIXME: This needs to go into a separate .h file
 * to be included by the ich7 smi handler, ich7 smi init
 * code and the mainboard fadt.
 */
#define APM_CNT		0x0   /* ACPI mode only */
#define   CST_CONTROL	0x85
#define   PST_CONTROL	0x0
#define   ACPI_DISABLE	0xAA
#define   ACPI_ENABLE	0x55
#define   S4_BIOS	0x77
#define   GNVS_UPDATE   0xea

void acpi_fill_fadt(acpi_fadt_t *fadt)
{
	u16 pmbase = pci_read_config16(pcidev_on_root(0x1f, 0), 0x40) & 0xfffe;

	fadt->sci_int = 0x9;

	if (permanent_smi_handler()) {
		fadt->smi_cmd = APM_CNT;
		fadt->acpi_enable = ACPI_ENABLE;
		fadt->acpi_disable = ACPI_DISABLE;
		fadt->pstate_cnt = PST_CONTROL;
	}

	fadt->pm1a_evt_blk = pmbase;
	fadt->pm1a_cnt_blk = pmbase + 0x4;
	fadt->pm_tmr_blk = pmbase + 0x8;
	fadt->gpe0_blk = pmbase + 0x28;

	fadt->pm1_evt_len = 4;
	fadt->pm1_cnt_len = 2;
	fadt->pm_tmr_len = 4;
	fadt->gpe0_blk_len = 8;
	fadt->p_lvl2_lat = 1;
	fadt->p_lvl3_lat = 85;
	fadt->duty_offset = 1;
	fadt->duty_width = 0;
	fadt->day_alrm = 0xd;
	fadt->mon_alrm = 0x00;
	fadt->iapc_boot_arch = ACPI_FADT_LEGACY_DEVICES | ACPI_FADT_8042;

	fadt->flags |= ACPI_FADT_WBINVD | ACPI_FADT_C1_SUPPORTED |
			ACPI_FADT_C2_MP_SUPPORTED | ACPI_FADT_SLEEP_BUTTON |
			ACPI_FADT_S4_RTC_WAKE | ACPI_FADT_PLATFORM_CLOCK;

	fadt->x_pm1a_evt_blk.space_id = ACPI_ADDRESS_SPACE_IO;
	fadt->x_pm1a_evt_blk.bit_width = fadt->pm1_evt_len * 8;
	fadt->x_pm1a_evt_blk.bit_offset = 0;
	fadt->x_pm1a_evt_blk.access_size = ACPI_ACCESS_SIZE_UNDEFINED;
	fadt->x_pm1a_evt_blk.addrl = pmbase;
	fadt->x_pm1a_evt_blk.addrh = 0x0;

	fadt->x_pm1a_cnt_blk.space_id = ACPI_ADDRESS_SPACE_IO;
	fadt->x_pm1a_cnt_blk.bit_width = fadt->pm1_cnt_len * 8;
	fadt->x_pm1a_cnt_blk.bit_offset = 0;
	fadt->x_pm1a_cnt_blk.access_size = ACPI_ACCESS_SIZE_UNDEFINED;
	fadt->x_pm1a_cnt_blk.addrl = pmbase + 0x4;
	fadt->x_pm1a_cnt_blk.addrh = 0x0;

	fadt->x_pm_tmr_blk.space_id = ACPI_ADDRESS_SPACE_IO;
	fadt->x_pm_tmr_blk.bit_width = fadt->pm_tmr_len * 8;
	fadt->x_pm_tmr_blk.bit_offset = 0;
	fadt->x_pm_tmr_blk.access_size = ACPI_ACCESS_SIZE_UNDEFINED;
	fadt->x_pm_tmr_blk.addrl = pmbase + 0x8;
	fadt->x_pm_tmr_blk.addrh = 0x0;

	fadt->x_gpe0_blk.space_id = ACPI_ADDRESS_SPACE_IO;
	fadt->x_gpe0_blk.bit_width = fadt->gpe0_blk_len * 8;
	fadt->x_gpe0_blk.bit_offset = 0;
	fadt->x_gpe0_blk.access_size = ACPI_ACCESS_SIZE_UNDEFINED;
	fadt->x_gpe0_blk.addrl = pmbase + 0x28;
	fadt->x_gpe0_blk.addrh = 0x0;
}
