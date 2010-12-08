#include <arch/io.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ops.h>
#include <device/pci_ids.h>
#include <pc80/mc146818rtc.h>
#include <arch/ioapic.h>
#include "chip.h"

/* The epia-m is really short on interrupts available, so PCI interupts A & D are ganged togther and so are B & C.
   This is how the Award bios sets it up too.
   epia can be more generous as it does not need to reserve interrupts for cardbus devices, but if changed then
   make sure that ACPI dsdt is changed to suit.

	IRQ 0 = timer
	IRQ 1 = keyboard
	IRQ 2 = cascade
	IRQ 3 = COM 2
	IRQ 4 = COM 1
	IRQ 5 = available for PCI interrupts
	IRQ 6 = floppy or availbale for PCI if floppy controller disabled
        IRQ 7 = LPT or available if LPT port disabled
	IRQ 8 = rtc
	IRQ 9 = available for PCI interrupts
	IRQ 10 = cardbus slot or available for PCI if no cardbus (ie epia)
	IRQ 11 = cardbus slot or available for PCI if no cardbus (ie epia)
	IRQ 12 = PS2 mouse (hardwired to 12)
	IRQ 13 = legacy FPU interrupt
	IRQ 14 = IDE controller 1
	IRQ 15 = IDE controller 2

*/
static const unsigned char pciIrqs[4] = { 5 , 9 , 9, 5 };

static const unsigned char usbPins[4] =      { 'A','B','C','D'};
static const unsigned char enetPins[4] =     { 'A','B','C','D'};
static const unsigned char slotPins[4] =     { 'B','C','D','A'};
static const unsigned char firewirePins[4] = { 'B','C','D','A'};
static const unsigned char vt8235Pins[4] =   { 'A','B','C','D'};
static const unsigned char vgaPins[4] =      { 'A','B','C','D'};
static const unsigned char cbPins[4] =       { 'A','B','C','D'};
static const unsigned char riserPins[4] =    { 'A','B','C','D'};


static unsigned char *pin_to_irq(const unsigned char *pin)
{
	static unsigned char Irqs[4];
	int i;
	for (i = 0 ; i < 4 ; i++)
		Irqs[i] = pciIrqs[ pin[i] - 'A' ];

	return Irqs;
}

static void pci_routing_fixup(struct device *dev)
{
	printk(BIOS_INFO, "%s: dev is %p\n", __func__, dev);

	/* set up PCI IRQ routing */
	pci_write_config8(dev, 0x55, pciIrqs[0] << 4);
	pci_write_config8(dev, 0x56, pciIrqs[1] | (pciIrqs[2] << 4) );
	pci_write_config8(dev, 0x57, pciIrqs[3] << 4);


	// firewire built into southbridge
	printk(BIOS_INFO, "setting firewire\n");
	pci_assign_irqs(0, 0x0d, pin_to_irq(firewirePins));

	// Standard usb components
	printk(BIOS_INFO, "setting usb\n");
	pci_assign_irqs(0, 0x10, pin_to_irq(usbPins));

	// VT8235 + sound hardware
	printk(BIOS_INFO, "setting vt8235\n");
	pci_assign_irqs(0, 0x11, pin_to_irq(vt8235Pins));

	// Ethernet built into southbridge
	printk(BIOS_INFO, "setting ethernet\n");
	pci_assign_irqs(0, 0x12, pin_to_irq(enetPins));

	// VGA
	printk(BIOS_INFO, "setting vga\n");
	pci_assign_irqs(1, 0x00, pin_to_irq(vgaPins));

	// PCI slot
	printk(BIOS_INFO, "setting pci slot\n");
	pci_assign_irqs(0, 0x14, pin_to_irq(slotPins));

	// Cardbus slot
	printk(BIOS_INFO, "setting cardbus slot\n");
	pci_assign_irqs(0, 0x0a, pin_to_irq(cbPins));

	// Via 2 slot riser card 2nd slot
	printk(BIOS_INFO, "setting riser slot\n");
	pci_assign_irqs(0, 0x13, pin_to_irq(riserPins));

	printk(BIOS_SPEW, "%s: DONE\n", __func__);
}

/*
 * Set up the power management capabilities directly into ACPI mode.  This
 * avoids having to handle any System Management Interrupts (SMI's) which I
 * can't figure out how to do !!!!
 */

