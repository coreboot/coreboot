/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <cbfs.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci_def.h>
#include <device/pci_ops.h>
#include <device/resource.h>
#include <fsp/api.h>
#include <intelblocks/acpi.h>
#include <intelblocks/irq.h>
#include <ramstage.h>
#include <soc/acpi.h>
#include <soc/pci_devs.h>
#include <stdbool.h>
#include <stdint.h>

#include "chip.h"
#include "common/fsp_hob.h"

static void soc_silicon_init_params(FSPS_UPD *supd)
{
	const struct microcode *microcode_file;
	size_t microcode_len;

	microcode_file = cbfs_map("cpu_microcode_blob.bin", &microcode_len);
	if (microcode_file && (microcode_len != 0)) {
		supd->FspsConfig.PcdCpuMicrocodePatchBase = (UINT32)microcode_file;
		supd->FspsConfig.PcdCpuMicrocodePatchSize = (UINT32)microcode_len;
	}
}

/* UPD parameters to be initialized before SiliconInit */
void platform_fsp_silicon_init_params_cb(FSPS_UPD *supd)
{
	soc_silicon_init_params(supd);
	mainboard_silicon_init_params(supd);
}

static void chip_domain_resource_table_init(config_t *chip_info)
{
	memset(&chip_info->domain, 0, sizeof(chip_info->domain));
	/**
	 * Valid resource range meets base <= limit.
	 */
	for (int i = 0; i < MAX_DOMAIN; i++) {
		chip_info->domain[i].bus_base = UINT8_MAX;
		chip_info->domain[i].io_base = UINT16_MAX;
		chip_info->domain[i].mem32_base = UINT32_MAX;
		chip_info->domain[i].mem64_base = UINT64_MAX;
	}
}

static void chip_domain_resource_table_load(config_t *chip_info)
{
	const BL_IIO_UDS *hob = fsp_hob_get_iio_uds_data();
	if (!hob)
		die("FSP HOB IIO UDS DATA not found!\n");

	/**
	 * Assert for `numofIIO` and `BL_MAX_LOGIC_IIO_STACK` so the following loop effects.
	 */
	assert(hob->PlatformData.numofIIO == 1);
	assert(BL_MAX_LOGIC_IIO_STACK);

	const BL_STACK_RES *stack_res = hob->PlatformData.IIO_resource[0].StackRes;
	for (int i = 0; i < BL_MAX_LOGIC_IIO_STACK; i++) {
		struct snr_domain *domain = &chip_info->domain[i];
		domain->personality = stack_res[i].Personality;

		/**
		 * SNR FSP uses specific enumerations for different type of stacks, see
		 * vendorcode/intel/fsp/fsp2_0/snowridge/FspmUpd.h.
		 */
		if (stack_res[i].Personality >= BL_TYPE_DISABLED)
			continue;

		if (stack_res[i].BusBase > stack_res[i].BusLimit)
			die("Incorrect bus base 0x%02x and limit 0x%02x for domain %d",
			    stack_res[i].BusBase, stack_res[i].BusLimit, i);

		domain->bus_base = stack_res[i].BusBase;
		domain->bus_limit = stack_res[i].BusLimit;

		/**
		 * Stack with `BL_TYPE_RESERVED` personality has valid bus range but no IO and MMIO resources.
		 */
		if (stack_res[i].Personality == BL_TYPE_RESERVED)
			continue;

		domain->enabled = true;

		/**
		 * Only non-zero resource base is valid.
		 */
		if (stack_res[i].PciResourceIoBase) {
			if (stack_res[i].PciResourceIoBase > stack_res[i].PciResourceIoLimit)
				die("Incorrect IO base 0x%04x and limit 0x%04x for domain %d",
				    stack_res[i].PciResourceIoBase,
				    stack_res[i].PciResourceIoLimit, i);

			domain->io_base = stack_res[i].PciResourceIoBase;
			domain->io_limit = stack_res[i].PciResourceIoLimit;
		}

		if (stack_res[i].PciResourceMem32Base) {
			if (stack_res[i].PciResourceMem32Base >
			    stack_res[i].PciResourceMem32Limit)
				die("Incorrect Mem32 base 0x%08x and limit 0x%08x for domain %d",
				    stack_res[i].PciResourceMem32Base,
				    stack_res[i].PciResourceMem32Limit, i);

			domain->mem32_base = stack_res[i].PciResourceMem32Base;
			domain->mem32_limit = stack_res[i].PciResourceMem32Limit;
		}

		if (stack_res[i].PciResourceMem64Base) {
			if (stack_res[i].PciResourceMem64Base >
			    stack_res[i].PciResourceMem64Limit)
				die("Incorrect Mem64 base 0x%16llx and limit 0x%16llx for domain %d",
				    stack_res[i].PciResourceMem64Base,
				    stack_res[i].PciResourceMem64Limit, i);

			domain->mem64_base = stack_res[i].PciResourceMem64Base;
			domain->mem64_limit = stack_res[i].PciResourceMem64Limit;
		}
	}
}

