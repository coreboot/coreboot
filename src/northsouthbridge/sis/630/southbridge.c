
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
		pci_write_config_byte(pcidev, 0x40, 0x3);
		/* Flash can be flashed */
		pci_write_config_byte(pcidev, 0x45, 0x40);
		printk(KERN_INFO "Enabled in SIS 503 regs 0x40 and 0x45\n");

	}
	printk(KERN_INFO "Now try to turn off shadow\n");
	/* turn off nvram shadow in 0xc0000 ~ 0xfffff */
	pcidev = pci_find_device(PCI_VENDOR_ID_SI, PCI_DEVICE_ID_SI_630, (void *)NULL);
	printk(KERN_INFO "device for SiS 630 is 0x%x\n", pcidev);
	if (pcidev != NULL) {
		/* read cycle goes to System Memory */
		pci_write_config_word(pcidev, 0x70, 0x1fff);
		/* write cycle goest to System Memory */
		pci_write_config_word(pcidev, 0x72, 0x1fff);
		printk(KERN_INFO "Shadow memory disabled in SiS 630\n");

	}
}

// simple fixup (which we hope can leave soon) for the sis southbridge part
void
final_southbridge_fixup()
{
    struct pci_dev *pcidev;
    
    // ethernet fixup. This should all work, and doesn't, yet. 
    // so we hack it for now. 
    // need a manifest constant for the enet device. 
    pcidev = pci_find_device(PCI_VENDOR_ID_SI, 	0x0900, 
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
	} else {
	    printk(KERN_ERR "Can't find ethernet interface\n");
	}

	// fix up southbridge interrupt enables. 
	pcidev = pci_find_device(PCI_VENDOR_ID_SI, PCI_DEVICE_ID_SI_503, 
	    (void *)NULL);
	if (pcidev != NULL) {
	    // put symbolic names in here soon ... so much typing, so little time. 

	    // remap IRQ for PCI -- this is exactly what the BIOS does now. 
	    pci_write_config_byte(pcidev, 0x42, 0xa);
	    pci_write_config_byte(pcidev, 0x43, 0xb);
	    pci_write_config_byte(pcidev, 0x44, 0xc);

	    // now set up legacy serial interrupts
	    // that doesn't mean serial ports -- it's a serial interrupt
	    // line from the superio (LPC)
	    // first, enable them. 
	    pci_write_config_byte(pcidev, 0x70, 0x80); 
	    // skip the SMI, inta, etc. stuff. 
	    // enable IRQs 7 to 1
	    pci_write_config_byte(pcidev, 0x72, 0xfd);
	    pci_write_config_byte(pcidev, 0x73, 0xff);
	    
	} else {
	    printk(KERN_EMERG "Can't find south bridge!\n");
	}

	printk(KERN_INFO "Southbridge fixup done for SIS 503\n");
}
