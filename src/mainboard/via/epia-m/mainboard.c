#include <printk.h>
#include <pci.h>
#include <pci_ids.h>
#include <cpu/p5/io.h>

#include <types.h>

static const unsigned char usbIrqs[4] = { 11, 10, 12, 5 };
static const unsigned char enetIrqs[4] = { 11, 5, 10, 12 };
static const unsigned char slotIrqs[4] = { 10, 12, 5, 11 };
static const unsigned char firewireIrqs[4] = {10, 12, 5, 11 };
static const unsigned char vt8235Irqs[4] = { 5,10, 12, 11 };
static const unsigned char vgaIrqs[4] = { 11, 5, 12, 10 };

/*
	Our IDSEL mappings are as follows
	PCI slot is AD31          (device 15) (00:14.0)
	Southbridge is AD28       (device 12) (00:11.0)
*/
static void pci_routing_fixup(void)
{
	struct pci_dev *dev;

        dev = pci_find_device(PCI_VENDOR_ID_VIA, 0x3177, 0);
	if (dev != NULL) {
		/*
		 * initialize PCI interupts - these assignments depend
		 * on the PCB routing of PINTA-D
		 *
		 * PINTA = IRQ11
		 * PINTB = IRQ10
		 * PINTC = IRQ12
		 * PINTD = IRQ5
		 */
		pci_write_config_byte(dev, 0x55, 0xb0);
		pci_write_config_byte(dev, 0x56, 0xca);
		pci_write_config_byte(dev, 0x57, 0x50);

	}

	// firewire built into southbridge
	printk_info("setting firewire\n");
	pci_assign_irqs(0, 0x0d, firewireIrqs);

	// Standard usb components
	printk_info("setting usb\n");
	pci_assign_irqs(0, 0x10, usbIrqs);

	// VT8235 + sound hardware
	printk_info("setting vt8235\n");
	pci_assign_irqs(0, 0x11, vt8235Irqs);

	// Ethernet built into southbridge
	printk_info("setting ethernet\n");
	pci_assign_irqs(0, 0x12, enetIrqs);

	// VGA
	printk_info("setting vga\n");
	pci_assign_irqs(1, 0x00, vgaIrqs);

	// PCI slot
	printk_info("setting pci slot\n");
	pci_assign_irqs(0, 0x14, slotIrqs);

	// vt8235 slot
	printk_info("setting vt8235 slot\n");
	pci_assign_irqs(0, 0x11, vt8235Irqs);
}

static unsigned char vt1211hwmonitorinits[]={
 0x10,0x3, 0x11,0x10, 0x12,0xd, 0x13,0x7f,
 0x14,0x21, 0x15,0x81, 0x16,0xbd, 0x17,0x8a,
 0x18,0x0, 0x19,0x0, 0x1a,0x0, 0x1b,0x0,
 0x1d,0xff, 0x1e,0x0, 0x1f,0x73, 0x20,0x67,
 0x21,0xc1, 0x22,0xca, 0x23,0x74, 0x24,0xc2,
 0x25,0xc7, 0x26,0xc9, 0x27,0x7f, 0x29,0x0,
 0x2a,0x0, 0x2b,0xff, 0x2c,0x0, 0x2d,0xff,
 0x2e,0x0, 0x2f,0xff, 0x30,0x0, 0x31,0xff,
 0x32,0x0, 0x33,0xff, 0x34,0x0, 0x39,0xff,
 0x3a,0x0, 0x3b,0xff, 0x3c,0xff, 0x3d,0xff,
 0x3e,0x0, 0x3f,0xb0, 0x43,0xff, 0x44,0xff,
 0x46,0xff, 0x47,0x50, 0x4a,0x3, 0x4b,0xc0,
 0x4c,0x0, 0x4d,0x0, 0x4e,0xf, 0x5d,0x77,
 0x5c,0x0, 0x5f,0x33, 0x40,0x1};

void
mainboard_fixup()
{
	printk_info("Mainboard fixup\n");

	northbridge_fixup();
	southbridge_fixup();
}

void
final_southbridge_fixup()
{
 	struct pci_dev *dev;
 	int i;

	printk_info("Southbridge fixup\n");

	//	nvram_on();
	//	pci_routing_fixup();
	northbridge_fixup();
	southbridge_fixup();
 
 	dev = pci_find_device(PCI_VENDOR_ID_VIA, 0x3123, 0);
 	if(dev) {
 // enable shadow RAM for c0000-cffff
 		pci_write_config_byte(dev, 0x61, 0xff);
 // enable 2x + 4x agp
 		pci_write_config_byte(dev, 0xac, 0x2f);
 		pci_write_config_byte(dev, 0xae, 0x04);
 	}
 
 
 // Activate the vt1211 hardware monitor
 	outb(0x87,0x2e); // enter config mode part a
 	outb(0x87,0x2e); // enter config mode part b
 	outb(7,0x2e); // device index#
 	outb(0x0b,0x2f); // select hardware monitor LDN
 	outb(0x30,0x2e); // activate index#
 	outb(1,0x2f);
 	outb(0xaa,0x2e); // exit config mode
 
 
 // initialize vt1211 hardware monitor registers, which are at 0xECXX
 	for(i=0;i<sizeof(vt1211hwmonitorinits);i+=2)
 		outb(vt1211hwmonitorinits[i+1],0xec00+vt1211hwmonitorinits[i]);
 
 // clear out cmos contents, it seems to cause trouble
 	for(i=14;i<256;++i)
 	{
		outb(i,0x74);
 		outb(0,0x75);
 	}
#ifdef CONFIG_EPIAMVERSIONSTRING
 // Use CMOS bytes 128+ to store version string
 	for(i=0;;++i)
 	{
 		char c;
 		outb(i+128,0x74);
 		c=CONFIG_EPIAMVERSIONSTRING[i];
 		outb(c,0x75);
 		if(!c) break;
 	}
#endif
 
 }
 
void write_protect_vgabios(void)
{
 	struct pci_dev *dev;
 
 	printk_info("write_protect_vgabios\n");
 	dev = pci_find_device(PCI_VENDOR_ID_VIA, 0x3123, 0);
 	if(dev)
 		pci_write_config_byte(dev, 0x61, 0xaa);
 
}

void
final_mainboard_fixup()
{
	printk_info("Final mainboard fixup\n");
	final_southbridge_fixup();
}

int handleint21( unsigned long *edi, unsigned long *esi, unsigned long *ebp,
			  unsigned long *esp, unsigned long *ebx, unsigned long *edx,
			  unsigned long *ecx, unsigned long *eax, unsigned long *flags)
{
int res=-1;
	switch(*eax&0xffff)
	{
	case 0x5f19:
		break;
	case 0x5f18:
		*eax=0x5f;
		*ebx=0x15; // MCLK = 133, 32M frame buffer
		res=0;
		break;
	case 0x5f02:
		*eax=0x5f;
		*ebx=0 | (3<<8);
		*ecx=5 | (0<<8) | (0<<16);
		res=0;
		break;
	case 0x5f0f:
		*eax=0x5f;
		*ebx=0;
		*ecx=0;
		*edx=0;
		res=0;
		break;
	}
	return res;
}
