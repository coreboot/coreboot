#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <arch/io.h>
#include <cpu/x86/msr.h>
#include <cpu/amd/lxdef.h>
#include "chip.h"

#define DIVIL_LBAR_GPIO		0x5140000c

static void init_gpio()
{
	msr_t msr;
	printk_debug("Initializing GPIO module...\n");

	// initialize the GPIO LBAR
	msr.lo = GPIO_BASE;
	msr.hi = 0x0000f001;
	wrmsr(DIVIL_LBAR_GPIO, msr);
	msr = rdmsr(DIVIL_LBAR_GPIO);
	printk_debug("DIVIL_LBAR_GPIO set to 0x%08x 0x%08x\n", msr.hi, msr.lo);
}

static void init(struct device *dev)
{
	// BOARD-SPECIFIC INIT
	printk_debug("ARTECGROUP DBE61 ENTER %s\n", __FUNCTION__);

	init_gpio();

	printk_debug("ARTECGROUP DBE61 EXIT %s\n", __FUNCTION__);
}

static void enable_dev(struct device *dev)
{
        dev->ops->init = init;
}

struct chip_operations mainboard_artecgroup_dbe61_ops = {
	CHIP_NAME("Artec Group dbe61 mainboard")
        .enable_dev = enable_dev,
};
