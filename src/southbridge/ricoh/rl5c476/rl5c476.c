/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ops.h>
#include <device/pci_ids.h>
#include <console/console.h>
#include <device/cardbus.h>
#include <delay.h>
#include "rl5c476.h"
#include "chip.h"

static int enable_cf_boot = 0;
static unsigned int cf_base;

static void rl5c476_init(struct device *dev)
{
	pc16reg_t *pc16;
	unsigned char *base;

	/* cardbus controller function 1 for CF Socket */
	printk(BIOS_DEBUG, "Ricoh RL5c476: Initializing.\n");

	printk(BIOS_DEBUG, "CF Base = %0x\n",cf_base);

	/* misc control register */
	pci_write_config16(dev,0x82,0x00a0);

	/* set up second slot as compact flash port if asked to do so */

	if (!enable_cf_boot) {
		printk(BIOS_DEBUG, "CF boot not enabled.\n");
		return;
	}

	if (PCI_FUNC(dev->path.pci.devfn) != 1) {
		// Only configure if second CF slot.
		return;
	}

	/* make sure isa interrupts are enabled */
	pci_write_config16(dev,0x3e,0x0780);

	/* pick up where 16 bit card control structure is
	 * (0x800 bytes into config structure)
	 */
	base = (unsigned char *)pci_read_config32(dev,0x10);
	pc16 = (pc16reg_t *)(base + 0x800);

	/* disable memory and io windows and turn off socket power */
	pc16->pwctrl = 0;

	/* disable irq lines */
	pc16->igctrl = 0;

	/* disable memory and I/O windows */
	pc16->awinen = 0;

	/* reset card, configure for I/O and set IRQ line */
	pc16->igctrl = 0x69;

	/* set io window 0 for 1e0 - 1ef */
	/* NOTE: This now sets CF up on a contiguous I/O window of
	 * 16 bytes, 0x1e0 to 0x1ef.
	 * Be warned that this is not a standard IDE address as
	 * automatically detected by the likes of FILO, and would need
	 * patching to recognize these addresses as an IDE drive.
	 *
	 * An earlier version of this driver set up 2 I/O windows to
	 * emulate the expected addresses for IDE2, however the PCMCIA
	 * package within Linux then could not re-initialize the
	 * device as it tried to take control of it. So I believe it is
	 * easier to patch Filo or the like to pick up this drive
	 * rather than playing silly games as the kernel tries to
	 * boot.
	 *
	 * Nonetheless, FILO needs a special option enabled to boot
	 * from this configuration, and it needs to clean up
	 * afterwards. Please refer to FILO documentation and source
	 * code for more details.
	 */
	pc16->iostl0 = 0xe0;
	pc16->iosth0 = 1;

	pc16->iospl0 = 0xef;
	pc16->iosph0 = 1;

	pc16->ioffl0 = 0;
	pc16->ioffh0 = 0;

	/* clear window 1 */
	pc16->iostl1 = 0;
	pc16->iosth1 = 0;

	pc16->iospl1 = 0;
	pc16->iosph1 = 0;

	pc16->ioffl1 = 0x0;
	pc16->ioffh1 = 0;

	/* set up CF config window */
	pc16->smpga0 = cf_base>>24;
	pc16->smsth0 = (cf_base>>20)&0x0f;
	pc16->smstl0 = (cf_base>>12)&0xff;
	pc16->smsph0 = ((cf_base>>20)&0x0f) | 0x80;
	pc16->smspl0 = (cf_base>>12)&0xff;
	pc16->moffl0 = 0;
	pc16->moffh0 = 0x40;

	/* set I/O width for Auto Data width */
	pc16->ioctrl = 0x22;

	/* enable I/O window 0 and 1 */
	pc16->awinen = 0xc1;

	pc16->miscc1 = 1;

	/* apply power and enable outputs */
	pc16->pwctrl = 0xb0;

	// delay could be optimised, but this works
	udelay(100000);

	pc16->igctrl = 0x69;

	/* 16 bit CF always have first config byte at 0x200 into
	 * Config structure, but CF+ may not according to spec -
	 * should locate through reading tuple data, but this should
	 * do for now.
	 */
	unsigned char *cptr;
	cptr = (unsigned char *)(cf_base + 0x200);
	printk(BIOS_DEBUG, "CF Config = %x\n",*cptr);

	/* Set CF to decode 16 IO bytes on any 16 byte boundary -
	 * rely on the io windows of the bridge set up above to
	 * map those bytes into the addresses for IDE controller 3
	 * (0x1e8 - 0x1ef and 0x3ed - 0x3ee)
	 */
	*cptr = 0x41;
}

static void rl5c476_read_resources(struct device *dev)
{

	struct resource *resource;

	/* For CF socket we need an extra memory window for
	 * the control structure of the CF itself
	 */
	if (enable_cf_boot && (PCI_FUNC(dev->path.pci.devfn) == 1)) {
		/* fake index as it isn't in PCI config space */
		resource = new_resource(dev, 1);
		resource->flags |= IORESOURCE_MEM;
		resource->size = 0x1000;
		resource->align = resource->gran = 12;
		resource->limit= 0xffff0000;
	}
	cardbus_read_resources(dev);
}

static void rl5c476_set_resources(struct device *dev)
{
	struct resource *resource;
	printk(BIOS_DEBUG, "%s In set resources\n",dev_path(dev));
	if (enable_cf_boot && (PCI_FUNC(dev->path.pci.devfn) == 1)) {
		resource = find_resource(dev,1);
		if (!(resource->flags & IORESOURCE_STORED)) {
			resource->flags |= IORESOURCE_STORED;
			printk(BIOS_DEBUG, "%s 1 ==> %llx\n", dev_path(dev), resource->base);
			cf_base = resource->base;
		}
	}

	pci_dev_set_resources(dev);

}

static void rl5c476_set_subsystem(struct device *dev, unsigned int vendor,
				  unsigned int device)
{
	u16 miscreg = pci_read_config16(dev, 0x82);
	/* Enable subsystem id register writes */
	pci_write_config16(dev, 0x82, miscreg | 0x40);

	pci_dev_set_subsystem(dev, vendor, device);

	/* restore original contents */
	pci_write_config16(dev, 0x82, miscreg);
}

static struct pci_operations rl5c476_pci_ops = {
	.set_subsystem    = rl5c476_set_subsystem,
};

static struct device_operations ricoh_rl5c476_ops = {
	.read_resources   = rl5c476_read_resources,
	.set_resources    = rl5c476_set_resources,
	.enable_resources = cardbus_enable_resources,
	.init             = rl5c476_init,
	.scan_bus         = pci_scan_bridge,
	.ops_pci          = &rl5c476_pci_ops,
};

static const struct pci_driver ricoh_rl5c476_driver __pci_driver = {
	.ops    = &ricoh_rl5c476_ops,
	.vendor = PCI_VID_RICOH,
	.device = PCI_DID_RICOH_RL5C476,
};

static void southbridge_init(struct device *dev)
{
	struct southbridge_ricoh_rl5c476_config *conf = dev->chip_info;
	enable_cf_boot = conf->enable_cf;
}

struct chip_operations southbridge_ricoh_rl5c476_ops = {
	CHIP_NAME("Ricoh RL5C476 CardBus Controller")
	.enable_dev    = southbridge_init,
};
