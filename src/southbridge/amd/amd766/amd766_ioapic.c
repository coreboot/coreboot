#include <pci.h>
#include <pci_ids.h>
#include <southbridge/amd/amd766.h>

void amd766_enable_ioapic(void)
{
	struct pci_dev *dev;
	dev = pci_find_device(PCI_VENDOR_ID_AMD, PCI_DEVICE_ID_AMD_VIPER_7410,0);
	if (dev != NULL) {
		/* Enable the ioapic */
		pci_write_config_byte(dev, 0x4b, (0 << 3)|(0 <<2)|(0 << 1)|(1<< 0));
	}
	
}