static void chip_cfg_domain_resource_table_split(config_t *chip_info, int base_domain_index,
						 int addtional_domain_index)
{
	assert(base_domain_index < MAX_DOMAIN);
	assert(addtional_domain_index < MAX_DOMAIN);

	struct snr_domain *base_domain = &chip_info->domain[base_domain_index];
	struct snr_domain *addtional_domain = &chip_info->domain[addtional_domain_index];
	memcpy(addtional_domain, base_domain, sizeof(*addtional_domain));

	/**
	 * Base + (Limit - Base) / 2 to avoid overflow.
	 */
	if (base_domain->bus_base < base_domain->bus_limit) {
		base_domain->bus_limit = base_domain->bus_base +
					 (base_domain->bus_limit - base_domain->bus_base) / 2;
		addtional_domain->bus_base = base_domain->bus_limit + 1;
	}

	if (base_domain->io_base < base_domain->io_limit) {
		base_domain->io_limit = base_domain->io_base +
					(base_domain->io_limit - base_domain->io_base) / 2;
		addtional_domain->io_base = base_domain->io_limit + 1;
	}

	if (base_domain->mem32_base < base_domain->mem32_limit) {
		base_domain->mem32_limit =
			base_domain->mem32_base +
			(base_domain->mem32_limit - base_domain->mem32_base) / 2;
		addtional_domain->mem32_base = base_domain->mem32_limit + 1;
	}

	if (base_domain->mem64_base < base_domain->mem64_limit) {
		base_domain->mem64_limit =
			base_domain->mem64_base +
			(base_domain->mem64_limit - base_domain->mem64_base) / 2;
		addtional_domain->mem64_base = base_domain->mem64_limit + 1;
	}
}

static void chip_domain_resource_table_handle_multi_root(config_t *chip_info)
{
	/**
	 * Split domains that have more than 1 root bus.
	 */
	int addtional_domain = BL_MAX_LOGIC_IIO_STACK;
	chip_cfg_domain_resource_table_split(chip_info, 7,
					     addtional_domain++); /**< Domain 8 (Ubox1). */

	if (chip_info->domain[2].enabled)
		chip_cfg_domain_resource_table_split(
			chip_info, 2,
			addtional_domain++); /**< Domain 2 (S2) has an extra root bus for Intel DLB. */
}

static void chip_domain_resource_table_dump(config_t *chip_info)
{
	printk(BIOS_DEBUG,
	       "---------------------------------------------------------------------------------------------------------------------\n"
	       "| Domain | Enabled | Bus Base/Limit | IO Base/Limit |    Mem32 Base/Limit   |            Mem64 Base/Limit           |\n"
	       "---------------------------------------------------------------------------------------------------------------------\n");
	for (int i = 0; i < MAX_DOMAIN; i++) {
		struct snr_domain *domain = &chip_info->domain[i];
		printk(BIOS_DEBUG,
		       "|   %2u   | %c(0x%02x) |    0x%02x/0x%02x   | 0x%04x/0x%04x | 0x%08x/0x%08x | 0x%016llx/0x%016llx |\n",
		       i, domain->enabled ? 'Y' : 'N', domain->personality, domain->bus_base,
		       domain->bus_limit, domain->io_base, domain->io_limit, domain->mem32_base,
		       domain->mem32_limit, domain->mem64_base, domain->mem64_limit);
	}
	printk(BIOS_DEBUG,
	       "---------------------------------------------------------------------------------------------------------------------\n");
}

