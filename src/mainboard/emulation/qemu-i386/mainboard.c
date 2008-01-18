#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <arch/io.h>
#include "chip.h"

void vga_enable_console();

static void vga_init(device_t dev)
{
	/* code to make vga init run in real mode - does work but against the current coreboot philosophy */
	printk_debug("INSTALL REAL-MODE IDT\n");
        setup_realmode_idt();
        printk_debug("DO THE VGA BIOS\n");
        do_vgabios();

	vga_enable_console();
}

static struct device_operations vga_operations = {
	.read_resources   = pci_dev_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init             = vga_init,
	.ops_pci          = 0,
};

static const struct pci_driver vga_driver __pci_driver = {
	.ops = &vga_operations,
	.vendor = 0x1013,
	.device = 0x00b8,
};

struct chip_operations mainboard_emulation_qemu_i386_ops = {
	CHIP_NAME("QEMU Mainboard")
};

