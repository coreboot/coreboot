#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>

#include <arch/io.h>
#include "chip.h"

void vga_enable_console();


static int
mainboard_scan_bus(device_t root, int maxbus) 
{
	int retval;
	printk_spew("%s: root %p maxbus %d\n", __FUNCTION__, root, maxbus);
	retval = pci_scan_bus(root->bus, 0, 0xff, maxbus);
	printk_spew("DONE %s: return %d\n", __FUNCTION__, maxbus);
	return maxbus;
}

void vga_fixup(void) {
        // we do this right here because:
        // - all the hardware is working, and some VGA bioses seem to need
        //   that
        // - we need page 0 below for linuxbios tables.

        printk_debug("INSTALL REAL-MODE IDT\n");
        setup_realmode_idt();
        printk_debug("DO THE VGA BIOS\n");
        do_vgabios();
        post_code(0x93);
	vga_enable_console();


}
 
void write_protect_vgabios(void)
{
 	device_t dev;
 
 	printk_info("write_protect_vgabios\n");
 	dev = dev_find_device(PCI_VENDOR_ID_VIA, 0x3123, 0);
 	if(dev)
 		pci_write_config8(dev, 0x61, 0xaa);
 
}

static struct device_operations mainboard_operations = {
	.read_resources   = root_dev_read_resources,
	.set_resources    = root_dev_set_resources,
	.enable_resources = root_dev_enable_resources,
	.init             = root_dev_init,
	.scan_bus         = mainboard_scan_bus,
	.enable           = 0,
};

static void enable_dev(device_t dev)
{
	dev->ops = &mainboard_operations;
}

struct chip_operations mainboard_via_epia_m_control = {
	.enable_dev = enable_dev, 
	.name       = "VIA EPIA-M mainboard ",
};