static void setup_pm(device_t dev)
{
	// Set gen config 0
	pci_write_config8(dev, 0x80, 0x20);

	// Set ACPI base address to IO 0x400
	pci_write_config16(dev, 0x88, 0x0401);

	// set ACPI irq to 5
	pci_write_config8(dev, 0x82, 0x45);

	// primary interupt channel
	pci_write_config16(dev, 0x84, 0x30f2);

	// throttle / stop clock control
	pci_write_config8(dev, 0x8d, 0x18);

	pci_write_config8(dev, 0x93, 0x88);
	pci_write_config8(dev, 0x94, 0xb0);
	pci_write_config8(dev, 0x95, 0xc0);
	pci_write_config8(dev, 0x98, 0);
	pci_write_config8(dev, 0x99, 0xea);
	pci_write_config8(dev, 0xe4, 0x14);
	pci_write_config8(dev, 0xe5, 0x08);


	// Enable ACPI access (and setup like award)
	pci_write_config8(dev, 0x81, 0x84);

	outw(0xffff, 0x400);
	outw(0xffff, 0x420);
	outw(0xffff, 0x428);
	outl(0xffffffff, 0x430);

	outw(0x0, 0x424);
	outw(0x0, 0x42a);
	outw(0x1, 0x42c);
	outl(0x0, 0x434);
	outl(0x01, 0x438);
	outb(0x0, 0x442);
	outl(0xffff7fff, 0x448);
	outw(0x001, 0x404);
}

static void vt8235_init(struct device *dev)
{
	unsigned char enables;

	printk(BIOS_DEBUG, "vt8235 init\n");

	// enable the internal I/O decode
	enables = pci_read_config8(dev, 0x6C);
	enables |= 0x80;
	pci_write_config8(dev, 0x6C, enables);

	// Map 4MB of FLASH into the address space
	pci_write_config8(dev, 0x41, 0x7f);

	// Set bit 6 of 0x40, because Award does it (IO recovery time)
	// IMPORTANT FIX - EISA 0x4d0 decoding must be on so that PCI
	// interrupts can be properly marked as level triggered.
	enables = pci_read_config8(dev, 0x40);
	enables |= 0x45;
	pci_write_config8(dev, 0x40, enables);

	// Set 0x42 to 0xf0 to match Award bios
	enables = pci_read_config8(dev, 0x42);
	enables |= 0xf0;
	pci_write_config8(dev, 0x42, enables);

	/* Set 0x58 to 0x03 to match Award */
	pci_write_config8(dev, 0x58, 0x03);

	/* Set bit 3 of 0x4f to match award (use INIT# as cpu reset) */
	enables = pci_read_config8(dev, 0x4f);
	enables |= 0x08;
	pci_write_config8(dev, 0x4f, enables);

	// Set bit 3 of 0x4a, to match award (dummy pci request)
	enables = pci_read_config8(dev, 0x4a);
	enables |= 0x08;
	pci_write_config8(dev, 0x4a, enables);

	// Set bit 3 of 0x4f to match award (use INIT# as cpu reset)
	enables = pci_read_config8(dev, 0x4f);
	enables |= 0x08;
	pci_write_config8(dev, 0x4f, enables);

	// Set 0x58 to 0x03 to match Award
	pci_write_config8(dev, 0x58, 0x03);


	/* enable serial irq */
	pci_write_config8(dev, 0x52, 0x9);

	/* dma */
	pci_write_config8(dev, 0x53, 0x00);

	// Power management setup
	setup_pm(dev);

	/* set up isa bus -- i/o recovery time, rom write enable, extend-ale */
	pci_write_config8(dev, 0x40, 0x54);

	// Start the rtc
	rtc_init(0);
}

/* total kludge to get lxb to call our childrens set/enable functions - these are not called unless this
   device has a resource to set - so set a dummy one */
static void vt8235_read_resources(device_t dev)
{
	struct resource *res;

	pci_dev_read_resources(dev);

	res = new_resource(dev, 1);
	res->base = 0x0UL;
	res->size = 0x1000UL;
	res->limit = 0xffffUL;
	res->flags = IORESOURCE_IO | IORESOURCE_ASSIGNED | IORESOURCE_FIXED;

	res = new_resource(dev, 3); /* IOAPIC */
	res->base = IO_APIC_ADDR;
	res->size = 0x00001000;
	res->flags = IORESOURCE_MEM | IORESOURCE_ASSIGNED | IORESOURCE_FIXED;
}

static void vt8235_set_resources(device_t dev)
{
	//struct resource *resource;
	//resource = find_resource(dev,1);
	//resource->flags |= IORESOURCE_STORED;
	pci_dev_set_resources(dev);
}

static void southbridge_init(struct device *dev)
{
	vt8235_init(dev);
	pci_routing_fixup(dev);
}

static struct device_operations vt8235_lpc_ops = {
	.read_resources   = vt8235_read_resources,
	.set_resources    = vt8235_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init             = southbridge_init,
	.scan_bus         = scan_static_bus,
};

static const struct pci_driver lpc_driver __pci_driver = {
	.ops    = &vt8235_lpc_ops,
	.vendor = PCI_VENDOR_ID_VIA,
	.device = PCI_DEVICE_ID_VIA_8235,
};