static void chip_domain_resource_table_fill(config_t *chip_info)
{
	chip_domain_resource_table_init(chip_info);
	chip_domain_resource_table_load(chip_info);
	chip_domain_resource_table_handle_multi_root(chip_info);
	chip_domain_resource_table_dump(chip_info);
}

static void devicetree_domain_definition_update(config_t *chip_info)
{
	struct device *dev = NULL;
	int domain;

	/**
	 * Check static domain id agaginst dynamic and fill domain device.
	 */
	while ((dev = dev_find_path(dev, DEVICE_PATH_DOMAIN)) != NULL) {
		domain = dev_get_domain_id(dev);
		if (domain >= MAX_DOMAIN)
			die("Incorrect domain[%d] in devicetree\n", domain);

		chip_info->domain[domain].dev = dev;

		printk(BIOS_SPEW, "%s -> domain[%d] defined in devicetree\n", dev_path(dev),
		       domain);
	}

	if (!chip_info->domain[0].dev)
		die("Please add domain 0 to device tree!\n");

	for (domain = 0; domain < MAX_DOMAIN; domain++) {
		if (!chip_info->domain[domain].enabled) {
			if (chip_info->domain[domain].dev &&
			    chip_info->domain[domain].dev->enabled) {
				printk(BIOS_WARNING,
				       "Domain[%d] is disabled, reserved or not existed but defined in device tree!\n",
				       domain);
				dev_set_enabled(chip_info->domain[domain].dev, false);
			}
			continue;
		}

		if (!chip_info->domain[domain].dev)
			die("Domain[%d] is enabled but not defined in device tree!\n", domain);

		dev = chip_info->domain[domain].dev;

		printk(BIOS_SPEW, "Domain[%d] is enabled, updating PCI root bus to 0x%02x\n",
		       domain, chip_info->domain[domain].bus_base);

		/* Use same bus number for secondary and subordinate before PCI enumeration. */
		dev->downstream->secondary = dev->downstream->subordinate =
			chip_info->domain[domain].bus_base;
		dev->downstream->max_subordinate = chip_info->domain[domain].bus_limit;
	}
}

static void soc_init_pre_device(void *chip_info)
{
	fsp_silicon_init();

	chip_domain_resource_table_fill((config_t *)chip_info);

	devicetree_domain_definition_update((config_t *)chip_info);
}

static void domain_read_resources(struct device *dev)
{
	struct resource *res;
	uint8_t index = 0;
	const uint8_t domain = dev->path.domain.domain_id;
	const config_t *chip_info = dev->chip_info;

	printk(BIOS_DEBUG, "Reading resources for %s\n", dev_path(dev));

	/**
	 * Allocate IO resource.
	 */
	if (chip_info->domain[domain].io_limit >= chip_info->domain[domain].io_base) {
		res = new_resource(dev, index++);
		if (!res)
			die("Allocate resource for %s failed!\n", dev_path(dev));

		res->base = chip_info->domain[domain].io_base;
		res->limit = chip_info->domain[domain].io_limit;
		res->size = res->limit - res->base + 1;
		res->flags = IORESOURCE_IO | IORESOURCE_ASSIGNED;
		printk(BIOS_SPEW, "IO Base/Limit: 0x%04llx/0x%04llx\n", res->base, res->limit);
	}

	/**
	 * Allocate Mem32 resource.
	 */
	if (chip_info->domain[domain].mem32_limit >= chip_info->domain[domain].mem32_base) {
		res = new_resource(dev, index++);
		if (!res)
			die("Allocate resource for %s failed!\n", dev_path(dev));

		res->base = chip_info->domain[domain].mem32_base;
		res->limit = chip_info->domain[domain].mem32_limit;
		res->size = res->limit - res->base + 1;
		res->flags = IORESOURCE_MEM | IORESOURCE_ASSIGNED;
		printk(BIOS_SPEW, "Mem32 Base/Limit: 0x%08llx/0x%08llx\n", res->base,
		       res->limit);
	}

	/**
	 * Allocate Mem64 resource.
	 */
	if (chip_info->domain[domain].mem64_limit >= chip_info->domain[domain].mem64_base) {
		res = new_resource(dev, index++);
		if (!res)
			die("Allocate resource for %s failed!\n", dev_path(dev));

		res->base = chip_info->domain[domain].mem64_base;
		res->limit = chip_info->domain[domain].mem64_limit;
		res->size = res->limit - res->base + 1;
		res->flags = IORESOURCE_MEM | IORESOURCE_ASSIGNED | IORESOURCE_PCI64 |
			     IORESOURCE_ABOVE_4G;
		printk(BIOS_SPEW, "Mem64 Base/Limit: 0x%016llx/0x%016llx\n", res->base,
		       res->size);
	}

	printk(BIOS_DEBUG, "Reading resources for %s done\n", dev_path(dev));
}

