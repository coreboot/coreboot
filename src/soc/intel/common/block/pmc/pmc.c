/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/pci_ops.h>
#include <console/console.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <intelblocks/acpi.h>
#include <intelblocks/pmc.h>
#include <intelblocks/pmclib.h>
#include <soc/pci_devs.h>
#include <soc/pm.h>

static void pch_pmc_add_new_resource(struct device *dev,
		uint8_t offset, uintptr_t base, size_t size,
		unsigned long flags)
{
	struct resource *res;
	res = new_resource(dev, offset);
	res->base = base;
	res->size = size;
	res->flags = flags;
}

static void pch_pmc_add_mmio_resources(struct device *dev,
		const struct pmc_resource_config *cfg)
{
	pch_pmc_add_new_resource(dev, cfg->pwrmbase_offset,
			cfg->pwrmbase_addr, cfg->pwrmbase_size,
			IORESOURCE_MEM | IORESOURCE_ASSIGNED |
			IORESOURCE_FIXED | IORESOURCE_RESERVE);
}

static void pch_pmc_add_io_resources(struct device *dev,
		const struct pmc_resource_config *cfg)
{
	pch_pmc_add_new_resource(dev, cfg->abase_offset,
			cfg->abase_addr, cfg->abase_size,
			 IORESOURCE_IO | IORESOURCE_ASSIGNED |
			 IORESOURCE_FIXED);
	if (CONFIG(PMC_INVALID_READ_AFTER_WRITE)) {
		/*
		 * The ACPI IO BAR (offset 0x20) is not PCI compliant. We've
		 * observed cases where the BAR reads back as 0, but the IO
		 * window is open. This also means that it will not respond
		 * to PCI probing.
		 */
		pci_write_config16(dev, cfg->abase_offset, cfg->abase_addr);
		/*
		 * In pci_dev_enable_resources, reading IO SPACE ACCESS bit in
		 * STATUSCOMMAND register does not read back the written
		 * value correctly, hence IO access gets disabled. This is
		 * seen in some PMC devices, hence this code makes sure
		 * IO access is available.
		 */
		dev->command |= PCI_COMMAND_IO;
	}
}

static void pch_pmc_read_resources(struct device *dev)
{
	struct pmc_resource_config pmc_cfg;
	struct pmc_resource_config *config = &pmc_cfg;

	if (pmc_soc_get_resources(config) < 0)
		die_with_post_code(POST_HW_INIT_FAILURE,
				   "Unable to get PMC controller resource information!");

	/* Get the normal PCI resources of this device. */
	pci_dev_read_resources(dev);

	/* Add non-standard MMIO resources. */
	pch_pmc_add_mmio_resources(dev, config);

	/* Add IO resources. */
	pch_pmc_add_io_resources(dev, config);
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

static struct device_operations device_ops = {
	.read_resources		= pch_pmc_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= pmc_soc_init,
	.ops_pci		= &pci_dev_ops_pci,
	.scan_bus		= scan_static_bus,
#if CONFIG(HAVE_ACPI_TABLES)
	.acpi_fill_ssdt		= pmc_fill_ssdt,
#endif
	.final			= pmc_final,
};

static const unsigned short pci_device_ids[] = {
	PCI_DID_INTEL_MTL_SOC_PMC,
	PCI_DID_INTEL_MTL_IOE_M_PMC,
	PCI_DID_INTEL_MTL_IOE_P_PMC,
	PCI_DID_INTEL_RPP_P_PMC,
	PCI_DID_INTEL_DNV_PMC,
	PCI_DID_INTEL_SPT_LP_PMC,
	PCI_DID_INTEL_SPT_H_PMC,
	PCI_DID_INTEL_LWB_PMC,
	PCI_DID_INTEL_LWB_PMC_SUPER,
	PCI_DID_INTEL_UPT_H_PMC,
	PCI_DID_INTEL_APL_PMC,
	PCI_DID_INTEL_GLK_PMC,
	PCI_DID_INTEL_CNP_H_PMC,
	PCI_DID_INTEL_ICP_PMC,
	PCI_DID_INTEL_CMP_PMC,
	PCI_DID_INTEL_CMP_H_PMC,
	PCI_DID_INTEL_TGP_PMC,
	PCI_DID_INTEL_TGP_H_PMC,
	PCI_DID_INTEL_MCC_PMC,
	PCI_DID_INTEL_JSP_PMC,
	PCI_DID_INTEL_ADP_P_PMC,
	PCI_DID_INTEL_ADP_S_PMC,
	PCI_DID_INTEL_ADP_M_N_PMC,
	0
};

static const struct pci_driver pch_pmc __pci_driver = {
	.ops	 = &device_ops,
	.vendor	 = PCI_VID_INTEL,
	.devices = pci_device_ids,
};
