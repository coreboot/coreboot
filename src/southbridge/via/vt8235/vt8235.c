#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ops.h>
#include <device/pci_ids.h>
#include <pc80/keyboard.h>
#include "vt8235.h"
#include "chip.h"

/*
 * Base VT8235.
 */
static device_t lpc_dev;

void hard_reset(void) 
{
	printk_err("NO HARD RESET ON VT8235! FIX ME!\n");
}

static void keyboard_on(struct device *dev)
{
	unsigned char regval;

	regval = pci_read_config8(dev, 0x51);
//	regval |= 0x0f; 
	/* !!!FIX let's try this */
	regval |= 0x1d; 
	pci_write_config8(dev, 0x51, regval);

	init_pc_keyboard(0x60, 0x64, 0);
}

void dump_south(void)
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

void set_led(struct device *dev)
{
	// set power led to steady now that lxbios has virtually done its job
	pci_write_config8(dev, 0x94, 0xb0);
}

static void vt8235_enable(struct device *dev)
{
	struct southbridge_via_vt8235_config *conf = dev->chip_info;

	printk_debug("In vt8235_enable.\n");
	if (!lpc_dev) {
		lpc_dev = dev_find_device(PCI_VENDOR_ID_VIA,
				PCI_DEVICE_ID_VIA_8235, 0);
		if (conf->enable_keyboard)
			keyboard_on(lpc_dev);
	}
}

struct chip_operations southbridge_via_vt8235_ops = {
	CHIP_NAME("VIA vt8235")
	.enable_dev = vt8235_enable,
};
