/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008-2009 coresystems GmbH
 * Copyright (C) 2014 Google Inc.
 * Copyright (C) 2015-2017 Intel Corporation.
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
#include <chip.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci_ops.h>
#include <intelblocks/pmc.h>
#include <intelblocks/pmclib.h>
#include <intelblocks/rtc.h>
#include <reg_script.h>
#include <soc/pci_devs.h>
#include <soc/pm.h>

void pmc_set_disb(void)
{
	/* Set the DISB after DRAM init */
	u32 disb_val;
	device_t dev = PCH_DEV_PMC;

	disb_val = pci_read_config32(dev, GEN_PMCON_A);
	disb_val |= DISB;

	/* Don't clear bits that are write-1-to-clear */
	disb_val &= ~(GBL_RST_STS | MS4V);
	pci_write_config32(dev, GEN_PMCON_A, disb_val);
}

#if ENV_RAMSTAGE
/* Fill up PMC resource structure */
int pmc_soc_get_resources(struct pmc_resource_config *cfg)
{
	cfg->pwrmbase_offset = PWRMBASE;
	cfg->pwrmbase_addr = PCH_PWRM_BASE_ADDRESS;
	cfg->pwrmbase_size = PCH_PWRM_BASE_SIZE;
	cfg->abase_offset = ABASE;
	cfg->abase_addr = ACPI_BASE_ADDRESS;
	cfg->abase_size = ACPI_BASE_SIZE;

	return 0;
}

static const struct reg_script pch_pmc_misc_init_script[] = {
	/* SLP_S4=4s, SLP_S3=50ms, disable SLP_X stretching after SUS loss. */
	REG_PCI_RMW16(GEN_PMCON_B,
			~(S4MAW_MASK | SLP_S3_MIN_ASST_WDTH_MASK),
			S4MAW_4S | SLP_S3_MIN_ASST_WDTH_50MS |
			DIS_SLP_X_STRCH_SUS_UP),
	/* Enable SCI and clear SLP requests. */
	REG_IO_RMW32(ACPI_BASE_ADDRESS + PM1_CNT, ~SLP_TYP, SCI_EN),
	REG_SCRIPT_END
};

static const struct reg_script pmc_write1_to_clear_script[] = {
	REG_PCI_OR32(GEN_PMCON_A, 0),
	REG_PCI_OR32(GEN_PMCON_B, 0),
	REG_PCI_OR32(GEN_PMCON_B, 0),
	REG_RES_OR32(PWRMBASE, GBLRST_CAUSE0, 0),
	REG_RES_OR32(PWRMBASE, GBLRST_CAUSE1, 0),
	REG_SCRIPT_END
};

/*
 * Set which power state system will be after reapplying
 * the power (from G3 State)
 */
static void pmc_set_afterg3(struct device *dev, int s5pwr)
{
	uint8_t reg8;

	reg8 = pci_read_config8(dev, GEN_PMCON_B);

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

	pci_write_config8(dev, GEN_PMCON_B, reg8);
}

static void pch_power_options(struct device *dev)
{
	const char *state;

	/* Get the chip configuration */
	int pwr_on = pmc_get_mainboard_power_failure_state_choice();

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

void pmc_soc_init(struct device *dev)
{
	const config_t *config = dev->chip_info;

	rtc_init();

	/* Initialize power management */
	pch_power_options(dev);

	/* Note that certain bits may be cleared from running script as
	 * certain bit fields are write 1 to clear. */
	reg_script_run_on_dev(dev, pch_pmc_misc_init_script);
	pmc_set_acpi_mode();

	config_deep_s3(config->deep_s3_enable_ac, config->deep_s3_enable_dc);
	config_deep_s5(config->deep_s5_enable_ac, config->deep_s5_enable_dc);
	config_deep_sx(config->deep_sx_config);

	/* Clear registers that contain write-1-to-clear bits. */
	reg_script_run_on_dev(dev, pmc_write1_to_clear_script);
}

/*
 * Set PMC register to know which state system should be after
 * power reapplied
 */
void pmc_soc_restore_power_failure(void)
{
	pmc_set_afterg3(PCH_DEV_PMC,
		pmc_get_mainboard_power_failure_state_choice());
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

#endif
