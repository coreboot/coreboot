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

	/* Base 8235 controller */
	struct pci_dev *dev0 = pci_find_device(PCI_VENDOR_ID_VIA, \
						0x3177, 0);
	/* USB controller 1 */
	struct pci_dev *dev1 = pci_find_device(PCI_VENDOR_ID_VIA, \
						0x3038, 0);
	/* USB controller 2 */
	struct pci_dev *dev2 = pci_find_device(PCI_VENDOR_ID_VIA, \
						0x3038, \
						dev1);
	/* USB controller 3 */
	struct pci_dev *dev3 = pci_find_device(PCI_VENDOR_ID_VIA, \
						0x3038, \
						dev2);
#if ENABLE_VT8235_USB
        if(dev0) {
	    pci_read_config_byte(dev0, 0x50, &regval);
	    regval &= ~(0x36);
	    pci_write_config_byte(dev0, 0x50, regval);
	}

	/* enable USB1 */
	if(dev1) {
	    pci_write_config_byte(dev1, 0x3c, 0x05);
	    pci_write_config_byte(dev1, 0x04, 0x07);
	}

	/* enable USB2 */
	if(dev2) {
	    pci_write_config_byte(dev2, 0x3c, 0x05);
	    pci_write_config_byte(dev2, 0x04, 0x07);
	}

	/* enable USB3 */
	if(dev3) {
	    pci_write_config_byte(dev3, 0x3c, 0x05);
	    pci_write_config_byte(dev3, 0x04, 0x07);
	}

#else
        if(dev0) {
	    pci_read_config_byte(dev0, 0x50, &regval);
	    regval |= 0x36;    
	    pci_write_config_byte(dev0, 0x50, regval);
	}

	/* disable USB1 */
	if(dev1) {
	    pci_write_config_byte(dev1, 0x3c, 0x00);
	    pci_write_config_byte(dev1, 0x04, 0x00);
	}

	/* disable USB2 */
	if(dev2) {
	    pci_write_config_byte(dev2, 0x3c, 0x00);
	    pci_write_config_byte(dev2, 0x04, 0x00);
	}

	/* disable USB3 */
	if(dev3) {
	    pci_write_config_byte(dev3, 0x3c, 0x00);
	    pci_write_config_byte(dev3, 0x04, 0x00);
	}
#endif
}

void keyboard_on()
{
	unsigned char regval;

	/* Base 8235 controller */
	struct pci_dev *dev0 = pci_find_device(PCI_VENDOR_ID_VIA, \
						0x3177, 0);

	if (dev0) {
		pci_read_config_byte(dev0, 0x51, &regval);
//		regval |= 0x0f; 
		/* !!!FIX let's try this */
		regval |= 0x1d; 
		pci_write_config_byte(dev0, 0x51, regval);
	}
	pc_keyboard_init();

}

void nvram_on()
{
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

	edev = pci_find_device(PCI_VENDOR_ID_VIA, 0x3065, 0);
	if (edev != NULL) {
		printk_debug("Configuring VIA LAN\n");
#if 0
		/* We don't need stepping - though the device supports it */
		pci_read_config_byte(edev, PCI_COMMAND, &byte);
		byte &= ~PCI_COMMAND_WAIT;
		pci_write_config_byte(edev, PCI_COMMAND, byte);

		/* turn on master and pio */
		pci_read_config_byte(edev, PCI_COMMAND, &byte);
		byte = byte | PCI_COMMAND_MASTER|PCI_COMMAND_IO;
		pci_write_config_byte(edev, PCI_COMMAND, byte);
#endif
	} else {
		printk_debug("VIA LAN not found\n");
	}
}


