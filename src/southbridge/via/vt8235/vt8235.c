
#include <arch/io.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ops.h>
#include <device/pci_ids.h>
#include <console/console.h>
#include "vt8235.h"
#include "chip.h"

void rtc_init(int i);

void pc_keyboard_init(void);

void hard_reset(void) 
{
	printk_err("NO HARD RESET ON VT8235! FIX ME!\n");
}

static void usb_on(int enable)
{
	unsigned char regval;

	/* Base 8235 controller */
	device_t dev0 = dev_find_device(PCI_VENDOR_ID_VIA, PCI_DEVICE_ID_VIA_8235, 0);
	/* USB controller 1 */
	device_t dev1 = dev_find_device(PCI_VENDOR_ID_VIA, PCI_DEVICE_ID_VIA_82C586_2, 0);
	/* USB controller 2 */
	device_t dev2 = dev_find_device(PCI_VENDOR_ID_VIA, PCI_DEVICE_ID_VIA_82C586_2, dev1);
	/* USB controller 2 */
	device_t dev3 = dev_find_device(PCI_VENDOR_ID_VIA, 
	PCI_DEVICE_ID_VIA_82C586_2, dev2);
	
	if(enable){
        	if(dev0) {
	    		regval = pci_read_config8(dev0, 0x50);
	    		regval &= ~(0x36);
	    		pci_write_config8(dev0, 0x50, regval);
		}

		/* enable USB1 */
		if(dev1) {
	    		pci_write_config8(dev1, 0x04, 0x07);
		}

		/* enable USB2 */
		if(dev2) {
	    		pci_write_config8(dev2, 0x04, 0x07);
		}

		/* enable USB3 */
		if(dev3) {
	    		pci_write_config8(dev3, 0x04, 0x07);
		}

	}else{
        	if(dev0) {
	    		regval = pci_read_config8(dev0, 0x50);
	    		regval |= 0x36;    
	    		pci_write_config8(dev0, 0x50, regval);
		}

		/* disable USB1 */
		if(dev1) {
	    		pci_write_config8(dev1, 0x3c, 0x00);
	    		pci_write_config8(dev1, 0x04, 0x00);
		}

		/* disable USB2 */
		if(dev2) {
	    		pci_write_config8(dev2, 0x3c, 0x00);
	    		pci_write_config8(dev2, 0x04, 0x00);
		}

		/* disable USB3 */
		if(dev3) {
	    		pci_write_config8(dev3, 0x3c, 0x00);
	    		pci_write_config8(dev3, 0x04, 0x00);
		}
	}
}

static void keyboard_on(void)
{
	unsigned char regval;

	/* Base 8235 controller */
	device_t dev0 = dev_find_device(PCI_VENDOR_ID_VIA, \
						PCI_DEVICE_ID_VIA_8235, 0);

	if (dev0) {
		regval = pci_read_config8(dev0, 0x51);
//		regval |= 0x0f; 
		/* !!!FIX let's try this */
		regval |= 0x1d; 
		pci_write_config8(dev0, 0x51, regval);
	}
	pc_keyboard_init();
}

