/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ops.h>
#include <device/cardbus.h>
#include <types.h>

/*
 * I don't think this code is quite correct but it is close.
 * Anyone with a cardbus bridge and a little time should be able
 * to make it usable quickly. -- Eric Biederman 24 March 2005
 */

/*
 * IO should be max 256 bytes. However, since we may have a P2P bridge below
 * a cardbus bridge, we need 4K.
 */
#define CARDBUS_IO_SIZE		4096
#define CARDBUS_MEM_SIZE	(32 * 1024 * 1024)

static void cardbus_record_bridge_resource(struct device *dev, resource_t moving,
		resource_t min_size, unsigned int index, unsigned long type)
{
	struct resource *resource;
	unsigned long gran;
	resource_t step;

	/* Initialize the constraints on the current bus. */
	resource = NULL;
	if (!moving)
		return;

	resource = new_resource(dev, index);
	resource->size = 0;
	gran = 0;
	step = 1;
	while ((moving & step) == 0) {
		gran += 1;
		step <<= 1;
	}
	resource->gran = gran;
	resource->align = gran;
	resource->limit = moving | (step - 1);
	resource->flags = type;

	/* Don't let the minimum size exceed what we can put in the resource. */
	if ((min_size - 1) > resource->limit)
		min_size = resource->limit + 1;

	resource->size = min_size;
}

void cardbus_read_resources(struct device *dev)
{
	resource_t moving_base, moving_limit, moving;
	unsigned long type;
	u16 ctl;

	/* See if needs a card control registers base address. */

	pci_get_resource(dev, PCI_BASE_ADDRESS_0);

	compact_resources(dev);

	/* See which bridge I/O resources are implemented. */
	moving_base = pci_moving_config32(dev, PCI_CB_IO_BASE_0);
	moving_limit = pci_moving_config32(dev, PCI_CB_IO_LIMIT_0);
	moving = moving_base & moving_limit;

	/* Initialize the I/O space constraints on the current bus. */
	cardbus_record_bridge_resource(dev, moving, CARDBUS_IO_SIZE,
				       PCI_CB_IO_BASE_0, IORESOURCE_IO);

	/* See which bridge I/O resources are implemented. */
	moving_base = pci_moving_config32(dev, PCI_CB_IO_BASE_1);
	moving_limit = pci_moving_config32(dev, PCI_CB_IO_LIMIT_1);
	moving = moving_base & moving_limit;

	/* Initialize the I/O space constraints on the current bus. */
	cardbus_record_bridge_resource(dev, moving, CARDBUS_IO_SIZE,
				       PCI_CB_IO_BASE_1, IORESOURCE_IO);

	/* If I can, enable prefetch for mem0. */
	ctl = pci_read_config16(dev, PCI_CB_BRIDGE_CONTROL);
	ctl &= ~PCI_CB_BRIDGE_CTL_PREFETCH_MEM0;
	ctl &= ~PCI_CB_BRIDGE_CTL_PREFETCH_MEM1;
	ctl |= PCI_CB_BRIDGE_CTL_PREFETCH_MEM0;
	pci_write_config16(dev, PCI_CB_BRIDGE_CONTROL, ctl);
	ctl = pci_read_config16(dev, PCI_CB_BRIDGE_CONTROL);

	/* See which bridge memory resources are implemented. */
	moving_base = pci_moving_config32(dev, PCI_CB_MEMORY_BASE_0);
	moving_limit = pci_moving_config32(dev, PCI_CB_MEMORY_LIMIT_0);
	moving = moving_base & moving_limit;

	/* Initialize the memory space constraints on the current bus. */
	type = IORESOURCE_MEM;
	if (ctl & PCI_CB_BRIDGE_CTL_PREFETCH_MEM0)
		type |= IORESOURCE_PREFETCH;
	cardbus_record_bridge_resource(dev, moving, CARDBUS_MEM_SIZE,
				       PCI_CB_MEMORY_BASE_0, type);

	/* See which bridge memory resources are implemented. */
	moving_base = pci_moving_config32(dev, PCI_CB_MEMORY_BASE_1);
	moving_limit = pci_moving_config32(dev, PCI_CB_MEMORY_LIMIT_1);
	moving = moving_base & moving_limit;

	/* Initialize the memory space constraints on the current bus. */
	cardbus_record_bridge_resource(dev, moving, CARDBUS_MEM_SIZE,
				       PCI_CB_MEMORY_BASE_1, IORESOURCE_MEM);

	compact_resources(dev);
}

void cardbus_enable_resources(struct device *dev)
{
	u16 ctrl;

	ctrl = pci_read_config16(dev, PCI_CB_BRIDGE_CONTROL);
	ctrl |= (dev->link_list->bridge_ctrl & (
			PCI_BRIDGE_CTL_VGA |
			PCI_BRIDGE_CTL_MASTER_ABORT |
			PCI_BRIDGE_CTL_BUS_RESET));
	/* Error check */
	ctrl |= (PCI_CB_BRIDGE_CTL_PARITY | PCI_CB_BRIDGE_CTL_SERR);
	printk(BIOS_DEBUG, "%s bridge ctrl <- %04x\n", dev_path(dev), ctrl);
	pci_write_config16(dev, PCI_CB_BRIDGE_CONTROL, ctrl);

	pci_dev_enable_resources(dev);
}

struct device_operations default_cardbus_ops_bus = {
	.read_resources   = cardbus_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = cardbus_enable_resources,
	.scan_bus         = pci_scan_bridge,
	.reset_bus        = pci_bus_reset,
};
