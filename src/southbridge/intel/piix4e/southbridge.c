#include <pci.h>
#include <pc80/keyboard.h>
#include <printk.h>
#include <subr.h>

void
southbridge_fixup()
{
#if (CONFIG_LINUXBIOS_ENABLE_IDE == 1)
        struct pci_dev *pcidev;

        printk_info( "Enabling IDE...");

        pcidev = pci_find_device(0x8086, 0x7111, (void *)NULL);

	if (! pcidev) {
		printk_err("Can't find piix4e\n");
	} else {
		unsigned char c;

		pci_read_config_word(pcidev, 0x41, &c);
		c |= 0x80;
		pci_read_config_word(pcidev, 0x43, &c);
		c |= 0x80;
		printk_info("Enabled IDE for channels 1 and 2\n");
	}

        printk_info("done.\n");
#endif
}

void nvram_on()
{
	/*; now set up PIIX4e registers 4e and 4f for nvram access.
	   ; 4e will have value 0xc3, 4f will have value 2
	   ; we are going to PIIX4 function 0; the PIIX4 is device 0x12.
	   ; bit coding is 0x80000000 + 0x9000 (dev 0x12)  + 0x4c (x4 aligned)
	   ; plus 2 or 3 (e or f)
	 */
	/* well, this turns on the 1 MB, but we might as well enable APIC 
	 * access too
	 */

        struct pci_dev *pcidev;

	printk_info( "Enabling extended BIOS access...");

        pcidev = pci_find_device(0x8086, 0x7110, (void *)NULL);
        if (pcidev) pci_write_config_word(pcidev, 0x4e, 0x03c3);

	printk_info("done.\n");
	post_code(0x91);
}

// Have to talk to Eric Beiderman about this ...
// for now, I am putting in the old keyboard code, until we figure out 
// the best way to do this -- RGM

#ifdef NO_KEYBOARD
void keyboard_on()
{
	post_code(0x94);
}
#else
void keyboard_on()
{
        u32 controlbits;
        struct pci_dev *pcidev;
        volatile unsigned char regval;
	unsigned short devfn;
#define KBC_EN_DEV11 0x10000000
#define KBC_EIO_EN   0x40000000
        pcidev = pci_find_device(0x8086, 0x7110, (void *)NULL);

	if (! pcidev) {
	  printk_err( __FUNCTION__ "Can't find dev 0x7110\n");
	  return;
	}
        /* oh no, we are setting this below. Fix it later. */
        /* to do -- changed these to PciReadByte */
	pci_read_config_byte(pcidev, 0x4e, &regval);
	printk_debug( __FUNCTION__ "regcal at 0x4e is 0x%x\n", regval);
        regval |= 0x2;
	pci_write_config_byte(pcidev, 0x4e, regval);

	/* this is a hole in the linux pci function design. You get devfn 0,
	 * but you can't select functions 1-3 using the pci_find_device!
	 */
	devfn = PCI_DEVFN(0x12, 3);
        /* gosh, the PIIX4E is such a special chip! Not in any good sense! */
        /* we have to set two other bits to make the keyboard work. */
        /* this is related to power management, I guess. */
        /* enable the enables -- I'm not kidding, that's what this is. */
        pcibios_read_config_dword(0, devfn, 0x5c, &controlbits);
        controlbits |= KBC_EN_DEV11;
	pcibios_write_config_dword(0, devfn, 0x5c, controlbits);

        /* now enable actual keyboard IO */
        pcibios_read_config_dword(0, devfn, 0x60, &controlbits);
        controlbits |= KBC_EIO_EN;
        pcibios_write_config_dword(0, devfn, 0x60, controlbits);

        /* now keyboard should work, ha ha. */
        pc_keyboard_init();
	post_code(0x94);
}
#endif