static void nvram_on(void)
{
	/*
	 * the VIA 8235 South has a very different nvram setup than the 
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
static void vt8235_pci_enable(struct southbridge_via_vt8235_config *conf) 
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

/* taken some liberties - changed irq structures to pins numbers so that it is easier to
 * change PCI irq assignments without having to change each PCI function individually
 */

/* pciIrqs contains the irqs assigned for PCI pins A-D */
/* setting will depend on motherboard as irqs can be quite scarce */
/* e.g on EPIA-MII, 16 bit CF card wants a dedicated IRQ. A 16 bit card in pcmcia socket */
/* may want another - for now only claim 3 interupts for PCI, leaving at least one spare */
/* for CF.  */
/* On EPIA-M one could allocated all four irqs to different numbers since there are no cardbus */
/* devices */


static const unsigned char pciIrqs[4] = { 5 , 9 , 9, 10 };

static const unsigned char usbPins[4] =      { 'A','B','C','D'};
static const unsigned char enetPins[4] =     { 'A','B','C','D'};
static const unsigned char slotPins[4] =     { 'B','C','D','A'};
static const unsigned char firewirePins[4] = { 'B','C','D','A'};
static const unsigned char vt8235Pins[4] =   { 'A','B','C','D'};
static const unsigned char vgaPins[4] =      { 'A','B','C','D'};
static const unsigned char cbPins[4] =       { 'A','B','C','D'};
static const unsigned char riserPins[4] =    { 'A','B','C','D'};
/*
	Our IDSEL mappings are as follows
	PCI slot is AD31          (device 15) (00:14.0)
	Southbridge is AD28       (device 12) (00:11.0)
*/
static unsigned char *pin_to_irq(const unsigned char *pin)
{
	static unsigned char Irqs[4];
	int i;
	for (i = 0 ; i < 4 ; i++)
		Irqs[i] = pciIrqs[ pin[i] - 'A' ];

	return Irqs;
}
static void pci_routing_fixup(void)
{
	device_t dev;

        dev = dev_find_device(PCI_VENDOR_ID_VIA, PCI_DEVICE_ID_VIA_8235, 0);
	printk_info("%s: dev is %p\n", __FUNCTION__, dev);
	if (dev) {
		/* initialize PCI interupts - these assignments depend
		   on the PCB routing of PINTA-D 

		   PINTA = IRQ11
		   PINTB = IRQ5
		   PINTC = IRQ10
		   PINTD = IRQ12
		*/
		pci_write_config8(dev, 0x55, pciIrqs[0] << 4);
		pci_write_config8(dev, 0x56, pciIrqs[1] | (pciIrqs[2] << 4) );
		pci_write_config8(dev, 0x57, pciIrqs[3] << 4);

	}



	// firewire built into southbridge
	printk_info("setting firewire\n");
	pci_assign_irqs(0, 0x0d, pin_to_irq(firewirePins) );

	// Standard usb components
	printk_info("setting usb\n");
	pci_assign_irqs(0, 0x10, pin_to_irq(usbPins) );

	// VT8235 + sound hardware
	printk_info("setting vt8235\n");
	pci_assign_irqs(0, 0x11, pin_to_irq(vt8235Pins) );

	// Ethernet built into southbridge
	printk_info("setting ethernet\n");
	pci_assign_irqs(0, 0x12, pin_to_irq(enetPins) );

	// VGA
	printk_info("setting vga\n");
	pci_assign_irqs(1, 0x00, pin_to_irq(vgaPins) );

	// PCI slot
	printk_info("setting pci slot\n");
	pci_assign_irqs(0, 0x14, pin_to_irq(slotPins) );

	// Cardbus slot 
	printk_info("setting cardbus slot\n");
	pci_assign_irqs(0, 0x0a, pin_to_irq(cbPins) );

	// Via 2 slot riser card 2nd slot
	printk_info("setting riser slot\n");
	pci_assign_irqs(0, 0x13, pin_to_irq(riserPins) );


}


void
dump_south(void)
{
	device_t dev0;
	dev0 = dev_find_device(PCI_VENDOR_ID_VIA, PCI_DEVICE_ID_VIA_8235, 0);
	int i,j;
	
	for(i = 0; i < 256; i += 16) {
		printk_debug("0x%x: ", i);
		for(j = 0; j < 16; j++) {
			printk_debug("%02x ", pci_read_config8(dev0, i+j));
		}
		printk_debug("\n");
	}
}

void set_led(void)
{

	// set power led to steady now that lxbios has virtually done its job
	device_t dev0;
	dev0 = dev_find_device(PCI_VENDOR_ID_VIA, PCI_DEVICE_ID_VIA_8235,0);

	pci_write_config8(dev0,0x94,0xb0);

}

/* set up the power management capabilities directly into ACPI mode */
/* this avoids having to handle any System Management Interrupts (SMI's) which I can't */
/* figure out how to do !!!! */ 

void setup_pm(device_t dev0)
{

	// Set gen config 0
	pci_write_config8(dev0,0x80,0x20);

	// Set ACPI base address to IO 0x4000
	pci_write_config16(dev0, 0x88, 0x0401);
	
	// set ACPI irq to 5
	pci_write_config8(dev0,0x82,0x55);

	// primary interupt channel
	pci_write_config16(dev0,0x84,0x30f2);

	// throttle / stop clock control
	pci_write_config8(dev0,0x8d,0x18);

	pci_write_config8(dev0,0x93,0x88);
	//pci_write_config8(dev0,0x94,0xb0);
	pci_write_config8(dev0,0x95,0xc0);
	pci_write_config8(dev0,0x98,0);
	pci_write_config8(dev0,0x99,0xea);
	pci_write_config8(dev0,0xe4,0x14);
	pci_write_config8(dev0,0xe5,0x08);


	// Enable ACPI access (and setup like award)
	pci_write_config8(dev0, 0x81, 0x84);

	outw(0xffff,0x400);
	outw(0xffff,0x420);
	outw(0xffff,0x428);
	outl(0xffffffff,0x430);
	
	outw(0x0,0x424);
	outw(0x0,0x42a);
	outw(0x1,0x42c);
	outl(0x0,0x434);
	outl(0x01,0x438);
	outb(0x0,0x442);
	outl(0xffff7fff,0x448);
	outw(0x001,0x404);


}

static void vt8235_init(struct southbridge_via_vt8235_config *conf)
{
	unsigned char enables;
	device_t dev0;
	device_t dev1;
	//device_t devpwr;
	//int i;

	// to do: use the pcibios_find function here, instead of 
	// hard coding the devfn. 
	// done - kevinh/Ispiri
	printk_debug("vt8235 init\n");
	/* Base 8235 controller */
	dev0 = dev_find_device(PCI_VENDOR_ID_VIA, PCI_DEVICE_ID_VIA_8235, 0);
	/* IDE controller */
	dev1 = dev_find_device(PCI_VENDOR_ID_VIA, PCI_DEVICE_ID_VIA_82C586_1, 0);
	/* Power management controller */
	//devpwr = dev_find_device(PCI_VENDOR_ID_VIA, PCI_DEVICE_ID_VIA_8235_4, 0);

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
	enables |= 0x45;
	pci_write_config8(dev0, 0x40, enables);
	
	// Set 0x42 to 0xf0 to match Award bios
	enables = pci_read_config8(dev0, 0x42);
	enables |= 0xf0;
	pci_write_config8(dev0, 0x42, enables);
	

	/* Set 0x58 to 0x03 to match Award */
	pci_write_config8(dev0, 0x58, 0x03);

	/* Set bit 3 of 0x4f to match award (use INIT# as cpu reset) */
	enables = pci_read_config8(dev0, 0x4f);
	enables |= 0x08;
	pci_write_config8(dev0, 0x4f, enables);



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
	
	
	/* enable serial irq */
	pci_write_config8(dev0,0x52,0x9);

	/* dma */
	pci_write_config8(dev0, 0x53, 0x00);

	/* Use compatability mode - per award bios */
	pci_write_config32(dev1, 0x10, 0x0);
	pci_write_config32(dev1, 0x14, 0x0);
	pci_write_config32(dev1, 0x18, 0x0);
	pci_write_config32(dev1, 0x1c, 0x0);


	// Power management setup
	setup_pm(dev0);
	
	//
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

	struct southbridge_via_vt8235_config *conf = 
		(struct southbridge_via_vt8235_config *)chip->chip_info;

	switch (pass) {
	case CONF_PASS_PRE_PCI:
		vt8235_pci_enable(conf);
		break;
		
	case CONF_PASS_POST_PCI:
		/* initialise the PIC - particularly so that VGA bios init code 
                   doesn't get nasty unknown interupt vectors when it tries to establish 
                   its interrupts. */
		setup_i8259();
		vt8235_init(conf);
		pci_routing_fixup();
		usb_on(1);
		keyboard_on();
		vga_fixup();



		break;

	case CONF_PASS_PRE_BOOT:
		dump_south();
		set_led();
		break;
		
	default:
		/* nothing yet */
		break;
	}
}

struct chip_operations southbridge_via_vt8235_control = {
	.enable    = southbridge_init,
	.name      = "VIA vt8235"
};
