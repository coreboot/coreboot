#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>

static void acpi_init(struct device *dev)
{
	uint8_t byte;
	uint16_t word;

#if 0
	printk_debug("ACPI: disabling NMI watchdog.. ");
	pci_read_config_byte(dev, 0x49, &byte);
	pci_write_config_byte(dev, 0x49, byte | (1<<2));


	pci_read_config_byte(dev, 0x41, &byte);
	pci_write_config_byte(dev, 0x41, byte | (1<<6)|(1<<2));

	/* added from sourceforge */
	pci_read_config_byte(dev, 0x48, &byte);
	pci_write_config_byte(dev, 0x48, byte | (1<<3));

	printk_debug("done.\n");


	printk_debug("ACPI: Routing IRQ 12 to PS2 port.. ");
	pci_read_config_word(dev, 0x46, &word);
	pci_write_config_word(dev, 0x46, word | (1<<9));
	printk_debug("done.\n");

	
	printk_debug("ACPI: setting PM class code.. ");
	pci_write_config_dword(dev, 0x60, 0x06800000);
	printk_debug("done.\n");
#endif

}

static struct device_operations acpi_ops  = {
	.read_resources   = pci_dev_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init = acpi_init,
	.scan_bus = 0,
};

static struct pci_driver acpi_driver __pci_driver = {
	.ops    = &acpi_ops,
	.vendor = PCI_VENDOR_ID_AMD,
	.device = PCI_DEVICE_ID_AMD_8111_ACPI,
};

