#include <printk.h>
#include <pci.h>
#include <pci_ids.h>
#include <cpu/p5/io.h>

#include <types.h>

//static const unsigned char usbIrqs[4] = { 11, 5, 10, 12 };
static const unsigned char usbIrqs[4] = { 11, 12, 10, 5 };
static const unsigned char enetIrqs[4] = { 11, 5, 10, 12 };
static const unsigned char slotIrqs[4] = { 10, 12, 5, 11 };
static const unsigned char firewireIrqs[4] = { 12, 10, 5, 11 };

/*
	Our IDSEL mappings are as follows
	PCI slot is AD31          (device 15) (00:14.0)
	Southbridge is AD28       (device 12) (00:11.0)
*/
static void pci_routing_fixup(void)
{
	struct pci_dev *dev;

        dev = pci_find_device(PCI_VENDOR_ID_VIA, 0x3177, 0);
	if (dev != NULL) {
		/*
		 * initialize PCI interupts - these assignments depend
		 * on the PCB routing of PINTA-D
		 *
		 * PINTA = IRQ11
		 * PINTB = IRQ12
		 * PINTC = IRQ10
		 * PINTD = IRQ5
		 */
		pci_write_config_byte(dev, 0x55, 0xb0);
		pci_write_config_byte(dev, 0x56, 0xac);
		pci_write_config_byte(dev, 0x57, 0x50);

	}
#if 1
	// firewire built into southbridge
	printk_info("setting firewire\n");
	pci_assign_irqs(0, 0x0d, firewireIrqs);

	// Standard usb components
	printk_info("setting usb\n");
	pci_assign_irqs(0, 0x10, usbIrqs);

	// Ethernet built into southbridge
	printk_info("setting ethernet\n");
	pci_assign_irqs(0, 0x12, enetIrqs);

	// PCI slot
	printk_info("setting pci slot\n");
	pci_assign_irqs(0, 0x14, slotIrqs);

#endif

		printk_debug("4d0: 0x%02x\n", inb(0x4d0));
		printk_debug("4d1: 0x%02x\n", inb(0x4d1));
#if 0
		outb(0, 0x4d0);
		outb(0, 0x4d1);
#endif
		printk_debug("4d0: 0x%02x\n", inb(0x4d0));
		printk_debug("4d1: 0x%02x\n", inb(0x4d1));
}


void
mainboard_fixup()
{
	printk_info("Mainboard fixup\n");

	northbridge_fixup();
	southbridge_fixup();
}

void
final_southbridge_fixup()
{
	printk_info("Southbridge fixup\n");

	nvram_on();
//	keyboard_on();
	pci_routing_fixup();
}

void
final_mainboard_fixup()
{
	printk_info("Final mainboard fixup\n");
	final_southbridge_fixup();
}

