#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ops.h>
#include <device/pci_ids.h>
#include <pc80/keyboard.h>
#include <pc80/i8259.h>
#include "chip.h"

static void keyboard_on(struct device *dev)
{
	u8 regval;

	regval = pci_read_config8(dev, 0x51);
	regval |= 0x05;
	regval &= 0xfd;
	pci_write_config8(dev, 0x51, regval);

	pc_keyboard_init(0);
}

#ifdef UNUSED_CODE
void dump_south(device_t dev0)
{
	int i,j;

	for(i = 0; i < 256; i += 16) {
		printk(BIOS_DEBUG, "0x%x: ", i);
		for(j = 0; j < 16; j++) {
			printk(BIOS_DEBUG, "%02x ", pci_read_config8(dev0, i+j));
		}
		printk(BIOS_DEBUG, "\n");
	}
}

void set_led(void)
{
	// set power led to steady now that coreboot has virtually done its job
	device_t dev;
	dev = dev_find_device(PCI_VENDOR_ID_VIA, PCI_DEVICE_ID_VIA_8235, 0);
	pci_write_config8(dev, 0x94, 0xb0);
}
#endif

static void vt8235_enable(struct device *dev)
{
	u8 regval;
	u16 vendor,model;

	vendor = pci_read_config16(dev,0);
	model = pci_read_config16(dev,0x2);

	printk(BIOS_DEBUG, "In vt8235_enable %04x %04x.\n",vendor,model);

	/* If this is not the southbridge itself just return.
	 * This is necessary because USB devices are slot 10, whereas this
	 * device is slot 11 therefore usb devices get called first during
	 * the bus scan. We don't want to wait until we could do dev->init
	 * because that's too late.
	 */

	if( (vendor != PCI_VENDOR_ID_VIA) || (model != PCI_DEVICE_ID_VIA_8235))
		return;

	printk(BIOS_DEBUG, "Initialising Devices\n");

	/* make sure interupt controller is configured before keyboard init */
	setup_i8259();

	/* enable RTC and ethernet */
	regval = pci_read_config8(dev, 0x51);
	regval |= 0x18;
	pci_write_config8(dev, 0x51, regval);

	/* turn on keyboard */
	keyboard_on(dev);

	/* enable USB 1.1 & USB 2.0 - redundant really since we've
	 * already been there - see note above
	 */
   	regval = pci_read_config8(dev, 0x50);
	regval &= ~(0x36);
	pci_write_config8(dev, 0x50, regval);
}

struct chip_operations southbridge_via_vt8235_ops = {
	CHIP_NAME("VIA VT8235 Southbridge")
	.enable_dev = vt8235_enable,
};