#if 1
void southbridge_fixup()
{
	unsigned char enables;
	struct pci_dev *dev0;
	struct pci_dev *dev1;
	struct pci_dev *devpwr;
#if 0
	/* Base 8235 controller */
	dev0 = pci_find_device(PCI_VENDOR_ID_VIA, 0x3177, 0);
	/* IDE controller */
	dev1 = pci_find_device(PCI_VENDOR_ID_VIA, 0x0571, 0);

	/* Map 4MB of FLASH into the address space */
	pci_write_config_byte(dev0, 0x41, 0x7f);

	/* these follows award's values */
	pci_write_config_byte(dev0, 0x40, 0x45);
	pci_write_config_byte(dev0, 0x42, 0xf0);
	pci_write_config_byte(dev0, 0x4e, 0x0a);
	pci_write_config_byte(dev0, 0x4f, 0x08);
	pci_write_config_byte(dev0, 0x50, 0x81);
	pci_write_config_byte(dev0, 0x51, 0x1d);
//	pci_write_config_byte(dev0, 0x52, 0x09);
	pci_write_config_byte(dev0, 0x53, 0x00);
	pci_write_config_byte(dev0, 0x58, 0x20);
	pci_write_config_byte(dev0, 0x5b, 0x00);

	/* ide */
	/* these follows award's values */
	pci_write_config_byte(dev1, 0x40, 0x03);
	pci_write_config_byte(dev1, 0x41, 0xf2);
	pci_write_config_byte(dev1, 0x42, 0x09);
	pci_write_config_byte(dev1, 0x43, 0x05);
	pci_write_config_byte(dev1, 0x44, 0x18);
	pci_write_config_byte(dev1, 0x45, 0x1c);
	pci_write_config_byte(dev1, 0x4b, 0xa8);
	pci_write_config_byte(dev1, 0x4c, 0x3f);
	pci_write_config_byte(dev1, 0x4f, 0x20);

	pci_write_config_byte(dev1, 0x50, 0x17);
	pci_write_config_byte(dev1, 0x51, 0x17);
	pci_write_config_byte(dev1, 0x52, 0x17);
	pci_write_config_byte(dev1, 0x53, 0xe2);
	pci_write_config_byte(dev1, 0x54, 0x0c);
	pci_write_config_byte(dev1, 0x55, 0x03);

	/* Use compatability mode - per award bios */
	pci_write_config_dword(dev1, 0x10, 0x0);
	pci_write_config_dword(dev1, 0x14, 0x0);
	pci_write_config_dword(dev1, 0x18, 0x0);
	pci_write_config_dword(dev1, 0x1c, 0x0);

	ethernet_fixup();

	usb_on();
#endif
}

