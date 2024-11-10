/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <soc/iomap.h>
#include <soc/pm.h>
#include <static.h>
#include "chip.h"

void soc_fill_fadt(acpi_fadt_t *fadt)
{
	const uint16_t pmbase = ACPI_BASE_ADDRESS;
	config_t *config = config_of_soc();

	fadt->pm2_cnt_blk = pmbase + PM2_CNT;
	fadt->pm_tmr_blk = pmbase + PM1_TMR;

	fadt->pm2_cnt_len = 1;
	fadt->pm_tmr_len = 4;

	fill_fadt_extended_pm_io(fadt);

	fadt->iapc_boot_arch = ACPI_FADT_LEGACY_FREE;
	if (!CONFIG(NO_FADT_8042))
		fadt->iapc_boot_arch |= ACPI_FADT_8042;

	if (config->s0ix_enable)
		fadt->flags |= ACPI_FADT_LOW_PWR_IDLE_S0;

}
