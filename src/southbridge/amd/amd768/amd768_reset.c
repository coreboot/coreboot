#include <pci.h>
#include <pci_ids.h>
#include <southbridge/amd/amd766.h>

void amd766_enable_port92_reset(void)
{
	struct pci_dev *dev;
	dev = pci_find_device(PCI_VENDOR_ID_AMD, PCI_DEVICE_ID_AMD_VIPER_7410, 0);
	if (dev != NULL) {
		unsigned char byte;
		/* Enable reset using port 0x92 */
		pci_read_config_byte(dev, 0x41, &byte);		
		pci_write_config_byte(dev, 0x41, byte | (1<<5));
	}
}

void amd766_cpu_reset_sends_init(void)
{
	struct pci_dev *dev;
	dev = pci_find_device(PCI_VENDOR_ID_AMD, PCI_DEVICE_ID_AMD_VIPER_7410, 0);
	if (dev != NULL) {
		unsigned char byte;
		pci_read_config_byte(dev, 0x47, &byte);
		pci_write_config_byte(dev, 0x47, byte | (1<<7));
	}
}

void amd766_decode_stop_grant(unsigned how)
{
	struct pci_dev *dev;
	dev = pci_find_device(PCI_VENDOR_ID_AMD, PCI_DEVICE_ID_AMD_VIPER_7413, 0);
	if (dev != NULL) {
		unsigned char byte;
		pci_read_config_byte(dev, 0x41, &byte);
		pci_write_config_byte(dev, 0x41, byte | ((how & 1)<<1));
	}


}
