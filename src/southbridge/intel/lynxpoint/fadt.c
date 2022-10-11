/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/pci_ops.h>
#include <acpi/acpi.h>
#include <cpu/x86/smm.h>
#include <southbridge/intel/lynxpoint/pch.h>
#include "chip.h"

void acpi_fill_fadt(acpi_fadt_t *fadt)
{
	struct device *dev = pcidev_on_root(0x1f, 0);
	struct southbridge_intel_lynxpoint_config *cfg = dev->chip_info;
	u16 pmbase = get_pmbase();

	fadt->sci_int = 0x9;

	if (permanent_smi_handler()) {
		fadt->smi_cmd = APM_CNT;
		fadt->acpi_enable = APM_CNT_ACPI_ENABLE;
		fadt->acpi_disable = APM_CNT_ACPI_DISABLE;
	}

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

	/* P_LVLx not used */
	fadt->p_lvl2_lat = 101;
	fadt->p_lvl3_lat = 1001;
	fadt->duty_offset = 0;
	fadt->duty_width = 0;
	fadt->day_alrm = 0xd;
	fadt->mon_alrm = 0x00;
	fadt->iapc_boot_arch = ACPI_FADT_LEGACY_DEVICES | ACPI_FADT_8042;

	fadt->flags |= ACPI_FADT_WBINVD |
		       ACPI_FADT_C1_SUPPORTED |
		       ACPI_FADT_SLEEP_BUTTON |
		       ACPI_FADT_SEALED_CASE |
		       ACPI_FADT_S4_RTC_WAKE |
		       ACPI_FADT_PLATFORM_CLOCK;

	if (cfg && cfg->docking_supported)
		fadt->flags |= ACPI_FADT_DOCKING_SUPPORTED;

	fadt->x_pm1a_evt_blk.space_id = ACPI_ADDRESS_SPACE_IO;
	fadt->x_pm1a_evt_blk.bit_width = fadt->pm1_evt_len * 8;
	fadt->x_pm1a_evt_blk.bit_offset = 0;
	fadt->x_pm1a_evt_blk.access_size = ACPI_ACCESS_SIZE_WORD_ACCESS;
	fadt->x_pm1a_evt_blk.addrl = pmbase + PM1_STS;
	fadt->x_pm1a_evt_blk.addrh = 0x0;

	fadt->x_pm1a_cnt_blk.space_id = ACPI_ADDRESS_SPACE_IO;
	fadt->x_pm1a_cnt_blk.bit_width = fadt->pm1_cnt_len * 8;
	fadt->x_pm1a_cnt_blk.bit_offset = 0;
	fadt->x_pm1a_cnt_blk.access_size = ACPI_ACCESS_SIZE_WORD_ACCESS;
	fadt->x_pm1a_cnt_blk.addrl = fadt->pm1a_cnt_blk;
	fadt->x_pm1a_cnt_blk.addrh = 0x0;

	fadt->x_pm2_cnt_blk.space_id = ACPI_ADDRESS_SPACE_IO;
	fadt->x_pm2_cnt_blk.bit_width = fadt->pm2_cnt_len * 8;
	fadt->x_pm2_cnt_blk.bit_offset = 0;
	fadt->x_pm2_cnt_blk.access_size = ACPI_ACCESS_SIZE_BYTE_ACCESS;
	fadt->x_pm2_cnt_blk.addrl = pmbase + PM2_CNT;
	fadt->x_pm2_cnt_blk.addrh = 0x0;

	fadt->x_pm_tmr_blk.space_id = ACPI_ADDRESS_SPACE_IO;
	fadt->x_pm_tmr_blk.bit_width = fadt->pm_tmr_len * 8;
	fadt->x_pm_tmr_blk.bit_offset = 0;
	fadt->x_pm_tmr_blk.access_size = ACPI_ACCESS_SIZE_DWORD_ACCESS;
	fadt->x_pm_tmr_blk.addrl = fadt->pm_tmr_blk;
	fadt->x_pm_tmr_blk.addrh = 0x0;

	/*
	 * Windows 10 requires x_gpe0_blk to be set starting with FADT revision 5.
	 * The bit_width field intentionally overflows here.
	 * The OSPM can instead use the values in `fadt->gpe0_blk{,_len}`, which
	 * seems to work fine on Linux 5.0 and Windows 10.
	 */
	fadt->x_gpe0_blk.space_id = ACPI_ADDRESS_SPACE_IO;
	fadt->x_gpe0_blk.bit_width = fadt->gpe0_blk_len * 8;
	fadt->x_gpe0_blk.bit_offset = 0;
	fadt->x_gpe0_blk.access_size = ACPI_ACCESS_SIZE_BYTE_ACCESS;
	fadt->x_gpe0_blk.addrl = fadt->gpe0_blk;
	fadt->x_gpe0_blk.addrh = 0x0;
}
