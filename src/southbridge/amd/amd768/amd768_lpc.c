#include <pci.h>
#include <pci_ids.h>
#include <southbridge/amd/amd766.h>
#include <printk.h>

void amd766_enable_serial_irqs(int continuous, unsigned frames, unsigned startclocks)
{
	struct pci_dev *dev;
	unsigned value;
	value = ((!!continuous) << 6)|((frames & 0xf) << 2)|((startclocks & 3) << 0);
	
	dev = pci_find_device(PCI_VENDOR_ID_AMD, PCI_DEVICE_ID_AMD_VIPER_7443,0);
	if (dev != NULL) {
		/* Setup serial irq's for the LPC bus. */
		pci_write_config_byte(dev, 0x4a, value);
	} else
		printk_alert("Could not locate ACPI bridge\n");
	
}
