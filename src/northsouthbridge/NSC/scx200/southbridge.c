/* freebios/src/northsouthbridge/NSC/scx200/northbridge.c

   Copyright (c) 2002 Christer Weinigel <wingel@hack.org>

   Do chipset setup for a National Semiconductor SCx200 CPU.
*/

#include <printk.h>
#include <pci.h>
#include <pci_ids.h>
#include <cpu/p5/io.h>

#include <types.h>

void nvram_on()
{
}

// simple fixup (which we hope can leave soon) for the sis southbridge part
void
final_southbridge_fixup()
{
	struct pci_dev *dev;
	u8 b;
	u16 int_map;
	int inta, intb, intc, intd;
	u16 trig;

	printk_info("Final southbridge fixup\n");

	dev = pci_find_device(PCI_VENDOR_ID_NS, 
			      PCI_DEVICE_ID_NS_SCx200_BRIDGE, 
			      (void *)NULL);
	if (!dev) {
		printk_warning("scx200: Can't find PCI bridge\n");
		return;
	}

	pci_read_config_word(dev, 0x5c, &int_map);
	printk_debug("F0/5c: 0x%04x\n", int_map);
	inta = SCx200_INTA;
	intb = SCx200_INTB;
	intc = SCx200_INTC;
	intd = SCx200_INTD;
	printk_info("scx200: PCI INTA=%d, INTB=%d, INTC=%d, INTD=%d\n",
		    inta, intb, intc, intd);
	int_map = inta | (intb<<4) | (intc<<8) | (intd<<12);
	pci_write_config_word(dev, 0x5c, int_map);

	pci_read_config_word(dev, 0x5c, &int_map);
	printk_debug("F0/5c: 0x%04x\n", int_map);

	/* All PCI interrupts are level trigged, the rest are edge trigged */
	printk_debug("4d0: 0x%02x\n", inb(0x4d0));
	printk_debug("4d1: 0x%02x\n", inb(0x4d1));
	trig = 0;
	if (inta) trig |= (1<<inta);
	if (intb) trig |= (1<<intb);
	if (intc) trig |= (1<<intc);
	if (intd) trig |= (1<<intd);
	outb(trig, 0x4d0);
	outb(trig >> 8, 0x4d1);
	printk_debug("4d0: 0x%02x\n", inb(0x4d0));
	printk_debug("4d1: 0x%02x\n", inb(0x4d1));

	pci_read_config_byte(dev, 0x5b, &b);
	printk_debug("F0/5b = 0x%02x\n", b);
#ifdef SCx200_PRIMARY_IDE
	printk_info("scx200: Enabling Primary IDE Controller\n");
	b |= 1<<3;
#endif
#ifdef SCx200_SECONDARY_IDE
	printk_info("scx200: Enabling Secondary IDE Controller\n");
	b |= 1<<4;
#endif
	printk_debug("F0/5b = 0x%02x\n", b);
	pci_write_config_byte(dev, 0x5b, b);

	dev = pci_find_slot(0, PCI_DEVFN(0x13, 0));
	if (dev) {
		printk_debug("scx200: USB is on INTD, IRQ %d\n", intd);
		pci_write_config_byte(dev, PCI_INTERRUPT_PIN, 4);
		pci_write_config_byte(dev, PCI_INTERRUPT_LINE, intd);
	}
}

/*
    Local variables:
        compile-command: "make -C /export/nano/bios/nano"
        c-basic-offset: 8
    End:
*/
