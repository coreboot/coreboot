/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootstate.h>
#include <console/console.h>
#include <device/device.h>
#include <device/mmio.h>
#include <intelblocks/pmc.h>
#include <intelblocks/pmclib.h>
#include <intelblocks/rtc.h>
#include <soc/pci_devs.h>
#include <soc/pm.h>
#include <soc/soc_chip.h>

/*
 * Set which power state system will be after reapplying
 * the power (from G3 State)
 */
void pmc_soc_set_afterg3_en(const bool on)
{
	uint8_t reg8;
	uint8_t *const pmcbase = pmc_mmio_regs();

	reg8 = read8(pmcbase + GEN_PMCON_A);
	if (on)
		reg8 &= ~SLEEP_AFTER_POWER_FAIL;
	else
		reg8 |= SLEEP_AFTER_POWER_FAIL;
	write8(pmcbase + GEN_PMCON_A, reg8);
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

static void pmc_init(struct device *dev)
{
	const config_t *config = config_of_soc();

	rtc_init();

	pmc_set_power_failure_state(true);
	pmc_gpe_init();

	config_deep_s3(config->deep_s3_enable_ac, config->deep_s3_enable_dc);
	config_deep_s5(config->deep_s5_enable_ac, config->deep_s5_enable_dc);
	config_deep_sx(config->deep_sx_config);
}

static void soc_pmc_read_resources(struct device *dev)
{
	struct resource *res;

	mmio_resource(dev, 0, PCH_PWRM_BASE_ADDRESS / KiB, PCH_PWRM_BASE_SIZE / KiB);

	res = new_resource(dev, 1);
	res->base = (resource_t)ACPI_BASE_ADDRESS;
	res->size = (resource_t)ACPI_BASE_SIZE;
	res->limit = res->base + res->size + 1;
	res->flags = IORESOURCE_IO | IORESOURCE_ASSIGNED | IORESOURCE_FIXED;
}

static void soc_acpi_mode_init(struct device *dev)
{
	/*
	 * pmc_set_acpi_mode() should be delayed until BS_DEV_INIT in order
	 * to ensure the ordering does not break the assumptions that other
	 * drivers make about ACPI mode (e.g. Chrome EC). Since it disables
	 * ACPI mode, other drivers may take different actions based on this
	 * (e.g. Chrome EC will flush any pending hostevent bits). Because
	 * EHL has its PMC device available for device_operations, it can be
	 * done from the "ops->init" callback.
	 */
	pmc_set_acpi_mode();
}

struct device_operations pmc_ops = {
	.read_resources		= soc_pmc_read_resources,
	.set_resources		= noop_set_resources,
	.init			= soc_acpi_mode_init,
	.enable			= pmc_init,
};
