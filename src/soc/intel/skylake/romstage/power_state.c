/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc.
 * Copyright (C) 2015 Intel Corporation.
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

#include <arch/early_variables.h>
#include <arch/io.h>
#include <cbmem.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_def.h>
#include <reg_script.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <soc/iomap.h>
#include <soc/pmc.h>
#include <soc/pci_devs.h>
#include <soc/pm.h>
#include <soc/romstage.h>

static struct chipset_power_state power_state CAR_GLOBAL;

static void migrate_power_state(int is_recovery)
{
	struct chipset_power_state *ps_cbmem;
	struct chipset_power_state *ps_car;

	ps_car = car_get_var_ptr(&power_state);
	ps_cbmem = cbmem_add(CBMEM_ID_POWER_STATE, sizeof(*ps_cbmem));

	if (ps_cbmem == NULL) {
		printk(BIOS_DEBUG, "Not adding power state to cbmem!\n");
		return;
	}
	memcpy(ps_cbmem, ps_car, sizeof(*ps_cbmem));
}
ROMSTAGE_CBMEM_INIT_HOOK(migrate_power_state)

/* Return 0, 3, or 5 to indicate the previous sleep state. */
static uint32_t prev_sleep_state(struct chipset_power_state *ps)
{
	/* Default to S0. */
	uint32_t prev_sleep_state = SLEEP_STATE_S0;

	if (ps->pm1_sts & WAK_STS) {
		switch ((ps->pm1_cnt & SLP_TYP) >> SLP_TYP_SHIFT) {
#if IS_ENABLED(CONFIG_HAVE_ACPI_RESUME)
		case SLP_TYP_S3:
			prev_sleep_state = SLEEP_STATE_S3;
			break;
#endif
		case SLP_TYP_S5:
			prev_sleep_state = SLEEP_STATE_S5;
			break;
		}
		/* Clear SLP_TYP. */
		outl(ps->pm1_cnt & ~(SLP_TYP), ACPI_BASE_ADDRESS + PM1_CNT);
	}

	/*
	 * If waking from S3 determine if deep S3 is enabled. If not,
	 * need to check both deep sleep well and normal suspend well.
	 * Otherwise just check deep sleep well.
	 */
	if (prev_sleep_state == SLEEP_STATE_S3) {
		/* PWR_FLR represents deep sleep power well loss. */
		uint32_t mask = PWR_FLR;

		/* If deep s3 isn't enabled check the suspend well too. */
		if (!deep_s3_enabled())
			mask |= SUS_PWR_FLR;

		if (ps->gen_pmcon_b & mask)
			prev_sleep_state = SLEEP_STATE_S5;
	}

	return prev_sleep_state;
}

static void dump_power_state(struct chipset_power_state *ps)
{
	printk(BIOS_DEBUG, "PM1_STS:   %04x\n", ps->pm1_sts);
	printk(BIOS_DEBUG, "PM1_EN:    %04x\n", ps->pm1_en);
	printk(BIOS_DEBUG, "PM1_CNT:   %08x\n", ps->pm1_cnt);
	printk(BIOS_DEBUG, "TCO_STS:   %04x %04x\n",
	       ps->tco1_sts, ps->tco2_sts);

	printk(BIOS_DEBUG, "GPE0_STS:  %08x %08x %08x %08x\n",
	       ps->gpe0_sts[0], ps->gpe0_sts[1],
	       ps->gpe0_sts[2], ps->gpe0_sts[3]);
	printk(BIOS_DEBUG, "GPE0_EN:   %08x %08x %08x %08x\n",
	       ps->gpe0_en[0], ps->gpe0_en[1],
	       ps->gpe0_en[2], ps->gpe0_en[3]);

	printk(BIOS_DEBUG, "GEN_PMCON: %08x %08x\n",
	       ps->gen_pmcon_a, ps->gen_pmcon_b);

	printk(BIOS_DEBUG, "GBLRST_CAUSE: %08x %08x\n",
	       ps->gblrst_cause[0], ps->gblrst_cause[1]);

	printk(BIOS_DEBUG, "Previous Sleep State: S%d\n",
	       ps->prev_sleep_state);
}

/* Fill power state structure from ACPI PM registers */
struct chipset_power_state *fill_power_state(void)
{
	uint16_t tcobase;
	uint8_t *pmc;
	struct chipset_power_state *ps = car_get_var_ptr(&power_state);

	tcobase = pmc_tco_regs();

	ps->pm1_sts = inw(ACPI_BASE_ADDRESS + PM1_STS);
	ps->pm1_en = inw(ACPI_BASE_ADDRESS + PM1_EN);
	ps->pm1_cnt = inl(ACPI_BASE_ADDRESS + PM1_CNT);
	ps->tco1_sts = inw(tcobase + TCO1_STS);
	ps->tco2_sts = inw(tcobase + TCO2_STS);
	ps->gpe0_sts[0] = inl(ACPI_BASE_ADDRESS + GPE0_STS(0));
	ps->gpe0_sts[1] = inl(ACPI_BASE_ADDRESS + GPE0_STS(1));
	ps->gpe0_sts[2] = inl(ACPI_BASE_ADDRESS + GPE0_STS(2));
	ps->gpe0_sts[3] = inl(ACPI_BASE_ADDRESS + GPE0_STS(3));
	ps->gpe0_en[0] = inl(ACPI_BASE_ADDRESS + GPE0_EN(0));
	ps->gpe0_en[1] = inl(ACPI_BASE_ADDRESS + GPE0_EN(1));
	ps->gpe0_en[2] = inl(ACPI_BASE_ADDRESS + GPE0_EN(2));
	ps->gpe0_en[3] = inl(ACPI_BASE_ADDRESS + GPE0_EN(3));

	ps->gen_pmcon_a = pci_read_config32(PCH_DEV_PMC, GEN_PMCON_A);
	ps->gen_pmcon_b = pci_read_config32(PCH_DEV_PMC, GEN_PMCON_B);

	pmc = pmc_mmio_regs();
	ps->gblrst_cause[0] = read32(pmc + GBLRST_CAUSE0);
	ps->gblrst_cause[1] = read32(pmc + GBLRST_CAUSE1);

	ps->prev_sleep_state = prev_sleep_state(ps);

	dump_power_state(ps);

	return ps;
}
