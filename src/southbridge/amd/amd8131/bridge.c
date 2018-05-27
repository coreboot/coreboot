/*
 * This file is part of the coreboot project.
 *
 * (C) 2003-2004 Linux Networx
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

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <pc80/mc146818rtc.h>
#include <device/pci_def.h>
#include <device/pcix.h>

#define NMI_OFF 0

#define NPUML 0xD9	/* Non prefetchable upper memory limit */
#define NPUMB 0xD8	/* Non prefetchable upper memory base */

static void amd8131_walk_children(struct bus *bus,
	void (*visit)(struct device *dev, void *ptr), void *ptr)
{
	struct device *child;
	for (child = bus->children; child; child = child->sibling) {
		if (child->path.type != DEVICE_PATH_PCI) {
			continue;
		}
		if (child->hdr_type == PCI_HEADER_TYPE_BRIDGE) {
			amd8131_walk_children(child->link_list, visit, ptr);
		}
		visit(child, ptr);
	}
}

struct amd8131_bus_info {
	unsigned sstatus;
	unsigned rev;
	int errata_56;
	int master_devices;
	int max_func;
};

static void amd8131_count_dev(struct device *dev, void *ptr)
{
	struct amd8131_bus_info *info = ptr;
	/* Don't count pci bridges */
	if (dev->hdr_type != PCI_HEADER_TYPE_BRIDGE) {
		info->master_devices++;
	}
	if (PCI_FUNC(dev->path.pci.devfn) > info->max_func) {
		info->max_func = PCI_FUNC(dev->path.pci.devfn);
	}
}


