#include <pci.h>
#include <pc80/keyboard.h>
#include <printk.h>
#include <subr.h>

void
southbridge_fixup()
{

	struct pci_dev *pm_pcidev;
	unsigned smbus_io, pm_io;
#if (CONFIG_LINUXBIOS_ENABLE_IDE == 1)
        struct pci_dev *pcidev;
        struct pci_dev *pcidevdebug;
        int i;

        printk_info( "Enabling IDE...");

        pcidev = pci_find_device(0x8086, 0x7111, (void *)NULL);

	if (! pcidev) {
		printk_err("Can't find piix4e\n");
	} else {
		unsigned short c;

		pci_read_config_word(pcidev, 0x40, &c);
		c |= 0x8000;
		printk_info( "0x40 = 0x%04x\n", c);
		pci_write_config_word(pcidev, 0x40, c);
		pci_read_config_word(pcidev, 0x42, &c);
		c |= 0x8000;
		printk_info( "0x42 = 0x%04x\n", c);
		pci_write_config_word(pcidev, 0x42, c);
		printk_info("Enabled IDE for channels 1 and 2\n");
#if (CONFIG_LINUXBIOS_LEGACY_IDE == 1)
		printk_info("Enabling Legacy IDE\n");
		pci_read_config_word(pcidev, 4, &c);
		c |= 1;
		pci_write_config_word(pcidev, 4, c);
		pci_read_config_word(pcidev, 4, c);
		printk_info("Word at 4 is now 0x%04x\n", c);
#endif
	}
#endif
	pm_pcidev = pci_find_device(0x8086, 0x7113, 0);
	if (! pm_pcidev) {
		printk_err("Can't find piix4e PM\n");
	} else {

	  printk_debug("enabling smbus\n");
#if 0
	  smbus_io = NewPciIo(0x10);
#else
	  smbus_io = 0xFFF0;
#endif
	  pci_write_config_dword(pm_pcidev, 0x90, smbus_io | 1); /* iobase addr */
	  pci_write_config_byte(pm_pcidev, 0xd2,  (0x4 << 1) | 1); /* smbus enable */
	  pci_write_config_word(pm_pcidev, 0x4, 1); /* iospace enable */


	  printk_debug("enable pm functions\n");
#if 0
	  pm_io = NewPciIo(0x40);
#else
	  pm_io = 0xFF80;
#endif
	  pci_write_config_dword(pm_pcidev, 0x40, pm_io | 1); /* iobase addr */
	  pci_write_config_byte(pm_pcidev, 0x80, 1);  /* enable pm io address */
	}

#if CONFIG_SETUP_RTC==1
	printk_info( "Setting up RTC\n");
        rtc_init(0);
#endif

        printk_info("done.\n");
}

// Disables the secondary ide interface.  
// This tri-states the signals on the seconday ide
// interface.  Should free up the irq as well.
void disable_secondary_ide(void) 
{
        struct pci_dev *pcidev;
        volatile unsigned char regval;

	printk_info( "Disableing secondary ide controller\n");

        pcidev = pci_find_device(0x8086, 0x7110, (void *)NULL);
      
	pci_read_config_byte(pcidev, 0xb1, &regval);
        regval &= 0xef;
	pci_write_config_byte(pcidev, 0xb1, regval);

}

/*
 This functions is now very misnamed as it deals with much more
 than just the nvram
*/
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
        volatile unsigned char regval;

	printk_info( "Enabling extended BIOS access\n");

        pcidev = pci_find_device(0x8086, 0x7110, (void *)NULL);

	// Need to enable the full ISA bus rather than EIO which
	// is enabled default

	printk_info( "Enabling Full ISA Mode\n");
	pci_read_config_byte(pcidev, 0xb0, &regval);
        regval |= 0x01;
	pci_write_config_byte(pcidev, 0xb0, regval);

	// RAS 6/24/03
	// Datasheet says if you enable the APIC then IRQ8 must
	// not be setup as a GPI. Default is GPI so set this
	// before eaabling the APIC

	printk_info( "Enabling IRQ8\n");

	pci_read_config_byte(pcidev, 0xb1, &regval);
        regval |= 0x40;
	pci_write_config_byte(pcidev, 0xb1, regval);


#if CONFIG_ENABLE_MOUSE_IRQ12==0
	printk_info( "Disableing Mouse IRQ12 on piix4e\n");
        if (pcidev) pci_write_config_word(pcidev, 0x4e, 0x03e1);
#else
	printk_info( "Enabling Mouse IRQ12 on piix4e\n");
        if (pcidev) pci_write_config_word(pcidev, 0x4e, 0x03f1);
#endif

	printk_info("done.\n");
	post_code(0x91);
}

// Have to talk to Eric Beiderman about this ...
// for now, I am putting in the old keyboard code, until we figure out
// the best way to do this -- RGM

#if (NO_KEYBOARD==1)
void keyboard_on()
{
	printk_debug( __FUNCTION__ ": Skipping Keyboard\n");
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
	  printk_err( __FUNCTION__ ": Can't find dev 0x7110\n");
	  return;
	}

	// RAS 6/24/03
	// If your super IO dosen't have IO decodeing of ports 60 and 64
	// builtin then you need to enable this.  Default is off since most
	// superio don't need the extra help.

#if CONFIG_ENABLE_KBCCS == 1
	pci_read_config_byte(pcidev, 0x4e, &regval);
	printk_debug( __FUNCTION__ ": regcal at 0x4e is 0x%x\n", regval);
        regval |= 0x2;
	pci_write_config_byte(pcidev, 0x4e, regval);
#endif

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

	// RAS 6/24/03
        // Me thinks that calling the keyboard init stuff here is incorrect
	// since you may not have the keyboard _realley_ enabled yet if its in 
	// a superio.  I've left it up to the superio code to call it
	// but perhaps it should be a mainboard config item?

//       pc_keyboard_init();

	post_code(0x94);
}
#endif
