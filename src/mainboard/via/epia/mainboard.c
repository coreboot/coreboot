#include <printk.h>
#include <pci.h>
#include <pci_ids.h>
#include <cpu/p5/io.h>

#include <types.h>

void pci_routing_fixup(void)
{
	struct pci_dev *dev;

	dev = pci_find_device(PCI_VENDOR_ID_VIA, PCI_DEVICE_ID_VIA_8231, 0);
	if (dev != NULL) {
		/* 
		 * initialize PCI interupts - these assignments depend
		 * on the PCB routing of PINTA-D
		 */
		pci_write_config_byte(dev, 0x55, 0xb0);
		pci_write_config_byte(dev, 0x56, 0xa5);
		pci_write_config_byte(dev, 0x57, 0xc0);
	}
}

void
ethernet_fixup()
{
	struct pci_dev	*dev;
	u8		byte;

	printk_info("Ethernet fixup\n");
	dev = pci_find_device(PCI_VENDOR_ID_VIA, PCI_DEVICE_ID_VIA_8231, 0);
	if (dev != NULL) {
		/* enable lan */
		pci_read_config_byte(dev, 0x51, &byte);
		printk_debug("Get FC 2 in reg. 51 is 0x%x\n", byte);
		byte |= 0x30;
		printk_debug("Set FC 2 in reg. 51 to 0x%x\n", byte);
		pci_write_config_byte(dev, 0x51, byte);

		/* setup interrupt */
		pci_read_config_byte(dev, 0x4d, &byte);
		printk_debug("Get IRQ Control in reg. 4d is 0x%x\n", byte);
		byte |= 0x20;
		printk_debug("Set IRQ Control in reg. 4d to 0x%x\n", byte);
		pci_write_config_byte(dev, 0x4d, byte);

		/* setup PINTA */
		pci_read_config_byte(dev, 0x55, &byte);
		printk_debug("Get IRQ Control in reg. 55 is 0x%x\n", byte);
		byte = 0x50;
		printk_debug("Set IRQ Control in reg. 55 to 0x%x\n", byte);
		pci_write_config_byte(dev, 0x55, byte);
	}
}

void
mainboard_fixup()
{
	printk_info("Mainboard fixup\n");
}

void
final_southbridge_fixup()
{
	printk_info("Southbridge fixup\n");

	nvram_on();
	keyboard_on();
	southbridge_fixup();
	pci_routing_fixup();
	ethernet_fixup();
}

void
final_mainboard_fixup()
{
	final_southbridge_fixup();
	printk_info("Final mainboard fixup\n");
}

