
#include <arch/io.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ops.h>
#include <device/pci_ids.h>
#include <console/console.h>
#include "vt8231.h"
#include "chip.h"

void pc_keyboard_init(void);

void hard_reset(void) 
{
	printk_err("NO HARD RESET ON VT8231! FIX ME!\n");
}

static void usb_on(int enable)
{
	unsigned char regval;

	/* Base 8231 controller */
	device_t dev0 = dev_find_device(PCI_VENDOR_ID_VIA, PCI_DEVICE_ID_VIA_8231, 0);
	/* USB controller 1 */
	device_t dev2 = dev_find_device(PCI_VENDOR_ID_VIA, PCI_DEVICE_ID_VIA_82C586_2, 0);
	/* USB controller 2 */
	device_t dev3 = dev_find_device(PCI_VENDOR_ID_VIA, PCI_DEVICE_ID_VIA_82C586_2, dev2);
	
	/* enable USB1 */
	if(dev2) {
		if (enable) {
			pci_write_config8(dev2, 0x3c, 0x05);
			pci_write_config8(dev2, 0x04, 0x07);
		} else {
			pci_write_config8(dev2, 0x3c, 0x00);
			pci_write_config8(dev2, 0x04, 0x00);
		}
	}
	
	if(dev0) {
		regval = pci_read_config8(dev0, 0x50);
		if (enable) 
			regval &= ~(0x10);    
		else
			regval |= 0x10;    	      
		pci_write_config8(dev0, 0x50, regval);
	}
	
	/* enable USB2 */
	if(dev3) {
		if (enable) {
			pci_write_config8(dev3, 0x3c, 0x05);
			pci_write_config8(dev3, 0x04, 0x07);
		} else {
			pci_write_config8(dev3, 0x3c, 0x00);
			pci_write_config8(dev3, 0x04, 0x00);
		}
	}
	
	if(dev0) {
		regval = pci_read_config8(dev0, 0x50);
		if (enable) 
			regval &= ~(0x20);    
		else
			regval |= 0x20;    
		pci_write_config8(dev0, 0x50, regval);
	}
}

static void keyboard_on(void)
{
	unsigned char regval;
	
	/* Base 8231 controller */
	device_t dev0 = dev_find_device(PCI_VENDOR_ID_VIA, PCI_DEVICE_ID_VIA_8231, 0);
	
	/* kevinh/Ispiri - update entire function to use 
	   new pci_write_config8 */

	if (dev0) {
		regval = pci_read_config8(dev0, 0x51);
		regval |= 0x0f; 
		pci_write_config8(dev0, 0x51, regval);
	}
	init_pc_keyboard(0x60, 0x64, 0);
}

static void nvram_on(void)
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
static void ethernet_fixup()
{
	device_t	edev;
	uint8_t		byte;

	printk_info("Ethernet fixup\n");

	edev = dev_find_device(PCI_VENDOR_ID_VIA, PCI_DEVICE_ID_VIA_8233_7, 0);
	if (edev) {
		printk_debug("Configuring VIA LAN\n");
		
		/* We don't need stepping - though the device supports it */
		byte = pci_read_config8(edev, PCI_COMMAND);
		byte &= ~PCI_COMMAND_WAIT;
		pci_write_config8(edev, PCI_COMMAND, byte);
	} else {
		printk_debug("VIA LAN not found\n");
	}
}


/* we need to do things in this function so that PCI scan will find 
 * them.  One problem here is that we can't use ANY of the new device 
 * stuff. This work here precedes all that.      
 * Fundamental problem with linuxbios V2 architecture.
 * You can't do pci control in the C code without having done a PCI scan.
 * But in some cases you need to to pci control in the c code before doing
 * a PCI scan. But you can't use arch/romcc_io.h (the code you need) because
 * that has functions with the same name but different type signatures
 * (e.g. device_t). This needs to get fixed. We need low-level pci scans
 * in the C code. 
 */
