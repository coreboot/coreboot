#include <pci.h>
#include <pci_ids.h>
#include <southbridge/amd/amd766.h>

void amd766_posted_memory_write_enable(void)
{
	struct pci_dev *dev;
	dev = pci_find_device(PCI_VENDOR_ID_AMD, PCI_DEVICE_ID_AMD_VIPER_7410, 0);
	if (dev != NULL) {
		unsigned char byte;
		pci_read_config_byte(dev, 0x46, &byte);
		pci_write_config_byte(dev, 0x46, byte | (1<<0));
	}

}
