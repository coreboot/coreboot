/* freebios/src/mainboard/nano/nano/mainboard.c

   Copyright (c) 2002 Christer Weinigel <wingel@hack.org>

   Nano Computer support.
*/

#include <printk.h>
#include <pci.h>
#include <pci_ids.h>
#include <cpu/p5/io.h>

#include <types.h>

void
mainboard_fixup()
{
}

void
final_mainboard_fixup()
{
	struct pci_dev *dev;
	int i;

	void final_southbridge_fixup(void);
	final_southbridge_fixup();

	printk_info("Final mainboard fixup\n");

	dev = pci_find_slot(0, PCI_DEVFN(0x0f, 0));
	if (dev) {
		printk_debug("nano: Setting eth0 IRQ to %d\n", SCx200_INTA);
		pci_write_config_byte(dev, PCI_INTERRUPT_LINE, SCx200_INTA);
	}

	dev = pci_find_slot(0, PCI_DEVFN(0x10, 0));
	if (dev) {
		printk_debug("nano: Setting eth1 IRQ to %d\n", SCx200_INTA);
		pci_write_config_byte(dev, PCI_INTERRUPT_LINE, SCx200_INTA);
	}

	for (i = 0; i < 7; i++) {
		u8 pin;
		int irq;

		dev = pci_find_slot(0, PCI_DEVFN(0x11, i));
		if (!dev)
			break;

		pci_read_config_byte(dev, PCI_INTERRUPT_PIN, &pin);
		if (!pin) {
			continue;
		}
		switch (pin) {
		case 1:
		case 3:
			irq = SCx200_INTC;
			break;
		case 2:
		case 4:
			irq = SCx200_INTB;
			break;
		default:
			irq = 0;
		}

		if (irq) {
			printk_info("nano: Setting PCI slot function %d IRQ to %d\n", i, irq);
			pci_write_config_byte(dev, PCI_INTERRUPT_LINE, irq);
			dev->irq = irq;
		}
	}
}

/*
    Local variables:
        compile-command: "make -C /export/nano/bios/nano"
        c-basic-offset: 8
    End:
*/
