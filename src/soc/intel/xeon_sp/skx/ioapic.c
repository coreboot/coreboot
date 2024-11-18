/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <arch/ioapic.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <soc/acpi.h>
#include <soc/pci_devs.h>
#include <soc/ramstage.h>
#include <soc/chip_common.h>

static void ioapic_read_resources(struct device *dev)
{
	pci_dev_read_resources(dev);

	u16 abar = pci_read_config16(dev, APIC_ABAR);
	if (!abar)
		return;
	const u32 addr = IO_APIC_ADDR | ((abar & 0xfff) << 8);

	printk(BIOS_DEBUG, " %s: IOAPIC Address: 0x%x\n",
	       dev_path(dev), addr);

	mmio_range(dev, APIC_ABAR, addr, 0x100);
}

static void ioapic_init(struct device *dev)
{
	struct resource *resource;

	pci_dev_init(dev);

	/* See if there is a resource with the appropriate index. */
	resource = probe_resource(dev, APIC_ABAR);
	if (!resource)
		return;

	const union xeon_domain_path dn = {
		.domain_path = dev_get_domain_id(dev)
	};

	/* 14nm Xeon-SP has up to 6 stacks per socket */
	assert(dn.stack < MAX_LOGIC_IIO_STACK);

	/* Assign socket specific GSI_BASE */
	const u32 gsi_base = 24 + (dn.socket * MAX_LOGIC_IIO_STACK + dn.stack) * 8;

	ioapic_create_dev(dev, resource->base, gsi_base);
}

static const unsigned short ioapic_ids[] = {
	0x2036,
	0x2026,
	0
};

static struct device_operations ioapic_ops = {
	.read_resources   = ioapic_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init             = ioapic_init,
};

static const struct pci_driver ioapic_driver __pci_driver = {
	.ops = &ioapic_ops,
	.vendor = PCI_VID_INTEL,
	.devices = ioapic_ids,
};
