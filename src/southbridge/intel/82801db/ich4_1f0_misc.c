#include <pci.h>
#include <arch/io.h>
#include <printk.h>
#include <pci_ids.h>
#include <southbridge/intel/82801.h>
#include "82801.h"

void ich3_1f0_misc(void)
{
	struct pci_dev *dev;

	dev = pci_find_device(PCI_VENDOR_ID_INTEL, 
			PCI_DEVICE_ID_INTEL_82801CA_1F0, 0);
	if (!dev) {
  		printk_debug("*** ERROR Southbridge device %x not found\n",
			PCI_DEVICE_ID_INTEL_82801CA_1F0);
		return;
	}
	pci_write_config_word(dev, PCICMD, 0x014f);
	pci_write_config_dword(dev, PMBASE, 0x00001001);
	pci_write_config_byte(dev, ACPI_CNTL, 0x10);
	pci_write_config_dword(dev, GPIO_BASE, 0x00001181);
	pci_write_config_byte(dev, GPIO_CNTL, 0x10);
	pci_write_config_dword(dev, PIRQA_ROUT, 0x0A05030B);
	pci_write_config_byte(dev, PIRQE_ROUT, 0x07);
	pci_write_config_byte(dev, RTC_CONF, 0x04);
	pci_write_config_byte(dev, COM_DEC, 0xE0);
	pci_write_config_word(dev, LPC_EN, 0x000D);
	pci_write_config_word(dev, FUNC_DIS, 0x8060);
}
