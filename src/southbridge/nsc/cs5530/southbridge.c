/* 
    freebios/src/southbridge/nsc/cs5530/southbridge.c

    Copyright (c) 2002 Christer Weinigel <wingel@hack.org>

    Configure the National Semiconductor CS5530 southbridge 
*/

#include <printk.h>
#include <pci.h>
#include <pci_ids.h>
#include <cpu/p5/io.h>

#include <types.h>

#define NAME "cs5530: "

void nvram_on()
{
        struct pci_dev *dev;
        u8 b;

        printk_info("Enable FLASH\n");

        dev = pci_find_device(PCI_VENDOR_ID_CYRIX,
                              PCI_DEVICE_ID_CYRIX_5530_LEGACY,
                              (void *)NULL);
        if (!dev) {
                printk_warning(NAME "Can't find PCI bridge\n");
                return;
        }
	
	pci_read_config_byte(dev, 0x52, &b);
	printk_debug("Set F0/0x52 to 0xee\n");
	b = 0xee;
	pci_write_config_byte(dev, 0x52, b);

	// now set PCI decode
	pci_read_config_byte(dev, 0x5b, &b);
	b |= 1 << 5;
	/* why would you ever turn these off? */
#define CONFIG_CS5530_PRIMARY_IDE
#define CONFIG_CS5530_SECONDARY_IDE

#ifdef CONFIG_CS5530_PRIMARY_IDE
	printk_info(NAME "Enabling Primary IDE Controller\n");
	b |= 1<<3;
#endif
#ifdef CONFIG_CS5530_SECONDARY_IDE
	printk_info(NAME "Enabling Secondary IDE Controller\n");
	b |= 1<<4;
#endif

	printk_debug("Set F0/0x5b to |= 1 << 5(0x%x)\n", b);
	pci_write_config_byte(dev, 0x5b, b);


}

void final_southbridge_fixup()
{
	struct pci_dev *dev;
	u8 b;
	u16 int_map;
	int inta, intb, intc, intd;
	u16 trig;

	printk_info("Final southbridge fixup\n");

	dev = pci_find_device(PCI_VENDOR_ID_CYRIX, 
			      PCI_DEVICE_ID_CYRIX_5530_LEGACY, 
			      (void *)NULL);
	if (!dev) {
		printk_warning(NAME "Can't find PCI bridge\n");
		return;
	}

#if !(HAVE_PIRQ_TABLE)
	pci_read_config_word(dev, 0x5c, &int_map);
	printk_debug("F0/5c: 0x%04x\n", int_map);
	inta = CS5530_INTA;
	intb = CS5530_INTB;
	intc = CS5530_INTC;
	intd = CS5530_INTD;
	printk_info(NAME "PCI INTA=%d, INTB=%d, INTC=%d, INTD=%d\n",
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
#endif
	pci_read_config_byte(dev, 0x5b, &b);
	printk_debug("F0/5b = 0x%02x\n", b);
#ifdef CS5530_PRIMARY_IDE
	printk_info(NAME "Enabling Primary IDE Controller\n");
	b |= 1<<3;
#endif
#ifdef CS5530_SECONDARY_IDE
	printk_info(NAME "Enabling Secondary IDE Controller\n");
	b |= 1<<4;
#endif
	printk_debug("F0/5b = 0x%02x\n", b);
	pci_write_config_byte(dev, 0x5b, b);

#if !(HAVE_PIRQ_TABLE)
	dev = pci_find_slot(0, PCI_DEVFN(0x13, 0));
	if (dev) {
		printk_debug(NAME "USB is on INTA, IRQ %d\n", inta);
		pci_write_config_byte(dev, PCI_INTERRUPT_LINE, inta);
	}
#endif
}

/*
    Local variables:
        compile-command: "make -C /export/bios/voyager2"
        c-basic-offset: 8
    End:
*/