static void amd8131_pcix_tune_dev(struct device *dev, void *ptr)
{
	struct amd8131_bus_info *info = ptr;
	unsigned cap;
	unsigned status, cmd, orig_cmd;
	unsigned max_read, max_tran;
	int sib_funcs, sibs;
	struct device *sib;

	if (dev->hdr_type != PCI_HEADER_TYPE_NORMAL) {
		return;
	}
	cap = pci_find_capability(dev, PCI_CAP_ID_PCIX);
	if (!cap) {
		return;
	}
	/* How many siblings does this device have? */
	sibs = info->master_devices - 1;
	/* Count how many sibling functions this device has */
	sib_funcs = 0;
	for (sib = dev->bus->children; sib; sib = sib->sibling) {
		if (sib == dev) {
			continue;
		}
		if (PCI_SLOT(sib->path.pci.devfn) != PCI_SLOT(dev->path.pci.devfn)) {
			continue;
		}
		sib_funcs++;
	}


	printk(BIOS_DEBUG, "%s AMD8131 PCI-X tuning\n", dev_path(dev));
	status = pci_read_config32(dev, cap + PCI_X_STATUS);
	orig_cmd = cmd = pci_read_config16(dev,cap + PCI_X_CMD);

	max_read = (status & PCI_X_STATUS_MAX_READ) >> 21;
	max_tran = (status & PCI_X_STATUS_MAX_SPLIT) >> 23;

	/* Errata #49 don't allow 4K transactions */
	if (max_read >= 2) {
		max_read = 2;
	}

	/* Errata #37 Limit the number of split transactions to avoid starvation */
	if (sibs >= 2) {
		/* At most 2 outstanding split transactions when we have
		 * 3 or more bus master devices on the bus.
		 */
		if (max_tran > 1) {
			max_tran = 1;
		}
	}
	else if (sibs == 1) {
		/* At most 4 outstanding split transactions when we have
		 * 2 bus master devices on the bus.
		 */
		if (max_tran > 3) {
			max_tran = 3;
		}
	}
	else {
		/* At most 8 outstanding split transactions when we have
		 * only one bus master device on the bus.
		 */
		if (max_tran > 4) {
			max_tran = 4;
		}
	}
	/* Errata #56 additional limits when the bus runs at 133Mhz */
	if (info->errata_56 &&
		(PCI_X_SSTATUS_MFREQ(info->sstatus) == PCI_X_SSTATUS_MODE1_133MHZ))
	{
		unsigned limit_read;
		/* Look at the number of siblings and compute the
		 * largest legal read size.
		 */
		if (sib_funcs == 0) {
			/* 2k reads */
			limit_read = 2;
		}
		else if (sib_funcs <= 1) {
			/* 1k reads */
			limit_read = 1;
		}
		else {
			/* 512 byte reads */
			limit_read = 0;
		}
		if (max_read > limit_read) {
			max_read = limit_read;
		}
		/* Look at the read size and the number of siblings
		 * and compute how many outstanding transactions I can have.
		 */
		if (max_read == 2) {
			/* 2K reads */
			if (max_tran > 0) {
				/* Only 1 outstanding transaction allowed */
				max_tran = 0;
			}
		}
		else if (max_read == 1) {
			/* 1K reads */
			if (max_tran > (1 - sib_funcs)) {
				/* At most 2 outstanding transactions */
				max_tran = 1 - sib_funcs;
			}
		}
		else {
			/* 512 byte reads */
			max_read = 0;
			if (max_tran > (2 - sib_funcs)) {
				/* At most 3 outstanding transactions */
				max_tran = 2 - sib_funcs;
			}
		}
	}
#if 0
	printk(BIOS_DEBUG, "%s max_read: %d max_tran: %d sibs: %d sib_funcs: %d\n",
		dev_path(dev), max_read, max_tran, sibs, sib_funcs, sib_funcs);
#endif
	if (max_read != ((cmd & PCI_X_CMD_MAX_READ) >> 2)) {
		cmd &= ~PCI_X_CMD_MAX_READ;
		cmd |= max_read << 2;
		}
	if (max_tran != ((cmd & PCI_X_CMD_MAX_SPLIT) >> 4)) {
		cmd &= ~PCI_X_CMD_MAX_SPLIT;
		cmd |= max_tran << 4;
	}

	/* Don't attempt to handle PCI-X errors */
	cmd &= ~PCI_X_CMD_DPERR_E;
	/* The 8131 does not work properly with relax ordering enabled.
	 * Errata #58
	 */
	cmd &= ~PCI_X_CMD_ERO;
	if (orig_cmd != cmd) {
		pci_write_config16(dev, cap + PCI_X_CMD, cmd);
	}
}
static void amd8131_scan_bus(struct bus *bus,
	unsigned min_devfn, unsigned max_devfn)
{
	struct amd8131_bus_info info;
	struct bus *pbus;
	unsigned pos;

	/* Find the children on the bus */
	pci_scan_bus(bus, min_devfn, max_devfn);

	/* Find the revision of the 8131 */
	info.rev = pci_read_config8(bus->dev, PCI_CLASS_REVISION);

	/* See which errata apply */
	info.errata_56 = info.rev <= 0x12;

	/* Find the pcix capability and get the secondary bus status */
	pos = pci_find_capability(bus->dev, PCI_CAP_ID_PCIX);
	info.sstatus = pci_read_config16(bus->dev, pos + PCI_X_SEC_STATUS);

	/* Print the PCI-X bus speed */
	printk(BIOS_DEBUG, "PCI: %02x: %s\n", bus->secondary, pcix_speed(info.sstatus));


	/* Examine the bus and find out how loaded it is */
	info.max_func = 0;
	info.master_devices  = 0;
	amd8131_walk_children(bus, amd8131_count_dev, &info);

	/* Disable the bus if there are no devices on it or
	 * we are running at 133Mhz and have a 4 function device.
	 * see errata #56
	 */
	if (!bus->children ||
		(info.errata_56 &&
			(info.max_func >= 3) &&
			(PCI_X_SSTATUS_MFREQ(info.sstatus) == PCI_X_SSTATUS_MODE1_133MHZ)))
	{
		unsigned pcix_misc;
		/* Disable all of my children */
		disable_children(bus);

		/* Remember the device is disabled */
		bus->dev->enabled = 0;

		/* Disable the PCI-X clocks */
		pcix_misc = pci_read_config32(bus->dev, 0x40);
		pcix_misc &= ~(0x1f << 16);
		pci_write_config32(bus->dev, 0x40, pcix_misc);

		return;
	}

	/* If we are in conventional PCI mode nothing more is necessary.
	 */
	if (PCI_X_SSTATUS_MFREQ(info.sstatus) == PCI_X_SSTATUS_CONVENTIONAL_PCI) {
		return;
	}


	/* Tune the devices on the bus */
	amd8131_walk_children(bus, amd8131_pcix_tune_dev, &info);

	/* Don't allow the 8131 or any of it's parent busses to
	 * implement relaxed ordering.  Errata #58
	 */
	for (pbus = bus; !pbus->disable_relaxed_ordering; pbus = pbus->dev->bus) {
		printk(BIOS_SPEW, "%s disabling relaxed ordering\n",
			bus_path(pbus));
		pbus->disable_relaxed_ordering = 1;
	}
}

static void amd8131_scan_bridge(struct device *dev)
{
	do_pci_scan_bridge(dev, amd8131_scan_bus);
}