#else
void southbridge_fixup()
{
	unsigned char enables;
	struct pci_dev *dev0;
	struct pci_dev *dev1;
	struct pci_dev *devpwr;

	/* Base 8235 controller */
	dev0 = pci_find_device(PCI_VENDOR_ID_VIA, 0x3177, 0);
	/* IDE controller */
	dev1 = pci_find_device(PCI_VENDOR_ID_VIA, 0x0571, 0);

	/* enable the internal I/O decode */
#if 0
	enables = pci_read_config_byte(dev0, 0x6C, &enables);
	enables |= 0x80;
	pci_write_config_byte(dev0, 0x6C, enables);
#endif
	/* follow award */
	enables = pci_read_config_byte(dev0, 0x6C, &enables);
	enables = 0x00;
	pci_write_config_byte(dev0, 0x6C, enables);

	/* Map 4MB of FLASH into the address space */
	pci_write_config_byte(dev0, 0x41, 0x7f);
	/*  let's try 0x0 to see what's happen */
//	pci_write_config_byte(dev0, 0x41, 0x00);

	/*
	 * Set bit 6 of 0x40, because Award does it (IO recovery time)
	 * IMPORTANT FIX - EISA 0x4d0 decoding must be on so that PCI 
	 * interrupts can be properly marked as level triggered.
	 */
	enables = pci_read_config_byte(dev0, 0x40, &enables);
	enables |= 0x45;
	pci_write_config_byte(dev0, 0x40, enables);

	/* Set 0x42 to 0xf0 to match Award bios */
	enables = pci_read_config_byte(dev0, 0x42, &enables);
	enables |= 0xf0;
	pci_write_config_byte(dev0, 0x42, enables);

	/* Set bit 3 of 0x4a, to match award (dummy pci request) */
//	enables = pci_read_config_byte(dev0, 0x4a, &enables);
//	enables |= 0x08;
//	pci_write_config_byte(dev0, 0x4a, enables);

	/* Set bit 3 of 0x4f to match award (use INIT# as cpu reset) */
	enables = pci_read_config_byte(dev0, 0x4f, &enables);
	enables |= 0x08;
	pci_write_config_byte(dev0, 0x4f, enables);

	/* Set 0x58 to 0x03 to match Award */
	pci_write_config_byte(dev0, 0x58, 0x03);
#if 0
	/* enable the ethernet/RTC */
	if(dev0) {
		pci_read_config_byte(dev0, 0x51, &enables);
		enables |= 0x18; 
		pci_write_config_byte(dev0, 0x51, enables);
	}
#endif	
#ifndef DISABLE_SOUTHBRIDGE_COM_PORTS
	/* enable com1 and com2. */
	enables = pci_read_config_byte(dev0, 0x6e, &enables);
	/* 
	 * 0x80 is enable com port b, 0x10 is to make it com2, 0x8 is enable 
	 * com port a as com1
	 */
#if 0
	enables = 0x80 | 0x10 | 0x8 ;
	pci_write_config_byte(dev0, 0x6e, enables);
#endif
	/* following award */
	enables = 0x00;
	pci_write_config_byte(dev0, 0x6e, enables);
	/* 
	 * note: this is also a redo of some port of assembly, but we want 
	 * everything up. 
	 * set com1 to 115 kbaud
	 * not clear how to do this yet. 
	 * forget it; done in assembly.
	 */
#endif

	/*
	 * enable IDE, since Linux won't do it.
	 * First do some more things to devfn (17,0)
	 * note: this should already be cleared, according to the book. 
	 */
	pci_read_config_byte(dev0, 0x50, &enables);
	printk_debug("IDE enable in reg. 50 is 0x%x\n", enables);
	enables &= ~8; // need manifest constant here!
	printk_debug("set IDE reg. 50 to 0x%x\n", enables);
	pci_write_config_byte(dev0, 0x50, enables);

	/* moved it to setupide.inc */
#if 0
	/* set default interrupt values (IDE) */
	pci_read_config_byte(dev0, 0x4c, &enables);
	printk_debug("IRQs in reg. 4c are 0x%x\n", enables & 0xf);
	/* clear out whatever was there. */
	enables &= ~0xf;
	enables = 44;
	printk_debug("setting reg. 4c to 0x%x\n", enables);
	pci_write_config_byte(dev0, 0x4c, enables);
#endif
	
#if 1
	/* enable serial irq */
	pci_write_config_byte(dev0, 0x52, 0x09);

	/* dma */
	pci_write_config_byte(dev0, 0x53, 0x00);

	/* diskable dynamic clock stop */
	pci_write_config_byte(dev0, 0x5b, 0x00);

//	pci_write_config_byte(dev0, 0x6c, 0x00);
//	pci_write_config_byte(dev0, 0x6e, 0x00);

	/*
	 * set up the serial port interrupts. 
	 * com2 to 3, com1 to 4
	 */
//	pci_write_config_byte(dev0, 0x46, 0x04);
//        pci_write_config_byte(dev0, 0x47, 0x03);
#endif
#if 1
	/*
	 * IDE setup
	 */
#if !(ENABLE_IDE_NATIVE_MODE)
	/*
	 * Run the IDE controller in 'compatiblity mode - i.e. don't use PCI
	 * interrupts.  Using PCI ints confuses linux for some reason.
	 */

	pci_read_config_byte(dev1, 0x42, &enables);
	printk_debug("enables in reg 0x42 0x%x\n", enables);
	enables &= ~0xc0;
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

	/* Enable prefetch buffers */
	pci_read_config_byte(dev1, 0x41, &enables);
	enables |= 0xf0;
	pci_write_config_byte(dev1, 0x41, enables);

	/* Lower thresholds (cause award does it) */
	pci_read_config_byte(dev1, 0x43, &enables);
	enables &= ~0x0f;
	enables |=  0x05;
	pci_write_config_byte(dev1, 0x43, enables);

	/* PIO read prefetch counter (cause award does it) */
	pci_write_config_byte(dev1, 0x44, 0x18);

	/* Use memory read multiple */
	pci_write_config_byte(dev1, 0x45, 0x1c);

	/* address decoding. */
        pci_read_config_byte(dev1, 0x9, &enables);
        printk_debug("enables in reg 0x9 0x%x\n", enables);
	/* by the book, set the low-order nibble to 0xa. */
#if ENABLE_IDE_NATIVE_MODE
	enables &= ~0xf;
	/* cf/cg silicon needs an 'f' here. */
        enables |= 0xf;
#else
	enables &= ~0x5;
#endif

        pci_write_config_byte(dev1, 0x9, enables);
        pci_read_config_byte(dev1, 0x9, &enables);
        printk_debug("enables in reg 0x9 read back as 0x%x\n", enables);

	/* standard bios sets master bit. */
	pci_read_config_byte(dev1, 0x4, &enables);
	printk_debug("command in reg 0x4 0x%x\n", enables);
	enables |= 5;
#if 1	
	/* No need for stepping - kevinh@ispiri.com */
	enables &= ~0x80;
#endif
	pci_write_config_byte(dev1, 0x4, enables);
	pci_read_config_byte(dev1, 0x4, &enables);
	printk_debug("command in reg 0x4 reads back as 0x%x\n", enables);

#if (!ENABLE_IDE_NATIVE_MODE)
	/* Use compatability mode - per award bios */
	pci_write_config_dword(dev1, 0x10, 0x0);
	pci_write_config_dword(dev1, 0x14, 0x0);
	pci_write_config_dword(dev1, 0x18, 0x0);
	pci_write_config_dword(dev1, 0x1c, 0x0);

	/* Force interrupts to use compat mode - just like Award bios */
	/* !!! FIX moved it to ide_config.inc */
#if 0
	pci_write_config_byte(dev1, 0x3d, 0x0);
	pci_write_config_byte(dev1, 0x3c, 0xff);
#endif
#endif
#endif

	ethernet_fixup();

	usb_on();

#if 0
	/* Start the rtc */
	rtc_init(0);
#endif
}
#endif

