#include <pci.h>
#include <pc80/keyboard.h>
#include <pc80/mc146818rtc.h>
#include <printk.h>
#include <pci_ids.h>
#include <arch/io.h>


// #define IDE_NATIVE_MODE 1

void usb_on()
{
	unsigned char regval;

	/* Base 8231 controller */
	struct pci_dev *dev0 = pci_find_device(PCI_VENDOR_ID_VIA, \
						PCI_DEVICE_ID_VIA_8231, 0);
	/* USB controller 1 */
	struct pci_dev *dev2 = pci_find_device(PCI_VENDOR_ID_VIA, \
						PCI_DEVICE_ID_VIA_82C586_2, 0);
	/* USB controller 2 */
	struct pci_dev *dev3 = pci_find_device(PCI_VENDOR_ID_VIA, \
						PCI_DEVICE_ID_VIA_82C586_2, \
						dev2);
#if ENABLE_VT8231_USB
	/* enable USB1 */
	if(dev2) {
	    pci_write_config_byte(dev2, 0x3c, 0x05);
	    pci_write_config_byte(dev2, 0x04, 0x07);
	}

        if(dev0) {
	    pci_read_config_byte(dev0, 0x50, &regval);
	    regval &= ~(0x10);    
	    pci_write_config_byte(dev0, 0x50, regval);
	}

	/* enable USB2 */
	if(dev3) {
	    pci_write_config_byte(dev3, 0x3c, 0x05);
	    pci_write_config_byte(dev3, 0x04, 0x07);
	}

        if(dev0) {
	    pci_read_config_byte(dev0, 0x50, &regval);
	    regval &= ~(0x20);    
	    pci_write_config_byte(dev0, 0x50, regval);
	}
#else
	/* disable USB1 */
	if(dev2) {
	    pci_write_config_byte(dev2, 0x3c, 0x00);
	    pci_write_config_byte(dev2, 0x04, 0x00);
	}

        if(dev0) {
	    pci_read_config_byte(dev0, 0x50, &regval);
	    regval |= 0x10;    
	    pci_write_config_byte(dev0, 0x50, regval);
	}

	/* disable USB2 */
	if(dev3) {
	    pci_write_config_byte(dev3, 0x3c, 0x00);
	    pci_write_config_byte(dev3, 0x04, 0x00);
	}

        if(dev0) {
	    pci_read_config_byte(dev0, 0x50, &regval);
	    regval |= 0x20;    
	    pci_write_config_byte(dev0, 0x50, regval);
	}
#endif
}

void keyboard_on()
{
	unsigned char regval;

	/* Base 8231 controller */
	struct pci_dev *dev0 = pci_find_device(PCI_VENDOR_ID_VIA, \
						PCI_DEVICE_ID_VIA_8231, 0);

	/* kevinh/Ispiri - update entire function to use 
	   new pci_write_config_byte */

	if (dev0) {
	    pci_read_config_byte(dev0, 0x51, &regval);
	    regval |= 0x0f; 
	    pci_write_config_byte(dev0, 0x51, regval);
	}
	pc_keyboard_init();

}

void nvram_on()
{
	/*
	 * the VIA 8231 South has a very different nvram setup than the 
	 * piix4e ...
	 * turn on ProMedia nvram.
	 * TO DO: use the PciWriteByte function here.
	 */

	/*
	 * kevinh/Ispiri - I don't think this is the correct address/value
	 * intel_conf_writeb(0x80008841, 0xFF);
	 */
}


/*
 * Enable the ethernet device and turn off stepping (because it is integrated 
 * inside the southbridge)
 */
void ethernet_fixup()
{
	struct pci_dev	*dev, *edev;
	u8		byte;

	printk_info("Ethernet fixup\n");

	edev = pci_find_device(PCI_VENDOR_ID_VIA, PCI_DEVICE_ID_VIA_8233_7, 0);
	if (edev != NULL) {
		printk_debug("Configuring VIA LAN\n");

		/* We don't need stepping - though the device supports it */
		pci_read_config_byte(edev, PCI_COMMAND, &byte);
		byte &= ~PCI_COMMAND_WAIT;
		pci_write_config_byte(edev, PCI_COMMAND, byte);
	} else {
		printk_debug("VIA LAN not found\n");
	}
}


