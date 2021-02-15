/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/acpimmio.h>
#include <console/console.h>
#include <device/device.h>
#include <southbridge/amd/common/amd_pci_util.h>
#include <device/mmio.h>
#include <southbridge/amd/cimx/sb800/SBPLATFORM.h>

static const u8 mainboard_intr_data[] = {
	[0x00] = 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,  /* INTA# - INTH# */
	[0x08] = 0x00, 0x00, 0x00, 0x00, 0x1F, 0x1F, 0x1F, 0x1F,  /* Misc-nil, 0, 1, 2,  INT from Serial irq */
	[0x10] = 0x09, 0x1F, 0x1F, 0x10, 0x1F, 0x12, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x12, 0x11, 0x12, 0x11, 0x12, 0x11, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x11, 0x13, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x10, 0x11, 0x12, 0x13
};

/* PIRQ Setup */
static void pirq_setup(void)
{
	intr_data_ptr = mainboard_intr_data;
}

/**********************************************
 * Enable the dedicated functions of the board.
 **********************************************/
static void mainboard_enable(struct device *dev)
{
	pirq_setup();

	/* Power off unused clock pins of GPP PCIe devices
	 * GPP CLK0 connected to unpopulated mini PCIe slot
	 * GPP CLK1 connected to ethernet chip
	 */
	misc_write8(0, 0xff);
	/* GPP CLK2 connected to the external USB3 controller */
	misc_write8(1, 0x0f);
	misc_write8(2, 0);
	misc_write8(3, 0);
	/* SLT_GFX_CLK connected to PCIe slot */
	misc_write8(4, 0xf0);

	/*
	 * Initialize ASF registers to an arbitrary address because someone
	 * long ago set things up this way inside the SPD read code.  The
	 * SPD read code has been made generic and moved out of the board
	 * directory, so the ASF init is being done here.
	 */
	pm_write8(0x29, 0x80);
	pm_write8(0x28, 0x61);
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