static void vt8231_pci_enable(struct southbridge_via_vt8231_config *conf) 
{
	/*
	  unsigned long busdevfn = 0x8000;
	  if (conf->enable_ide) {
	  printk_debug("%s: enabling IDE function\n", __FUNCTION__);
	  }
	*/
}

/* PIRQ init
 */
void pci_assign_irqs(unsigned bus, unsigned slot, const unsigned char pIntAtoD[4]);


static const unsigned char southbridgeIrqs[4] = { 11, 5, 10, 12 };
static const unsigned char enetIrqs[4] = { 11, 5, 10, 12 };
static const unsigned char slotIrqs[4] = { 5, 10, 12, 11 };

/*
	Our IDSEL mappings are as follows
	PCI slot is AD31          (device 15) (00:14.0)
	Southbridge is AD28       (device 12) (00:11.0)
*/
static void pci_routing_fixup(void)
{
	device_t dev;

        dev = dev_find_device(PCI_VENDOR_ID_VIA, PCI_DEVICE_ID_VIA_8231, 0);
	printk_info("%s: dev is %p\n", __FUNCTION__, dev);
	if (dev) {
		/* initialize PCI interupts - these assignments depend
		   on the PCB routing of PINTA-D 

		   PINTA = IRQ11
		   PINTB = IRQ5
		   PINTC = IRQ10
		   PINTD = IRQ12
		*/
		pci_write_config8(dev, 0x55, 0xb0);
		pci_write_config8(dev, 0x56, 0xa5);
		pci_write_config8(dev, 0x57, 0xc0);
	}

	// Standard southbridge components
	printk_info("setting southbridge\n");
	pci_assign_irqs(0, 0x11, southbridgeIrqs);

	// Ethernet built into southbridge
	printk_info("setting ethernet\n");
	pci_assign_irqs(0, 0x12, enetIrqs);

	// PCI slot
	printk_info("setting pci slot\n");
	pci_assign_irqs(0, 0x14, slotIrqs);
	printk_info("%s: DONE\n", __FUNCTION__);
}


void
dump_south(void)
{
	device_t dev0;
	dev0 = dev_find_device(PCI_VENDOR_ID_VIA, PCI_DEVICE_ID_VIA_8231, 0);
	int i,j;
	
	for(i = 0; i < 256; i += 16) {
		printk_debug("0x%x: ", i);
		for(j = 0; j < 16; j++) {
			printk_debug("%02x ", pci_read_config8(dev0, i+j));
		}
		printk_debug("\n");
	}
}