void southbridge_fixup()
{
	unsigned char enables;
	struct pci_dev *dev0;
	struct pci_dev *dev1;
	struct pci_dev *devpwr;

	// to do: use the pcibios_find function here, instead of 
	// hard coding the devfn. 
	// done - kevinh/Ispiri

	/* Base 8231 controller */
	dev0 = pci_find_device(PCI_VENDOR_ID_VIA, PCI_DEVICE_ID_VIA_8231, 0);
	/* IDE controller */
	dev1 = pci_find_device(PCI_VENDOR_ID_VIA, PCI_DEVICE_ID_VIA_82C586_1, \
					0);
	/* Power management controller */
	 devpwr = pci_find_device(PCI_VENDOR_ID_VIA, \
			 		PCI_DEVICE_ID_VIA_8231_4, 0);

	// enable the internal I/O decode
	enables = pci_read_config_byte(dev0, 0x6C, &enables);
	enables |= 0x80;
	pci_write_config_byte(dev0, 0x6C, enables);

	// Map 4MB of FLASH into the address space
	pci_write_config_byte(dev0, 0x41, 0x7f);

	// Set bit 6 of 0x40, because Award does it (IO recovery time)
	// IMPORTANT FIX - EISA 0x4d0 decoding must be on so that PCI 
	// interrupts can be properly marked as level triggered.
	enables = pci_read_config_byte(dev0, 0x40, &enables); enables |= 0x44;
	pci_write_config_byte(dev0, 0x40, enables);

	// Set 0x42 to 0xf0 to match Award bios
	enables = pci_read_config_byte(dev0, 0x42, &enables);
	enables |= 0xf0;
	pci_write_config_byte(dev0, 0x42, enables);

	// Set bit 3 of 0x4a, to match award (dummy pci request)
	enables = pci_read_config_byte(dev0, 0x4a, &enables);
	enables |= 0x08;
	pci_write_config_byte(dev0, 0x4a, enables);

	// Set bit 3 of 0x4f to match award (use INIT# as cpu reset)
	enables = pci_read_config_byte(dev0, 0x4f, &enables);
	enables |= 0x08;
	pci_write_config_byte(dev0, 0x4f, enables);

	// Set 0x58 to 0x03 to match Award
	pci_write_config_byte(dev0, 0x58, 0x03);

	// enable the ethernet/RTC
	if(dev0) {
	    pci_read_config_byte(dev0, 0x51, &enables);
	    enables |= 0x18; 
	    pci_write_config_byte(dev0, 0x51, enables);
	}
	
#ifndef DISABLE_SOUTHBRIDGE_COM_PORTS
	// enable com1 and com2. 
	enables = pci_read_config_byte(dev0, 0x6e, &enables);
	// 0x80 is enable com port b, 0x10 is to make it com2, 0x8 is enable com port a as com1
	// kevinh/Ispiri - Old code thought 0x01 would make it com1, that was
	// wrong
	enables = 0x80 | 0x10 | 0x8 ;
	pci_write_config_byte(dev0, 0x6e, enables);
	// note: this is also a redo of some port of assembly, but we want everything up. 

	// set com1 to 115 kbaud
	// not clear how to do this yet. 
	// forget it; done in assembly. 
#endif

	// enable IDE, since Linux won't do it.
	// First do some more things to devfn (17,0)
	// note: this should already be cleared, according to the book. 
	pci_read_config_byte(dev0, 0x50, &enables);
	printk_debug("IDE enable in reg. 50 is 0x%x\n", enables);
	enables &= ~8; // need manifest constant here!
	printk_debug("set IDE reg. 50 to 0x%x\n", enables);
	pci_write_config_byte(dev0, 0x50, enables);

	// set default interrupt values (IDE)
	pci_read_config_byte(dev0, 0x4c, &enables);
	printk_debug("IRQs in reg. 4c are 0x%x\n", enables & 0xf);
	// clear out whatever was there. 
	enables &= ~0xf;
	enables |= 4;
	printk_debug("setting reg. 4c to 0x%x\n", enables);
	pci_write_config_byte(dev0, 0x4c, enables);
	
	// set up the serial port interrupts. 
	// com2 to 3, com1 to 4
	pci_write_config_byte(dev0, 0x46, 0x04);
        pci_write_config_byte(dev0, 0x47, 0x03);

	//
	// Power management setup
	//
	// Set ACPI base address to IO 0x4000
	pci_write_config_dword(devpwr, 0x48, 0x4001);

	// Enable ACPI access (and setup like award)
	pci_write_config_byte(devpwr, 0x41, 0x84);

	// Set hardware monitor base address to IO 0x6000
	pci_write_config_dword(devpwr, 0x70, 0x6001);

	// Enable hardware monitor (and setup like award)
	pci_write_config_byte(devpwr, 0x74, 0x01);

	// set IO base address to 0x5000
	pci_write_config_dword(devpwr, 0x90, 0x5001);

	// Enable SMBus 
	pci_write_config_byte(devpwr, 0xd2, 0x01);

	//
	// IDE setup
	//
#ifndef ENABLE_IDE_NATIVE_MODE
	// Run the IDE controller in 'compatiblity mode - i.e. don't use PCI
	// interrupts.  Using PCI ints confuses linux for some reason.

	pci_read_config_byte(dev1, 0x42, &enables);
	printk_debug("enables in reg 0x42 0x%x\n", enables);
	enables &= ~0xc0;		// compatability mode
	pci_write_config_byte(dev1, 0x42, enables);
	pci_read_config_byte(dev1, 0x42, &enables);
	printk_debug("enables in reg 0x42 read back as 0x%x\n", enables);
#endif

	pci_read_config_byte(dev1, 0x40, &enables);
	printk_debug("enables in reg 0x40 0x%x\n", enables);
	enables |= 3;
	pci_write_config_byte(dev1, 0x40, enables);
	pci_read_config_byte(dev1, 0x40, &enables);
	printk_debug("enables in reg 0x40 read back as 0x%x\n", enables);

	// Enable prefetch buffers
	pci_read_config_byte(dev1, 0x41, &enables);
	enables |= 0xf0;
	pci_write_config_byte(dev1, 0x41, enables);

	// Lower thresholds (cause award does it)
	pci_read_config_byte(dev1, 0x43, &enables);
	enables &= ~0x0f;
	enables |=  0x05;
	pci_write_config_byte(dev1, 0x43, enables);

	// PIO read prefetch counter (cause award does it)
	pci_write_config_byte(dev1, 0x44, 0x18);

	// Use memory read multiple
	pci_write_config_byte(dev1, 0x45, 0x1c);

	// address decoding. 
	// we want "flexible", i.e. 1f0-1f7 etc. or native PCI
	// kevinh@ispiri.com - the standard linux drivers seem ass slow when 
	// used in native mode - I've changed back to classic
        pci_read_config_byte(dev1, 0x9, &enables);
        printk_debug("enables in reg 0x9 0x%x\n", enables);
	// by the book, set the low-order nibble to 0xa. 
#if ENABLE_IDE_NATIVE_MODE
	enables &= ~0xf;
	// cf/cg silicon needs an 'f' here. 
        enables |= 0xf;
#else
	enables &= ~0x5;
#endif

        pci_write_config_byte(dev1, 0x9, enables);
        pci_read_config_byte(dev1, 0x9, &enables);
        printk_debug("enables in reg 0x9 read back as 0x%x\n", enables);

	// standard bios sets master bit. 
	pci_read_config_byte(dev1, 0x4, &enables);
	printk_debug("command in reg 0x4 0x%x\n", enables);
	enables |= 7;
	
	// No need for stepping - kevinh@ispiri.com
	enables &= ~0x80;

	pci_write_config_byte(dev1, 0x4, enables);
	pci_read_config_byte(dev1, 0x4, &enables);
	printk_debug("command in reg 0x4 reads back as 0x%x\n", enables);

#if (!ENABLE_IDE_NATIVE_MODE)
	// Use compatability mode - per award bios
	pci_write_config_dword(dev1, 0x10, 0x0);
	pci_write_config_dword(dev1, 0x14, 0x0);
	pci_write_config_dword(dev1, 0x18, 0x0);
	pci_write_config_dword(dev1, 0x1c, 0x0);

	// Force interrupts to use compat mode - just like Award bios
	pci_write_config_byte(dev1, 0x3d, 00);
	pci_write_config_byte(dev1, 0x3c, 0xff);
#endif

	ethernet_fixup();

	// Start the rtc
	rtc_init(0);
}
