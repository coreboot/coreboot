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
	    pci_write_config_byte(dev1, 0x04, 0x07);
	}

	/* enable USB2 */
	if(dev2) {
	    pci_write_config_byte(dev2, 0x04, 0x07);
	}

	/* enable USB3 */
	if(dev3) {
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
	enables = pci_read_config_byte(dev0, 0x6C, &enables);
	enables |= 0x80;
	pci_write_config_byte(dev0, 0x6C, enables);

	/* follow award */
	enables = pci_read_config_byte(dev0, 0x6C, &enables);
	enables = 0x00;
	pci_write_config_byte(dev0, 0x6C, enables);

	/* Map 4MB of FLASH into the address space */
	pci_write_config_byte(dev0, 0x41, 0x7f);

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

	/* Set 0x58 to 0x03 to match Award */
	pci_write_config_byte(dev0, 0x58, 0x03);

	/* Set bit 3 of 0x4f to match award (use INIT# as cpu reset) */
	enables = pci_read_config_byte(dev0, 0x4f, &enables);
	enables |= 0x08;
	pci_write_config_byte(dev0, 0x4f, enables);

#ifndef DISABLE_SOUTHBRIDGE_COM_PORTS
	/* enable com1 and com2. */
	enables = pci_read_config_byte(dev0, 0x6e, &enables);
	/* 
	 * 0x80 is enable com port b, 0x10 is to make it com2, 0x8 is enable 
	 * com port a as com1
	 */
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

	/* enable serial irq */
	pci_write_config_byte(dev0, 0x52, 0x09);

	/* dma */
	pci_write_config_byte(dev0, 0x53, 0x00);

#if (!ENABLE_IDE_NATIVE_MODE)
	/* Use compatability mode - per award bios */
	pci_write_config_dword(dev1, 0x10, 0x0);
	pci_write_config_dword(dev1, 0x14, 0x0);
	pci_write_config_dword(dev1, 0x18, 0x0);
	pci_write_config_dword(dev1, 0x1c, 0x0);
#endif

	usb_on();

#if 0
	/* Start the rtc */
	rtc_init(0);
#endif
}
