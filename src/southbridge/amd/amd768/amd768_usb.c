#include <pci.h>
#include <pci_ids.h>
#include <southbridge/amd/amd766.h>

void amd766_usb_setup(void)
{
	/* FIXME this is untested incomplete implementation. */
	struct pci_dev *dev;
	dev = pci_find_device(PCI_VENDOR_ID_AMD, PCI_DEVICE_ID_AMD_VIPER_7414, 0);
	if (dev) {
		u32 cmd;
		pci_read_config_dword(dev, PCI_COMMAND, &cmd);
		pci_write_config_dword(dev, PCI_COMMAND, 
			cmd | PCI_COMMAND_IO | PCI_COMMAND_MEMORY | 
			PCI_COMMAND_MASTER | PCI_COMMAND_INVALIDATE);
	}
}