static void vt8231_init(struct southbridge_via_vt8231_config *conf)
{
	unsigned char enables;
	device_t dev0;
	device_t dev1;
	device_t devpwr;
	
	// to do: use the pcibios_find function here, instead of 
	// hard coding the devfn. 
	// done - kevinh/Ispiri
	printk_debug("vt8231 init\n");
	/* Base 8231 controller */
	dev0 = dev_find_device(PCI_VENDOR_ID_VIA, PCI_DEVICE_ID_VIA_8231, 0);
	/* IDE controller */
	dev1 = dev_find_device(PCI_VENDOR_ID_VIA, PCI_DEVICE_ID_VIA_82C586_1, 0);
	/* Power management controller */
	devpwr = dev_find_device(PCI_VENDOR_ID_VIA, PCI_DEVICE_ID_VIA_8231_4, 0);

	// enable the internal I/O decode
	enables = pci_read_config8(dev0, 0x6C);
	enables |= 0x80;
	pci_write_config8(dev0, 0x6C, enables);
	
	// Map 4MB of FLASH into the address space
	pci_write_config8(dev0, 0x41, 0x7f);
	
	// Set bit 6 of 0x40, because Award does it (IO recovery time)
	// IMPORTANT FIX - EISA 0x4d0 decoding must be on so that PCI 
	// interrupts can be properly marked as level triggered.
	enables = pci_read_config8(dev0, 0x40);
	pci_write_config8(dev0, 0x40, enables);
	
	// Set 0x42 to 0xf0 to match Award bios
	enables = pci_read_config8(dev0, 0x42);
	enables |= 0xf0;
	pci_write_config8(dev0, 0x42, enables);
	
	// Set bit 3 of 0x4a, to match award (dummy pci request)
	enables = pci_read_config8(dev0, 0x4a);
	enables |= 0x08;
	pci_write_config8(dev0, 0x4a, enables);
	
	// Set bit 3 of 0x4f to match award (use INIT# as cpu reset)
	enables = pci_read_config8(dev0, 0x4f);
	enables |= 0x08;
	pci_write_config8(dev0, 0x4f, enables);
	
	// Set 0x58 to 0x03 to match Award
	pci_write_config8(dev0, 0x58, 0x03);
	
	// enable the ethernet/RTC
	if(dev0) {
		enables = pci_read_config8(dev0, 0x51);
		enables |= 0x18; 
		pci_write_config8(dev0, 0x51, enables);
	}
	
	
	// enable com1 and com2. 
	if (conf->enable_com_ports) {
		enables = pci_read_config8(dev0, 0x6e);
		
		/* 0x80 is enable com port b, 0x10 is to make it com2, 0x8
		 * is enable com port a as com1 kevinh/Ispiri - Old code
		 * thought 0x01 would make it com1, that was wrong enables =
		 * 0x80 | 0x10 | 0x8 ; pci_write_config8(dev0, 0x6e,
		 * enables); // note: this is also a redo of some port of
		 * assembly, but we want everything up.
		 */
		
		/* set com1 to 115 kbaud not clear how to do this yet.
		 * forget it; done in assembly.
		 */

	}
	// enable IDE, since Linux won't do it.
	// First do some more things to devfn (17,0)
	// note: this should already be cleared, according to the book. 
	enables = pci_read_config8(dev0, 0x50);
	printk_debug("IDE enable in reg. 50 is 0x%x\n", enables);
	enables &= ~8; // need manifest constant here!
	printk_debug("set IDE reg. 50 to 0x%x\n", enables);
	pci_write_config8(dev0, 0x50, enables);
	
	// set default interrupt values (IDE)
	enables = pci_read_config8(dev0, 0x4c);
	printk_debug("IRQs in reg. 4c are 0x%x\n", enables & 0xf);
	// clear out whatever was there. 
	enables &= ~0xf;
	enables |= 4;
	printk_debug("setting reg. 4c to 0x%x\n", enables);
	pci_write_config8(dev0, 0x4c, enables);
	
	// set up the serial port interrupts. 
	// com2 to 3, com1 to 4
	pci_write_config8(dev0, 0x46, 0x04);
	pci_write_config8(dev0, 0x47, 0x03);
	pci_write_config8(dev0, 0x6e, 0x98);
	//
	// Power management setup
	//
	// Set ACPI base address to IO 0x4000
	pci_write_config32(devpwr, 0x48, 0x4001);
	
	// Enable ACPI access (and setup like award)
	pci_write_config8(devpwr, 0x41, 0x84);
	
	// Set hardware monitor base address to IO 0x6000
	pci_write_config32(devpwr, 0x70, 0x6001);
	
	// Enable hardware monitor (and setup like award)
	pci_write_config8(devpwr, 0x74, 0x01);
	
	// set IO base address to 0x5000
	pci_write_config32(devpwr, 0x90, 0x5001);
	
	// Enable SMBus 
	pci_write_config8(devpwr, 0xd2, 0x01);
	
	//
	// IDE setup
	//
	if (! conf->enable_native_ide) {
		// Run the IDE controller in 'compatiblity mode - i.e. don't use PCI
		// interrupts.  Using PCI ints confuses linux for some reason.
		
		printk_info("%s: enabling compatibility IDE addresses\n", __FUNCTION__);
		enables = pci_read_config8(dev1, 0x42);
		printk_debug("enables in reg 0x42 0x%x\n", enables);
		enables &= ~0xc0;		// compatability mode
		pci_write_config8(dev1, 0x42, enables);
		enables = pci_read_config8(dev1, 0x42);
		printk_debug("enables in reg 0x42 read back as 0x%x\n", enables);
	}
	
	enables = pci_read_config8(dev1, 0x40);
	printk_debug("enables in reg 0x40 0x%x\n", enables);
	enables |= 3;
	pci_write_config8(dev1, 0x40, enables);
	enables = pci_read_config8(dev1, 0x40);
	printk_debug("enables in reg 0x40 read back as 0x%x\n", enables);
	
	// Enable prefetch buffers
	enables = pci_read_config8(dev1, 0x41);
	enables |= 0xf0;
	pci_write_config8(dev1, 0x41, enables);
	
	// Lower thresholds (cause award does it)
	enables = pci_read_config8(dev1, 0x43);
	enables &= ~0x0f;
	enables |=  0x05;
	pci_write_config8(dev1, 0x43, enables);
	
	// PIO read prefetch counter (cause award does it)
	pci_write_config8(dev1, 0x44, 0x18);
	
	// Use memory read multiple
	pci_write_config8(dev1, 0x45, 0x1c);
	
	// address decoding. 
	// we want "flexible", i.e. 1f0-1f7 etc. or native PCI
	// kevinh@ispiri.com - the standard linux drivers seem ass slow when 
	// used in native mode - I've changed back to classic
	enables = pci_read_config8(dev1, 0x9);
	printk_debug("enables in reg 0x9 0x%x\n", enables);
	// by the book, set the low-order nibble to 0xa. 
	if (conf->enable_native_ide) {
		enables &= ~0xf;
		// cf/cg silicon needs an 'f' here. 
		enables |= 0xf;
	} else {
		enables &= ~0x5;
	}
	
	pci_write_config8(dev1, 0x9, enables);
	enables = pci_read_config8(dev1, 0x9);
	printk_debug("enables in reg 0x9 read back as 0x%x\n", enables);
	
	// standard bios sets master bit. 
	enables = pci_read_config8(dev1, 0x4);
	printk_debug("command in reg 0x4 0x%x\n", enables);
	enables |= 7;
	
	// No need for stepping - kevinh@ispiri.com
	enables &= ~0x80;
	
	pci_write_config8(dev1, 0x4, enables);
	enables = pci_read_config8(dev1, 0x4);
	printk_debug("command in reg 0x4 reads back as 0x%x\n", enables);
	
	if (! conf->enable_native_ide) {
		// Use compatability mode - per award bios
		pci_write_config32(dev1, 0x10, 0x0);
		pci_write_config32(dev1, 0x14, 0x0);
		pci_write_config32(dev1, 0x18, 0x0);
		pci_write_config32(dev1, 0x1c, 0x0);
		
		// Force interrupts to use compat mode - just like Award bios
		pci_write_config8(dev1, 0x3d, 00);
		pci_write_config8(dev1, 0x3c, 0xff);
	}
	
	
	/* set up isa bus -- i/o recovery time, rom write enable, extend-ale */
	pci_write_config8(dev0, 0x40, 0x54);
	ethernet_fixup();
	
	// Start the rtc
	rtc_init(0);
}

static void southbridge_init(struct chip *chip, enum chip_pass pass)
{

	struct southbridge_via_vt8231_config *conf = 
		(struct southbridge_via_vt8231_config *)chip->chip_info;

	switch (pass) {
	case CONF_PASS_PRE_PCI:
		vt8231_pci_enable(conf);
		break;
		
	case CONF_PASS_POST_PCI:
		vt8231_init(conf);
		pci_routing_fixup();
		break;

	case CONF_PASS_PRE_BOOT:
		dump_south();
		break;
		
	default:
		/* nothing yet */
		break;
	}
}

static void enumerate(struct chip *chip)
{
	extern struct device_operations default_pci_ops_bus;
	chip_enumerate(chip);
	chip->dev->ops = &default_pci_ops_bus;
}

struct chip_control southbridge_via_vt8231_control = {
	.enumerate = enumerate,
	.enable    = southbridge_init,
	.name      = "VIA vt8231"
};
