/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008-2009 coresystems GmbH
 * Copyright (C) 2014 Google Inc.
 * Copyright (C) 2017-2019 Intel Corporation.
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

#include <bootstate.h>
#include <console/console.h>
#include <device/mmio.h>
#include <device/device.h>
#include <device/pci_ops.h>
#include <intelblocks/pmc.h>
#include <intelblocks/pmclib.h>
#include <intelblocks/rtc.h>
#include <soc/pci_devs.h>
#include <soc/pm.h>

#include "chip.h"

/*
 * Set which power state system will be after reapplying
 * the power (from G3 State)
 */
void pmc_set_afterg3(struct device *dev, int s5pwr)
{
	uint8_t reg8;
	uint8_t *pmcbase = pmc_mmio_regs();

	reg8 = read8(pmcbase + GEN_PMCON_A);

	switch (s5pwr) {
	case MAINBOARD_POWER_STATE_OFF:
		reg8 |= 1;
		break;
	case MAINBOARD_POWER_STATE_ON:
		reg8 &= ~1;
		break;
	case MAINBOARD_POWER_STATE_PREVIOUS:
	default:
		break;
	}

	write8(pmcbase + GEN_PMCON_A, reg8);
}

/*
 * Set PMC register to know which state system should be after
 * power reapplied
 */
void pmc_soc_restore_power_failure(void)
{
	pmc_set_afterg3(PCH_DEV_PMC, CONFIG_MAINBOARD_POWER_FAILURE_STATE);
}

static void pm1_enable_pwrbtn_smi(void *unused)
{
	/*
	 * Enable power button SMI only before jumping to payload. This ensures
	 * that:
	 * 1. Power button SMI is enabled only after coreboot is done.
	 * 2. On resume path, power button SMI is not enabled and thus avoids
	 * any shutdowns because of power button presses due to power button
	 * press in resume path.
	 */
	pmc_update_pm1_enable(PWRBTN_EN);
}

BOOT_STATE_INIT_ENTRY(BS_PAYLOAD_LOAD, BS_ON_EXIT, pm1_enable_pwrbtn_smi, NULL);

static void config_deep_sX(uint32_t offset, uint32_t mask, int sx, int enable)
{
	uint32_t reg;
	uint8_t *pmcbase = pmc_mmio_regs();

	printk(BIOS_DEBUG, "%sabling Deep S%c\n",
		enable ? "En" : "Dis", sx + '0');
	reg = read32(pmcbase + offset);
	if (enable)
		reg |= mask;
	else
		reg &= ~mask;
	write32(pmcbase + offset, reg);
}

static void config_deep_s5(int on_ac, int on_dc)
{
	/* Treat S4 the same as S5. */
	config_deep_sX(S4_PWRGATE_POL, S4AC_GATE_SUS, 4, on_ac);
	config_deep_sX(S4_PWRGATE_POL, S4DC_GATE_SUS, 4, on_dc);
	config_deep_sX(S5_PWRGATE_POL, S5AC_GATE_SUS, 5, on_ac);
	config_deep_sX(S5_PWRGATE_POL, S5DC_GATE_SUS, 5, on_dc);
}

static void config_deep_s3(int on_ac, int on_dc)
{
	config_deep_sX(S3_PWRGATE_POL, S3AC_GATE_SUS, 3, on_ac);
	config_deep_sX(S3_PWRGATE_POL, S3DC_GATE_SUS, 3, on_dc);
}

static void config_deep_sx(uint32_t deepsx_config)
{
	uint32_t reg;
	uint8_t *pmcbase = pmc_mmio_regs();

	reg = read32(pmcbase + DSX_CFG);
	reg &= ~DSX_CFG_MASK;
	reg |= deepsx_config;
	write32(pmcbase + DSX_CFG, reg);
}

static void pch_power_options(struct device *dev)
{
	const char *state;

	const int pwr_on = CONFIG_MAINBOARD_POWER_FAILURE_STATE;

	/*
	 * Which state do we want to goto after g3 (power restored)?
	 * 0 == S5 Soft Off
	 * 1 == S0 Full On
	 * 2 == Keep Previous State
	 */
	switch (pwr_on) {
	case MAINBOARD_POWER_STATE_OFF:
		state = "off";
		break;
	case MAINBOARD_POWER_STATE_ON:
		state = "on";
		break;
	case MAINBOARD_POWER_STATE_PREVIOUS:
		state = "state keep";
		break;
	default:
		state = "undefined";
	}
	pmc_set_afterg3(dev, pwr_on);
	printk(BIOS_INFO, "Set power %s after power failure.\n", state);

	/* Set up GPE configuration. */
	pmc_gpe_init();
}

static void pmc_init(void *unused)
{
	struct device *dev = PCH_DEV_PMC;
	config_t *config = dev->chip_info;

	rtc_init();

	/* Initialize power management */
	pch_power_options(dev);

	config_deep_s3(config->deep_s3_enable_ac, config->deep_s3_enable_dc);
	config_deep_s5(config->deep_s5_enable_ac, config->deep_s5_enable_dc);
	config_deep_sx(config->deep_sx_config);
}

/*
* Initialize PMC controller.
*
* PMC controller gets hidden from PCI bus during FSP-Silicon init call.
* Hence PCI enumeration can't be used to initialize bus device and
* allocate resources.
*/
BOOT_STATE_INIT_ENTRY(BS_DEV_INIT_CHIPS, BS_ON_EXIT, pmc_init, NULL);

static void soc_acpi_mode_init(void *unused)
{
	/*
	 * PMC initialization happens earlier for this SoC because FSP-Silicon
	 * init hides PMC from PCI bus. However, pmc_set_acpi_mode, which
	 * disables ACPI mode doesn't need to happen that early and can be
	 * delayed till typical BS_DEV_INIT. This ensures that ACPI mode
	 * disabling happens the same way for all SoCs and hence the ordering of
	 * events is the same.
	 *
	 * This is important to ensure that the ordering does not break the
	 * assumptions of any other drivers (e.g. ChromeEC) which could be
	 * taking different actions based on disabling of ACPI (e.g. flushing of
	 * all EC hostevent bits).
	 *
	 * P.S.: This cannot be done as part of pmc_soc_init as PMC device is
	 * hidden and hence the PMC driver never gets enumerated and so init is
	 * not called for it.
	 */
	pmc_set_acpi_mode();
}

BOOT_STATE_INIT_ENTRY(BS_DEV_INIT, BS_ON_EXIT, soc_acpi_mode_init, NULL);
