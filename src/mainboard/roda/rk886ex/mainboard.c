/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/device.h>
#include <arch/io.h>
#include <drivers/intel/gma/int15.h>
#include <ec/acpi/ec.h>

#include "m3885.h"

#define DUMP_RUNTIME_REGISTERS 0

static void backlight_enable(void)
{
	printk(BIOS_DEBUG, "Display I/O: 0x%02x\n", inb(0x60f));
}

#if DUMP_RUNTIME_REGISTERS
static void dump_runtime_registers(void)
{
	int i;

	printk(BIOS_DEBUG, "SuperIO runtime register block:\n");
	for (i = 0; i < 0x10; i++)
		printk(BIOS_DEBUG, "%02x ", i);
	printk(BIOS_DEBUG, "\n");
	for (i = 0; i < 0x10; i++)
		printk(BIOS_DEBUG, "%02x ", inb(0x600 +i));
	printk(BIOS_DEBUG, "\n");
}
#endif

static void mainboard_final(struct device *dev)
{
	/* Enable Dummy DCC ON# for DVI */
	printk(BIOS_DEBUG, "Laptop handling...\n");
	outb(inb(0x60f) & ~(1 << 5), 0x60f);
}

static void mainboard_enable(struct device *dev)
{
	/* Configure the MultiKey controller */

	/* Enable LCD Backlight */
	backlight_enable();

	/* Disable Dummy DCC -> GP45 = 1 */
	outb(inb(0x60f) | (1 << 5), 0x60f);

	/* LCD panel type is SIO GPIO40-43 */
	install_intel_vga_int15_handler(GMA_INT15_ACTIVE_LFP_INT_LVDS, GMA_INT15_PANEL_FIT_DEFAULT, GMA_INT15_BOOT_DISPLAY_DEFAULT, 3);

#if DUMP_RUNTIME_REGISTERS
	dump_runtime_registers();
#endif

	dev->ops->final = mainboard_final;
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
