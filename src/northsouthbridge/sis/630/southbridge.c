
/*
 * Bootstrap code for the INTEL 
 * $Id$
 *
 */

#ifndef lint
static char rcsid[] =
"$Id$";
#endif


#include <printk.h>
#include <pciconf.h>
#include <subr.h>
#include <pci.h>
#include <pci_ids.h>
#include <northsouthbridge/sis/630/param.h>

void keyboard_on()
{
	u8 regval;
	struct pci_dev *pcidev;

	/* turn on sis630 keyboard/mouse controller */
	pcidev = pci_find_device(PCI_VENDOR_ID_SI, PCI_DEVICE_ID_SI_503, (void *)NULL);
	if (pcidev != NULL) {
		/* Register 0x47, Keyboard Controller */
		pci_read_config_byte(pcidev, 0x47, &regval);
		/* enable both integrated keyboard and PS/2 mouse controller */
		regval |= 0x0c;
		pci_write_config_byte(pcidev, 0x47, regval);
	}
	pc_keyboard_init();
}

void nvram_on()
{
	struct pci_dev *pcidev;

	/* turn on sis630 nvram. */
	pcidev = pci_find_device(PCI_VENDOR_ID_SI, PCI_DEVICE_ID_SI_503, (void *)NULL);
	if (pcidev != NULL) {
		/* Enable FFF80000 to FFFFFFFF decode */
		pci_write_config_byte(pcidev, 0x40, 0x33);
		/* Flash can be flashed */
		pci_write_config_byte(pcidev, 0x45, 0x40);
	}

	/* turn off nvram shadow in 0xc0000 ~ 0xfffff */
	pcidev = pci_find_device(PCI_VENDOR_ID_SI, PCI_DEVICE_ID_SI_630, (void *)NULL);
	if (pcidev != NULL) {
		/* read cycle goes to System Memory */
		pci_write_config_word(pcidev, 0x70, 0x1fff);
		/* write cycle goest to System Memory */
		pci_write_config_word(pcidev, 0x72, 0x1fff);
	}
}
/* turn on the floppy. On some chipsets you have to do this. */
#ifdef MUST_ENABLE_FLOPPY
void enable_floppy()
{
	/* unlock it XXX make this a subr at some point */
	outb(0x87, 0x2e);
	outb(0x01, 0x2e);
	outb(0x55, 0x2e);
	outb(0x55, 0x2e);

	/* now set the LDN to floppy LDN */
	outb(0x7, 0x2e);	/* pick reg. 7 */
	outb(0x0, 0x2f);	/* LDN 0 to reg. 7 */

	/* now select register 0x30, and set bit 1 in that register */
	outb(0x30, 0x2e);
	outb(0x1, 0x2f);

	/* all done. */
	outb(1, 0x2e);
}
#endif /* MUST_ENABLE_FLOPPY */

// simple fixup (which we hope can leave soon) for the sis southbridge part
void
southbridge_fixup()
{
    struct pci_dev *pcidev;
    
    // ethernet fixup. This should all work, and doesn't, yet. 
    // so we hack it for now. 
    pcidev = pci_find_device(PCI_VENDOR_ID_SI, 	PCI_DEVICE_ID_SI_503, 
	(void *)NULL);
	if (pcidev != NULL) {
	    u32 bar0 = 0xb001;
	    // set the BAR 0 to 0xb000. Safe, high value, known good. 
	    // pci config set doesn't work for reasons we don't understand. 
	    pci_write_config_dword(pcidev, PCI_BASE_ADDRESS_0, bar0);

	    // Make sure bus mastering is on. The tried-and-true probe in linuxpci.c 
	    // doesn't set this for some reason. 
	    pci_write_config_byte(pcidev, PCI_COMMAND, 
		PCI_COMMAND_IO | PCI_COMMAND_MEMORY | PCI_COMMAND_MASTER);

	    // set the interrupt to 'b'
	    pci_write_config_byte(pcidev, PCI_INTERRUPT_LINE, 0xb);
	}

	printk(KERN_INFO "Southbridge fixup done for SIS 503\n");
}