#if 0
	pci_write_config_byte(dev1, 0x40, 0x0b);
	pci_write_config_byte(dev1, 0x41, 0xf2);
	pci_write_config_byte(dev1, 0x42, 0x09);
	pci_write_config_byte(dev1, 0x43, 0x05);
	pci_write_config_byte(dev1, 0x44, 0x18);
	pci_write_config_byte(dev1, 0x45, 0x1c);
	pci_write_config_byte(dev1, 0x4b, 0xa8);
	pci_write_config_byte(dev1, 0x4c, 0x3f);
	pci_write_config_byte(dev1, 0x4f, 0x20);

	pci_write_config_byte(dev1, 0x50, 0x17);
	pci_write_config_byte(dev1, 0x51, 0x17);
	pci_write_config_byte(dev1, 0x52, 0x17);
	pci_write_config_byte(dev1, 0x53, 0xe2);
	pci_write_config_byte(dev1, 0x54, 0x0c);
	pci_write_config_byte(dev1, 0x55, 0x03);


	pci_write_config_byte(dev0, 0x6C, 0x00);
	pci_write_config_byte(dev0, 0x41, 0x7f);

	enables = pci_read_config_byte(dev0, 0x40, &enables);
	enables |= 0x45;
	pci_write_config_byte(dev0, 0x40, enables);

	enables = pci_read_config_byte(dev0, 0x42, &enables);
	enables |= 0xf0;
	pci_write_config_byte(dev0, 0x42, enables);

	enables = pci_read_config_byte(dev0, 0x4f, &enables);
	enables |= 0x08;
	pci_write_config_byte(dev0, 0x4f, enables);

	pci_write_config_byte(dev0, 0x58, 0x03);

	enables = pci_read_config_byte(dev0, 0x6e, &enables);
	enables = 0x00;
	pci_write_config_byte(dev0, 0x6e, enables);

	pci_read_config_byte(dev0, 0x50, &enables);
	enables &= ~8; // need manifest constant here!
	pci_write_config_byte(dev0, 0x50, enables);

	pci_write_config_byte(dev0, 0x52, 0x09);
	pci_write_config_byte(dev0, 0x53, 0x00);
	pci_write_config_byte(dev0, 0x5b, 0x00);

	pci_read_config_byte(dev1, 0x42, &enables);
	enables &= ~0xc0;
	pci_write_config_byte(dev1, 0x42, enables);

	pci_read_config_byte(dev1, 0x40, &enables);
	enables |= 3;
	pci_write_config_byte(dev1, 0x40, enables);

	pci_read_config_byte(dev1, 0x41, &enables);
	enables |= 0xf0;
	pci_write_config_byte(dev1, 0x41, enables);

	pci_read_config_byte(dev1, 0x43, &enables);
	enables &= ~0x0f;
	enables |=  0x05;
	pci_write_config_byte(dev1, 0x43, enables);

	pci_write_config_byte(dev1, 0x44, 0x18);

	pci_write_config_byte(dev1, 0x45, 0x1c);

        pci_read_config_byte(dev1, 0x9, &enables);
	enables &= ~0x5;
        pci_write_config_byte(dev1, 0x9, enables);

	pci_read_config_byte(dev1, 0x4, &enables);
	printk_debug("command in reg 0x4 0x%x\n", enables);
	enables |= 5;
	enables &= ~0x80;

	pci_write_config_byte(dev1, 0x4, enables);

	pci_write_config_dword(dev1, 0x10, 0x0);
	pci_write_config_dword(dev1, 0x14, 0x0);
	pci_write_config_dword(dev1, 0x18, 0x0);
	pci_write_config_dword(dev1, 0x1c, 0x0);
#endif
