#include <pci.h>
#include <pci_ids.h>
#include <southbridge/amd/amd768.h>

void amd768_enable_ioapic(void)
{
	struct pci_dev *dev;
	dev = pci_find_device(PCI_VENDOR_ID_AMD, PCI_DEVICE_ID_AMD_SB768_7440,0);
	if (dev != NULL) {
		/* Enable the ioapic */
		pci_write_config_byte(dev, 0x4b, (1 <<7)|(0 << 3)|(1<< 0));
	}
        dev = pci_find_device(PCI_VENDOR_ID_AMD, PCI_DEVICE_ID_AMD_SB768_7448,0);
        if (dev != NULL) {
                /* Enable the ioapic */
                pci_write_config_byte(dev, 0x3e, 4);
        }

	
}
