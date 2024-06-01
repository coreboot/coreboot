/* SPDX-License-Identifier: GPL-2.0-only */

#include <cbmem.h>
#include <commonlib/helpers.h>
#include <console/console.h>
#include <device/pci_ops.h>
#include <stdint.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <acpi/acpi.h>
#include <cpu/intel/smm_reloc.h>
#include <cpu/intel/speedstep.h>
#include <cpu/x86/smm.h>
#include "i945.h"

static void mch_domain_read_resources(struct device *dev)
{
	uint32_t pci_tolm;
	uintptr_t tolud;
	struct device *const d0f0 = pcidev_on_root(0, 0);
	int idx = 3;

	pci_domain_read_resources(dev);

	/* Can we find out how much memory we can use at most
	 * this way?
	 */
	pci_tolm = find_pci_tolm(dev->downstream);
	printk(BIOS_DEBUG, "pci_tolm: 0x%x\n", pci_tolm);

	tolud = pci_read_config8(d0f0, TOLUD) << 24;
	printk(BIOS_SPEW, "Top of Low Used DRAM: 0x%08lx\n", tolud);

	/* Report the memory regions */
	ram_range(dev, idx++, 0, 0xa0000);
	ram_from_to(dev, idx++, 1 * MiB, cbmem_top());

	/* TSEG */
	uintptr_t tseg_base;
	size_t tseg_size;
	smm_region(&tseg_base, &tseg_size);
	mmio_range(dev, idx++, tseg_base, tseg_size);

	/* cbmem_top can be shifted downwards due to alignment.
	   Mark the region between cbmem_top and tseg_base as unusable */
	if (cbmem_top() < tseg_base) {
		printk(BIOS_DEBUG, "Unused RAM between cbmem_top and TOM: 0x%lx\n",
		       tseg_base - cbmem_top());
		mmio_from_to(dev, idx++, cbmem_top(), tseg_base);
	}
	if (tseg_base + tseg_size < tolud)
		mmio_from_to(dev, idx++, tseg_base + tseg_size, tolud);

	/* legacy VGA memory */
	mmio_from_to(dev, idx++, 0xa0000, 0xc0000);
	/* RAM to be used for option roms and BIOS */
	reserved_ram_from_to(dev, idx++, 0xc0000, 1 * MiB);
}

static void mch_domain_set_resources(struct device *dev)
{
	struct resource *res;

	for (res = dev->resource_list; res; res = res->next)
		report_resource_stored(dev, res, "");

	assign_resources(dev->downstream);
}

static const char *northbridge_acpi_name(const struct device *dev)
{
	if (dev->path.type == DEVICE_PATH_DOMAIN)
		return "PCI0";

	if (!is_pci_dev_on_bus(dev, 0))
		return NULL;

	switch (dev->path.pci.devfn) {
	case PCI_DEVFN(0, 0):
		return "MCHC";
	}

	return NULL;
}

void northbridge_write_smram(u8 smram)
{
	struct device *dev = pcidev_on_root(0, 0);

	if (!dev)
		die("could not find pci 00:00.0!\n");

	pci_write_config8(dev, SMRAM, smram);
}

struct device_operations i945_pci_domain_ops = {
	.read_resources   = mch_domain_read_resources,
	.set_resources    = mch_domain_set_resources,
	.scan_bus         = pci_host_bridge_scan_bus,
	.acpi_name        = northbridge_acpi_name,
};

static void mc_read_resources(struct device *dev)
{
	pci_dev_read_resources(dev);

	mmconf_resource(dev, PCIEXBAR);
}

static struct device_operations mc_ops = {
	.read_resources   = mc_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.acpi_fill_ssdt   = generate_cpu_entries,
	.ops_pci          = &pci_dev_ops_pci,
};

static const unsigned short pci_device_ids[] = {
	0x2770, /* desktop */
	0x27a0, 0x27ac, /* mobile */
	0 };

static const struct pci_driver mc_driver __pci_driver = {
	.ops    = &mc_ops,
	.vendor = PCI_VID_INTEL,
	.devices = pci_device_ids,
};

struct device_operations i945_cpu_bus_ops = {
	.read_resources   = noop_read_resources,
	.set_resources    = noop_set_resources,
	.init             = mp_cpu_bus_init,
};

struct chip_operations northbridge_intel_i945_ops = {
	.name = "Intel i945 Northbridge",
};

bool northbridge_support_slfm(void)
{
	return false;
}
