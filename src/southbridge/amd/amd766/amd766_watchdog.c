#include <pci.h>
#include <pci_ids.h>
#include <southbridge/amd/amd766.h>

void amd766_disable_watchdog(void)
{
	struct pci_dev *dev;
	dev = pci_find_device(PCI_VENDOR_ID_AMD, PCI_DEVICE_ID_AMD_VIPER_7413, 0);
	if (dev != NULL) {
		unsigned char byte;
		/* Disable the watchdog timer */
		pci_read_config_byte(dev, 0x41, &byte);
		pci_write_config_byte(dev, 0x41, byte | (1<<6)|(1<<2));
	}
}
