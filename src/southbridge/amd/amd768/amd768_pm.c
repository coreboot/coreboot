#include <pci.h>
#include <pci_ids.h>
#include <southbridge/amd/amd768.h>

void amd768_set_pm_classcode(void)
{
	struct pci_dev *dev;
	dev = pci_find_device(PCI_VENDOR_ID_AMD, PCI_DEVICE_ID_AMD_SB768_7443, 0);
	if (dev != NULL) {
		pci_write_config_dword(dev, 0x60, 0x06800000);
	}

}
