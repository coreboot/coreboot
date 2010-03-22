#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <arch/io.h>
#include "chip.h"
#include "vgachip.h"

void vga_enable_console();


static void vga_fixup(void) {
        // we do this right here because:
        // - all the hardware is working, and some VGA bioses seem to need
        //   that
        // - we need page 0 below for coreboot tables.

        printk(BIOS_DEBUG, "INSTALL REAL-MODE IDT\n");
        setup_realmode_idt();
        printk(BIOS_DEBUG, "DO THE VGA BIOS\n");
        do_vgabios();
        post_code(0x93);
	vga_enable_console();


}
 
void write_protect_vgabios(void)
{
 	device_t dev;
 
 	printk(BIOS_INFO, "write_protect_vgabios\n");
	/* there are two possible devices. Just do both. */
 	dev = dev_find_device(PCI_VENDOR_ID_VIA, 0x3122, 0);
 	if(dev)
 		pci_write_config8(dev, 0x61, 0xaa);

 	dev = dev_find_device(PCI_VENDOR_ID_VIA, 0x3123, 0);
 	if(dev)
 		pci_write_config8(dev, 0x61, 0xaa);
}

struct chip_operations mainboard_ops = {
	CHIP_NAME("VIA EPIA-M Mainboard")
};

