/*
 * (C) 2003-2004 Linux Networx
 */
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <device/pcix.h>
#include <pc80/mc146818rtc.h>
#include <arch/ioapic.h>
#include <delay.h>
#include "pxhd.h"

static void pxhd_enable(device_t dev)
{
	device_t bridge;
	uint16_t value;
	if ((dev->path.pci.devfn & 1) == 0) {
		/* Can we enable/disable the bridges? */
	  	return;
	}
	bridge = dev_find_slot(dev->bus->secondary, dev->path.pci.devfn & ~1);
	if (!bridge) {
		printk(BIOS_ERR, "Cannot find bridge for ioapic: %s\n",
			   dev_path(dev));
		return;
	}
	value = pci_read_config16(bridge, 0x40);
	value &= ~(1 << 13);
	if (!dev->enabled) {
		value |= (1 << 13);
	}
	pci_write_config16(bridge, 0x40, value);
}


#define NMI_OFF 0

static unsigned int pxhd_scan_bridge(device_t dev, unsigned int max)
{
	int bus_100Mhz = 0;

	dev->link_list->dev = dev;

	get_option(&bus_100Mhz, "pxhd_bus_speed_100");
	if(bus_100Mhz) {
		uint16_t word;

		printk(BIOS_DEBUG, "setting pxhd bus to 100 Mhz\n");
		/* set to pcix 100 mhz */
		word = pci_read_config16(dev, 0x40);
		word &= ~(3 << 14);
		word |= (1 << 14);
		word &= ~(3 << 9);
		word |= (2 << 9);
		pci_write_config16(dev, 0x40, word);

		/* reset the bus to make the new frequencies effective */
		pci_bus_reset(dev->link_list);
	}
	return pcix_scan_bridge(dev, max);
}
static void pcix_init(device_t dev)
{
	/* Bridge control ISA enable */
	pci_write_config8(dev, 0x3e, 0x07);

	// FIXME Please review lots of dead code here.
#if 0
	int nmi_option;
	uint32_t dword;
	uint16_t word;
	uint8_t byte;

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
	if(nmi_option) {
		dword = pci_read_config32(dev, 0x44);
        	dword |= (1<<0);
        	pci_write_config32(dev, 0x44, dword);
	}

	/* Set up CRC flood enable */
	dword = pci_read_config32(dev, 0xc0);
	if(dword) {  /* do device A only */
		dword = pci_read_config32(dev, 0xc4);
		dword |= (1<<1);
		pci_write_config32(dev, 0xc4, dword);
		dword = pci_read_config32(dev, 0xc8);
		dword |= (1<<1);
		pci_write_config32(dev, 0xc8, dword);
	}

	return;
#endif
}

static struct device_operations pcix_ops  = {
        .read_resources   = pci_bus_read_resources,
        .set_resources    = pci_dev_set_resources,
	.enable_resources = pci_bus_enable_resources,
        .init             = pcix_init,
        .scan_bus         = pxhd_scan_bridge,
	.reset_bus        = pci_bus_reset,
	.ops_pci          = 0,
};

static const struct pci_driver pcix_driver __pci_driver = {
        .ops    = &pcix_ops,
        .vendor = PCI_VENDOR_ID_INTEL,
        .device = 0x0329,
};

static const struct pci_driver pcix_driver2 __pci_driver = {
        .ops    = &pcix_ops,
        .vendor = PCI_VENDOR_ID_INTEL,
        .device = 0x032a,
};

static void ioapic_init(device_t dev)
{
	uint32_t value, ioapic_base;
	/* Enable bus mastering so IOAPICs work */
	value = pci_read_config16(dev, PCI_COMMAND);
	value |= PCI_COMMAND_MASTER;
	pci_write_config16(dev, PCI_COMMAND, value);

	ioapic_base = pci_read_config32(dev, PCI_BASE_ADDRESS_0);

	setup_ioapic(ioapic_base, 0); // Don't rename IOAPIC ID
}

static void intel_set_subsystem(device_t dev, unsigned vendor, unsigned device)
{
	pci_write_config32(dev, PCI_SUBSYSTEM_VENDOR_ID,
		((device & 0xffff) << 16) | (vendor & 0xffff));
}

static struct pci_operations intel_ops_pci = {
	.set_subsystem = intel_set_subsystem,
};

static struct device_operations ioapic_ops = {
	.read_resources   = pci_dev_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init             = ioapic_init,
	.scan_bus         = 0,
	.enable           = pxhd_enable,
	.ops_pci          = &intel_ops_pci,
};

static const struct pci_driver ioapic_driver __pci_driver = {
	.ops    = &ioapic_ops,
	.vendor = PCI_VENDOR_ID_INTEL,
	.device = 0x0326,

};

static const struct pci_driver ioapic2_driver __pci_driver = {
	.ops    = &ioapic_ops,
	.vendor = PCI_VENDOR_ID_INTEL,
	.device = 0x0327,

};

struct chip_operations southbridge_intel_pxhd_ops = {
	CHIP_NAME("Intel PXHD Southbridge")
	.enable_dev = pxhd_enable,
};