static void amd8131_pcix_init(struct device *dev)
{
	uint32_t dword;
	uint16_t word;
	uint8_t byte;
	int nmi_option;

	/* Enable memory write and invalidate ??? */
	byte = pci_read_config8(dev, 0x04);
	byte |= 0x10;
	pci_write_config8(dev, 0x04, byte);

	/* Set drive strength */
	word = pci_read_config16(dev, 0xe0);
	word = 0x0404;
	pci_write_config16(dev, 0xe0, word);
	word = pci_read_config16(dev, 0xe4);
	word = 0x0404;
	pci_write_config16(dev, 0xe4, word);

	/* Set impedance */
	word = pci_read_config16(dev, 0xe8);
	word = 0x0404;
	pci_write_config16(dev, 0xe8, word);

	/* Set discard unrequested prefetch data */
	/* Errata #51 */
	word = pci_read_config16(dev, 0x4c);
	word |= 1;
	pci_write_config16(dev, 0x4c, word);

	/* Set split transaction limits */
	word = pci_read_config16(dev, 0xa8);
	pci_write_config16(dev, 0xaa, word);
	word = pci_read_config16(dev, 0xac);
	pci_write_config16(dev, 0xae, word);

	/* Set up error reporting, enable all */
	/* system error enable */
	dword = pci_read_config32(dev, 0x04);
	dword |= (1<<8);
	pci_write_config32(dev, 0x04, dword);

	/* system and error parity enable */
	dword = pci_read_config32(dev, 0x3c);
	dword |= (3<<16);
	pci_write_config32(dev, 0x3c, dword);

	/* NMI enable */
	nmi_option = NMI_OFF;
	get_option(&nmi_option, "nmi");
	if (nmi_option) {
		dword = pci_read_config32(dev, 0x44);
		dword |= (1<<0);
		pci_write_config32(dev, 0x44, dword);
	}

	/* Set up CRC flood enable */
	dword = pci_read_config32(dev, 0xc0);
	if (dword) {  /* do device A only */
		dword = pci_read_config32(dev, 0xc4);
		dword |= (1<<1);
		pci_write_config32(dev, 0xc4, dword);
		dword = pci_read_config32(dev, 0xc8);
		dword |= (1<<1);
		pci_write_config32(dev, 0xc8, dword);
	}
	return;
}

#define BRIDGE_40_BIT_SUPPORT 0
#if BRIDGE_40_BIT_SUPPORT
static void bridge_read_resources(struct device *dev)
{
	struct resource *res;
	pci_bus_read_resources(dev);
	res = find_resource(dev, PCI_MEMORY_BASE);
	if (res) {
		res->limit = 0xffffffffffULL;
	}
}

static void bridge_set_resources(struct device *dev)
{
	struct resource *res;
	res = find_resource(dev, PCI_MEMORY_BASE);
	if (res) {
		resource_t base, end;
		/* set the memory range */
		dev->command |= PCI_COMMAND_MEMORY | PCI_COMMAND_MASTER;
		res->flags |= IORESOURCE_STORED;
		base = res->base;
		end  = resource_end(res);
		pci_write_config16(dev, PCI_MEMORY_BASE, base >> 16);
		pci_write_config8(dev, NPUML, (base >> 32) & 0xff);
		pci_write_config16(dev, PCI_MEMORY_LIMIT, end >> 16);
		pci_write_config8(dev, NPUMB, (end >> 32) & 0xff);

		report_resource_stored(dev, res, "");
	}
	pci_dev_set_resources(dev);
}
#endif /* BRIDGE_40_BIT_SUPPORT */

static struct device_operations pcix_ops  = {
#if BRIDGE_40_BIT_SUPPORT
	.read_resources   = bridge_read_resources,
	.set_resources    = bridge_set_resources,
#else
	.read_resources   = pci_bus_read_resources,
	.set_resources    = pci_dev_set_resources,
#endif
	.enable_resources = pci_bus_enable_resources,
	.init             = amd8131_pcix_init,
	.scan_bus         = amd8131_scan_bridge,
	.reset_bus        = pci_bus_reset,
};

static const struct pci_driver pcix_driver __pci_driver = {
	.ops    = &pcix_ops,
	.vendor = PCI_VENDOR_ID_AMD,
	.device = 0x7450,
};


static void ioapic_enable(struct device *dev)
{
	uint32_t value;

	value = pci_read_config32(dev, 0x44);
	if (dev->enabled) {
		value |= ((1 << 1) | (1 << 0));
	} else {
		value &= ~((1 << 1) | (1 << 0));
	}
	pci_write_config32(dev, 0x44, value);
}

static struct pci_operations pci_ops_pci_dev = {
	.set_subsystem    = pci_dev_set_subsystem,
};
static struct device_operations ioapic_ops = {
	.read_resources   = pci_dev_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init             = 0,
	.scan_bus         = 0,
	.enable           = ioapic_enable,
	.ops_pci          = &pci_ops_pci_dev,
};

static const struct pci_driver ioapic_driver __pci_driver = {
	.ops    = &ioapic_ops,
	.vendor = PCI_VENDOR_ID_AMD,
	.device = 0x7451,

};
