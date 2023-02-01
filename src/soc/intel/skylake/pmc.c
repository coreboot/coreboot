/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/io.h>
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
	const config_t *config = config_of(dev);
	uint8_t *const pwrmbase = pmc_mmio_regs();
	uint32_t reg32;

	rtc_init();

	pmc_set_power_failure_state(true);
	pmc_gpe_init();

	/* SLP_S4=4s, SLP_S3=50ms, disable SLP_X stretching after SUS loss. */
	pci_update_config32(dev, GEN_PMCON_B, ~(S4MAW_MASK | SLP_S3_MIN_ASST_WDTH_MASK),
			    S4MAW_4S | SLP_S3_MIN_ASST_WDTH_50MS | DIS_SLP_X_STRCH_SUS_UP);

	/* Enable SCI and clear SLP requests. */
	reg32 = inl(ACPI_BASE_ADDRESS + PM1_CNT);
	reg32 &= ~SLP_TYP;
	reg32 |= SCI_EN;
	outl(reg32, ACPI_BASE_ADDRESS + PM1_CNT);

	pmc_set_acpi_mode();

	config_deep_s3(config->deep_s3_enable_ac, config->deep_s3_enable_dc);
	config_deep_s5(config->deep_s5_enable_ac, config->deep_s5_enable_dc);
	config_deep_sx(config->deep_sx_config);

	/* Clear registers that contain write-1-to-clear bits. */
	pci_or_config32(dev, GEN_PMCON_B, 0);
	pci_or_config32(dev, GEN_PMCON_B, 0);
	setbits32(pwrmbase + GBLRST_CAUSE0, 0);
	setbits32(pwrmbase + GBLRST_CAUSE1, 0);

	/*
	 * Disable ACPI PM timer based on Kconfig
	 *
	 * Disabling ACPI PM timer is necessary for XTAL OSC shutdown.
	 * Disabling ACPI PM timer also switches off TCO.
	 */
	if (!CONFIG(USE_PM_ACPI_TIMER))
		setbits8(pmc_mmio_regs() + PCH_PWRM_ACPI_TMR_CTL, ACPI_TIM_DIS);
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

/*
 * Check if WAKE# pin is enabled based on DSX_EN_WAKE_PIN setting in
 * deep_sx_config. If WAKE# pin is not enabled, then PCI Express Wake Disable
 * bit needs to be set in PM1_EN to avoid unnecessary wakes caused by WAKE#
 * pin.
 */
static void pm1_handle_wake_pin(void *unused)
{
	const config_t *conf = config_of_soc();

	/* If WAKE# pin is enabled, bail out early. */
	if (conf->deep_sx_config & DSX_EN_WAKE_PIN)
		return;

	pmc_update_pm1_enable(PCIEXPWAK_DIS);
}

BOOT_STATE_INIT_ENTRY(BS_PAYLOAD_LOAD, BS_ON_EXIT, pm1_handle_wake_pin, NULL);
BOOT_STATE_INIT_ENTRY(BS_OS_RESUME, BS_ON_ENTRY, pm1_handle_wake_pin, NULL);
