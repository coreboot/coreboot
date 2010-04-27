#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <pc80/keyboard.h>
#include <arch/io.h>
#include "chip.h"

/* not sure how these are routed in qemu */
static const unsigned char enetIrqs[4] = { 11, 0, 0, 0 };

static void qemu_init(device_t dev)
{
	/* The VGA OPROM already lives at 0xc0000,
	 * force coreboot to use it.
	 */
	dev->on_mainboard = 1;

	/* Now do the usual initialization */
	pci_dev_init(dev);

	/* This sneaked in here, because Qemu does not
	 * emulate a SuperIO chip
	 */
	pc_keyboard_init(0);

	/* The PIRQ table is not working well for interrupt routing purposes.
	 * so we'll just set the IRQ directly.
	*/
	printk(BIOS_INFO, "setting ethernet\n");
	pci_assign_irqs(0, 3, enetIrqs);
}

static struct device_operations vga_operations = {
	.read_resources   = pci_dev_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init             = qemu_init,
	.ops_pci          = 0,
};

static const struct pci_driver vga_driver __pci_driver = {
	.ops = &vga_operations,
	.vendor = 0x1013,
	.device = 0x00b8,
};

struct chip_operations mainboard_ops = {
	CHIP_NAME("QEMU Mainboard")
};

