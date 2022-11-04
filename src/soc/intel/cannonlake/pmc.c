/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootstate.h>
#include <console/console.h>
#include <device/mmio.h>
#include <device/device.h>
#include <intelblocks/acpi.h>
#include <intelblocks/pmc.h>
#include <intelblocks/pmclib.h>
#include <intelblocks/rtc.h>
#include <soc/pci_devs.h>
#include <soc/pm.h>

#include "chip.h"

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

static void soc_pmc_read_resources(struct device *dev)
{
	struct resource *res;

	/* Add the fixed MMIO resource */
	mmio_resource_kb(dev, PWRMBASE, PCH_PWRM_BASE_ADDRESS / KiB,
			PCH_PWRM_BASE_SIZE / KiB);

	/* Add the fixed I/O resource */
	res = new_resource(dev, 1);
	res->base = (resource_t)ACPI_BASE_ADDRESS;
	res->size = (resource_t)ACPI_BASE_SIZE;
	res->limit = res->base + res->size - 1;
	res->flags = IORESOURCE_IO | IORESOURCE_ASSIGNED | IORESOURCE_FIXED;
}

static void soc_pmc_enable(struct device *dev)
{
	const config_t *config = config_of_soc();

	rtc_init();

	pmc_set_power_failure_state(true);
	pmc_gpe_init();

	config_deep_s3(config->deep_s3_enable_ac, config->deep_s3_enable_dc);
	config_deep_s5(config->deep_s5_enable_ac, config->deep_s5_enable_dc);
	config_deep_sx(config->deep_sx_config);
}

static void soc_pmc_init(struct device *dev)
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
	 * Because the device is set as `hidden` in the devicetree, enumeration
	 * is skipped, but the device callbacks are still called as if it were
	 * found.
	 */
	pmc_set_acpi_mode();

	/*
	 * Disable ACPI PM timer based on Kconfig
	 *
	 * Disabling ACPI PM timer is necessary for XTAL OSC shutdown.
	 * Disabling ACPI PM timer also switches off TCO.
	 */
	if (!CONFIG(USE_PM_ACPI_TIMER))
		setbits8(pmc_mmio_regs() + PCH_PWRM_ACPI_TMR_CTL, ACPI_TIM_DIS);
}

static void pmc_fill_ssdt(const struct device *dev)
{
	if (CONFIG(SOC_INTEL_COMMON_BLOCK_ACPI_PEP))
		generate_acpi_power_engine();
}

/*
 * `pmc_final` function is native implementation of equivalent events performed by
 * each FSP NotifyPhase() API invocations.
 *
 *
 * Clear PMCON status bits (Global Reset/Power Failure/Host Reset Status bits)
 *
 * Perform the PMCON status bit clear operation from `.final`
 * to cover any such chances where later boot stage requested a global
 * reset and PMCON status bit remains set.
 */
static void pmc_final(struct device *dev)
{
	pmc_clear_pmcon_sts();
}

struct device_operations pmc_ops = {
	.read_resources	  = soc_pmc_read_resources,
	.set_resources	  = noop_set_resources,
	.init		  = soc_pmc_init,
	.enable		  = soc_pmc_enable,
#if CONFIG(HAVE_ACPI_TABLES)
	.acpi_fill_ssdt	  = pmc_fill_ssdt,
#endif
	.scan_bus	  = scan_static_bus,
	.final		  = pmc_final,
};
