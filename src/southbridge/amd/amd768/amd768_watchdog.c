#include <pci.h>
#include <pci_ids.h>
#include <southbridge/amd/amd768.h>

void amd768_disable_watchdog(void)
{
	struct pci_dev *dev;
	dev = pci_find_device(PCI_VENDOR_ID_AMD, PCI_DEVICE_ID_AMD_SB768_7443, 0);
	if (dev != NULL) {
		unsigned char byte;
		/* Disable the watchdog timer */
		pci_read_config_byte(dev, 0x41, &byte);
		pci_write_config_byte(dev, 0x41, byte | (1<<6));
		pci_read_config_byte(dev, 0x49, &byte);
		pci_write_config_byte(dev, 0x49, byte | (1<<2));
	}
}
