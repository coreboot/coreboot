#include <pci.h>
#include <pci_ids.h>
#include <southbridge/amd/amd768.h>

void amd768_power_after_power_fail(int on)
{
	struct pci_dev *dev;
	dev = pci_find_device(PCI_VENDOR_ID_AMD, PCI_DEVICE_ID_AMD_SB768_7443, 0);
	if (dev != NULL) {
		unsigned char byte;
		pci_read_config_byte(dev, 0x43, &byte);
		if (on) { 
			byte &= ~(1<<6);
		}
		else {
			byte |= (1<<6);
		}
		pci_write_config_byte(dev, 0x43, byte);
	}

}