static void domain_set_resources(struct device *dev)
{
	printk(BIOS_DEBUG, "Setting resources for %s\n", dev_path(dev));

	pci_domain_set_resources(dev);

	printk(BIOS_DEBUG, "Setting resources for %s done\n", dev_path(dev));
}

static void domain_scan_bus(struct device *dev)
{
	printk(BIOS_DEBUG, "Scanning %s\n", dev_path(dev));

	pci_host_bridge_scan_bus(dev);

	printk(BIOS_DEBUG, "Scanning %s done\n", dev_path(dev));
}

const char *soc_acpi_name(const struct device *dev)
{
	if (dev->path.type == DEVICE_PATH_DOMAIN)
		switch (dev_get_domain_id(dev)) {
		case 0:
			return "PCI0";
		case 1:
			return "PCI1";
		case 2:
			return "PCI2";
		case 3:
			return "PCI3";
		case 4:
			return "PCI4";
		case 5:
			return "PCI5";
		case 6:
			return "PCI6";
		case 7:
			return "PCI7";
		case 8:
			return "PCI8";
		case 9:
			return "PCI9";
		default:
			return NULL;
		}

	return NULL;
}

static struct device_operations pci_domain_ops = {
	.read_resources = domain_read_resources,
	.set_resources = domain_set_resources,
	.scan_bus = domain_scan_bus,
	.acpi_fill_ssdt = domain_fill_ssdt,
	.acpi_name = soc_acpi_name,
};

static struct device_operations cpu_bus_ops = {
	.read_resources = noop_read_resources,
	.set_resources = noop_set_resources,
	.acpi_fill_ssdt = generate_cpu_entries,
};

static void southcluster_enable_dev(struct device *dev)
{
	if (!dev->enabled) {
		printk(BIOS_DEBUG, "Disable %s\n", dev_path(dev));
		/**
		 * Mark the device as hidden so that coreboot won't complain about leftover static
		 * device.
		 */
		dev->hidden = true;
		pci_and_config16(dev, PCI_COMMAND,
				 ~(PCI_COMMAND_IO | PCI_COMMAND_MEMORY | PCI_COMMAND_MASTER));
	} else {
		printk(BIOS_DEBUG, "Enable %s\n", dev_path(dev));
		pci_or_config16(dev, PCI_COMMAND, PCI_COMMAND_SERR);
	}
}

static void soc_enable_dev(struct device *dev)
{
	if (dev->path.type == DEVICE_PATH_DOMAIN) {
		printk(BIOS_SPEW, "Set domain operation for %s\n", dev_path(dev));
		dev->ops = &pci_domain_ops;
	} else if (dev->path.type == DEVICE_PATH_CPU_CLUSTER) {
		printk(BIOS_SPEW, "Set CPU cluster operation for %s\n", dev_path(dev));
		dev->ops = &cpu_bus_ops;
	} else if (dev->path.type == DEVICE_PATH_PCI) {
		/**
		 * For a static PCH device, if it's enabled, set the SERR bits, otherwise
		 * disable the ability of issuing and responding to IO and memory requests.
		 */
		if (is_dev_on_domain0(dev) && is_pci(dev) && is_pch_slot(dev->path.pci.devfn))
			southcluster_enable_dev(dev);
	}
}

struct chip_operations soc_intel_snowridge_ops = {
	.name = "Intel Snowridge",
	.init = &soc_init_pre_device,
	.enable_dev = &soc_enable_dev,
};
