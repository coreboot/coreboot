#include <pci.h>
#include <pci_ids.h>
#include <southbridge/amd/amd768.h>
#include <arch/io.h>
#include <printk.h>

void amd768_enable_port92_reset(void)
{
	struct pci_dev *dev;
	dev = pci_find_device(PCI_VENDOR_ID_AMD, PCI_DEVICE_ID_AMD_SB768_7440, 0);
	if (dev != NULL) {
		unsigned char byte;
		/* Enable reset using port 0x92 */
		pci_read_config_byte(dev, 0x41, &byte);		
		pci_write_config_byte(dev, 0x41, byte | (1<<5));
	}
}

void amd768_cpu_reset_sends_init(void)
{
	struct pci_dev *dev;
	dev = pci_find_device(PCI_VENDOR_ID_AMD, PCI_DEVICE_ID_AMD_SB768_7440, 0);
	if (dev != NULL) {
		unsigned char byte;
		pci_read_config_byte(dev, 0x47, &byte);
		pci_write_config_byte(dev, 0x47, byte | (1<<7));
	}
}

void amd768_decode_stop_grant(unsigned how)
{
	struct pci_dev *dev;
	dev = pci_find_device(PCI_VENDOR_ID_AMD, PCI_DEVICE_ID_AMD_SB768_7443, 0);
	if (dev != NULL) {
		unsigned char byte;
		pci_read_config_byte(dev, 0x41, &byte);
		pci_write_config_byte(dev, 0x41, byte | ((how & 1)<<1));
	}
}

void full_reset(void)
{
        struct pci_dev *dev;

	printk_err("Doing a Full Reset\n");
        dev = pci_find_device(PCI_VENDOR_ID_AMD, PCI_DEVICE_ID_AMD_SB768_7443, 0);
        /* make sure the cf9 port is enabled */
        if (dev != NULL) {
                unsigned char byte;
                /* Enable reset port 0xcf9 */
                pci_read_config_byte(dev, 0x41, &byte);
                pci_write_config_byte(dev, 0x41, byte | (1<<7) | (1<<5));
        }
        outb(0x0e, 0x0cf9);  /* full reset command */
        while(1);
}
