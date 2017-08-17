/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2009 coresystems GmbH
 * Copyright (C) 2014 Google Inc.
 * Copyright (C) 2017 Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <arch/acpi.h>
#include <arch/acpigen.h>
#include <arch/cpu.h>
#include <arch/io.h>
#include <arch/ioapic.h>
#include <arch/smp/mpspec.h>
#include <cbmem.h>
#include <chip.h>
#include <cpu/cpu.h>
#include <ec/google/chromeec/ec.h>
#include <intelblocks/cpulib.h>
#include <intelblocks/pmclib.h>
#include <intelblocks/acpi.h>
#include <soc/cpu.h>
#include <soc/iomap.h>
#include <soc/nvs.h>
#include <soc/pci_devs.h>
#include <soc/pm.h>
#include <vendorcode/google/chromeos/gnvs.h>
#include <wrdd.h>

void soc_fill_fadt(acpi_fadt_t *fadt)
{
	const uint16_t pmbase = ACPI_BASE_ADDRESS;
	const struct device *dev = PCH_DEV_LPC;
	const struct soc_intel_cannonlake_config *config = dev->chip_info;

	if (config->PmTimerDisabled != 0)
		return;

	fadt->pm_tmr_blk = pmbase + PM1_TMR;
	fadt->pm_tmr_len = 4;
	fadt->x_pm_tmr_blk.space_id = 1;
	fadt->x_pm_tmr_blk.bit_width = fadt->pm_tmr_len * 8;
	fadt->x_pm_tmr_blk.bit_offset = 0;
	fadt->x_pm_tmr_blk.resv = 0;
	fadt->x_pm_tmr_blk.addrl = pmbase + PM1_TMR;
	fadt->x_pm_tmr_blk.addrh = 0x0;
}
uint32_t soc_read_sci_irq_select(void)
{
	uintptr_t pmc_bar = soc_read_pmc_base();
	return read32((void *)pmc_bar + IRQ_REG);
}

void acpi_create_gnvs(struct global_nvs_t *gnvs)
{
	const struct device *dev = PCH_DEV_LPC;
	const struct soc_intel_cannonlake_config *config = dev->chip_info;

	/* Set unknown wake source */
	gnvs->pm1i = -1;

	/* CPU core count */
	gnvs->pcnt = dev_count_cpu();

	if (IS_ENABLED(CONFIG_CONSOLE_CBMEM))
	/* Update the mem console pointer. */
	gnvs->cbmc = (uintptr_t)cbmem_find(CBMEM_ID_CONSOLE);

	if (IS_ENABLED(CONFIG_CHROMEOS)) {
		/* Initialize Verified Boot data */
		chromeos_init_vboot(&(gnvs->chromeos));
		if (IS_ENABLED(CONFIG_EC_GOOGLE_CHROMEEC)) {
			gnvs->chromeos.vbt2 = google_ec_running_ro() ?
				ACTIVE_ECFW_RO : ACTIVE_ECFW_RW;
		} else
			gnvs->chromeos.vbt2 = ACTIVE_ECFW_RO;
	}

	/* Enable DPTF based on mainboard configuration */
	gnvs->dpte = config->dptf_enable;

	/* Fill in the Wifi Region id */
	gnvs->cid1 = wifi_regulatory_domain();

	/* Set USB2/USB3 wake enable bitmaps. */
	gnvs->u2we = config->usb2_wake_enable_bitmap;
	gnvs->u3we = config->usb3_wake_enable_bitmap;
}

uint32_t acpi_fill_soc_wake(uint32_t generic_pm1_en,
			    const struct chipset_power_state *ps)
{
	/*
	 * WAK_STS bit is set when the system is in one of the sleep states
	 * (via the SLP_EN bit) and an enabled wake event occurs. Upon setting
	 * this bit, the PMC will transition the system to the ON state and
	 * can only be set by hardware and can only be cleared by writing a one
	 * to this bit position.
	 */

	generic_pm1_en |= WAK_STS | RTC_EN | PWRBTN_EN;
	return generic_pm1_en;
}

int soc_madt_sci_irq_polarity(int sci)
{
	return MP_IRQ_POLARITY_HIGH;
}
