/*
 * (C) 2003 Linux Networx
 */
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <pc80/mc146818rtc.h>

#define NMI_OFF 0

static void pcix_init(device_t dev)
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
        word = 0x0808;
        pci_write_config16(dev, 0xe0, word);
	word = pci_read_config16(dev, 0xe4);
        word = 0x0808;
        pci_write_config16(dev, 0xe4, word);
	
	/* Set impedance */
	word = pci_read_config16(dev, 0xe8);
        word = 0x0f0f;
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
}

static struct device_operations pcix_ops  = {
        .read_resources   = pci_bus_read_resources,
        .set_resources    = pci_dev_set_resources,
	.enable_resources = pci_bus_enable_resources,
        .init             = pcix_init,
        .scan_bus         = pci_scan_bridge,
};

static struct pci_driver pcix_driver __pci_driver = {
        .ops    = &pcix_ops,
        .vendor = PCI_VENDOR_ID_AMD,
        .device = 0x7450,
};


static void ioapic_enable(device_t dev)
{
	uint32_t value;

	value = pci_read_config32(dev, 0x44);
	if (dev->enabled) {
		value |= ((1 << 1) | (1 << 0));
	} else {
		value &= ~((1 << 1) | (1 << 0));
	}
	pci_write_config32(dev, 0x44, value);

	/* We have to enable MEM and Bus Master for IOAPIC */
        value = pci_read_config32(dev, 0x4);
        value |= 6;
        pci_write_config32(dev, 0x4, value);
}

static struct device_operations ioapic_ops = {
	.read_resources   = pci_dev_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init     = 0,
	.scan_bus = 0,
	.enable   = ioapic_enable,
};

static struct pci_driver ioapic_driver __pci_driver = {
	.ops    = &ioapic_ops,
	.vendor = PCI_VENDOR_ID_AMD,
	.device = 0x7451,
	
};
