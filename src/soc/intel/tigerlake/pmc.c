/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * This file is created based on Intel Tiger Lake Processor PCH Datasheet
 * Document number: 575857
 * Chapter number: 4
 */

#include <acpi/acpigen.h>
#include <console/console.h>
#include <device/mmio.h>
#include <device/device.h>
#include <drivers/intel/pmc_mux/chip.h>
#include <intelblocks/pmc.h>
#include <intelblocks/pmclib.h>
#include <intelblocks/rtc.h>
#include <soc/pci_devs.h>
#include <soc/pm.h>
#include <soc/soc_chip.h>

#define PMC_HID		"INTC1026"

enum pch_pmc_xtal pmc_get_xtal_freq(void)
{
	uint8_t *const pmcbase = pmc_mmio_regs();

	return PCH_EPOC_XTAL_FREQ(read32(pmcbase + PCH_PMC_EPOC));
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

	/* Add the fixed MMIO resource */
	mmio_resource(dev, 0, PCH_PWRM_BASE_ADDRESS / KiB, PCH_PWRM_BASE_SIZE / KiB);

	/* Add the fixed I/O resource */
	res = new_resource(dev, 1);
	res->base = (resource_t)ACPI_BASE_ADDRESS;
	res->size = (resource_t)ACPI_BASE_SIZE;
	res->limit = res->base + res->size - 1;
	res->flags = IORESOURCE_IO | IORESOURCE_ASSIGNED | IORESOURCE_FIXED;
}

static void soc_pmc_fill_ssdt(const struct device *dev)
{
	const char *scope = acpi_device_scope(dev);
	const char *name = acpi_device_name(dev);
	if (!scope || !name)
		return;

	acpigen_write_scope(scope);
	acpigen_write_device(name);

	acpigen_write_name_string("_HID", PMC_HID);
	acpigen_write_name_string("_DDN", "Intel(R) Tiger Lake IPC Controller");

	/*
	 * Part of the PCH's reserved 32 MB MMIO range (0xFC800000 - 0xFE7FFFFF).
	 * The PMC gets 0xFE000000 - 0xFE00FFFF.
	 */
	acpigen_write_name("_CRS");
	acpigen_write_resourcetemplate_header();
	acpigen_write_mem32fixed(1, PCH_PWRM_BASE_ADDRESS, PCH_PWRM_BASE_SIZE);
	acpigen_write_resourcetemplate_footer();

	acpigen_pop_len(); /* PMC Device */
	acpigen_pop_len(); /* Scope */

	printk(BIOS_INFO, "%s: %s at %s\n", acpi_device_path(dev), dev->chip_ops->name,
	       dev_path(dev));
}

static void soc_acpi_mode_init(struct device *dev)
{
	/*
	 * pmc_set_acpi_mode() should be delayed until BS_DEV_INIT in order
	 * to ensure the ordering does not break the assumptions that other
	 * drivers make about ACPI mode (e.g. Chrome EC). Since it disables
	 * ACPI mode, other drivers may take different actions based on this
	 * (e.g. Chrome EC will flush any pending hostevent bits). Because
	 * TGL has its PMC device available for device_operations, it can be
	 * done from the "ops->init" callback.
	 */
	pmc_set_acpi_mode();
}

struct device_operations pmc_ops = {
	.read_resources	  = soc_pmc_read_resources,
	.set_resources	  = noop_set_resources,
	.init		  = soc_acpi_mode_init,
	.enable		  = pmc_init,
#if CONFIG(HAVE_ACPI_TABLES)
	.acpi_fill_ssdt	  = soc_pmc_fill_ssdt,
#endif
	.scan_bus	  = scan_static_bus,
};
