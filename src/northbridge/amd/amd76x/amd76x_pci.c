#include <pci.h>
#include <pci_ids.h>
#include <northbridge/amd/amd76x.h>

/* FIXME add support for 66Mhz pci */
void amd76x_setup_pci_arbiter(void)
{
	struct pci_dev *dev;
	dev = pci_find_device(PCI_VENDOR_ID_AMD, PCI_DEVICE_ID_AMD_FE_GATE_700C, 0);
	if (dev) {
		/* Enable:
		 * PCI parking
		 * memory prefetching
		 * EV6 mode
		 * Enable power management registers
		 * The southbridge lock
		 * Read data error disable
		 * PCI retries
		 * AGP retries
		 * AGP chaining
		 * PCI chaining
		 */
		pci_write_config_dword(dev, 0x84, 
			(0<<24)
			|(1<<23)
			|(1<<17)|(1<<16)
			|(0<<15)|(1<<14)|(1<<13)|(1<<12)
			|(0<<11)|(0<<10)|(0<<9)|(0<<8)
			|(1<<7)|(0<<6)|(0<<5)|(1<<4)
			|(0<<3)|(1<<2)|(1<<1)|(1<<0));
	}
}


