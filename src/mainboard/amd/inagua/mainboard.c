/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/acpimmio.h>
#include <console/console.h>
#include <device/device.h>
#include <southbridge/amd/common/amd_pci_util.h>

static const u8 mainboard_intr_data[] = {
	[0x00] = 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,  /* INTA# - INTH# */
	[0x08] = 0x00, 0x00, 0x00, 0x00, 0x1F, 0x1F, 0x1F, 0x1F,  /* Misc-nil, 0, 1, 2,  INT from Serial irq */
	[0x10] = 0x09, 0x1F, 0x1F, 0x10, 0x1F, 0x12, 0x1F, 0x00,
	[0x18] = 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	[0x20] = 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x00, 0x00,
	[0x28] = 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	[0x30] = 0x12, 0x11, 0x12, 0x11, 0x12, 0x11, 0x12, 0x00,
	[0x38] = 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	[0x40] = 0x11, 0x13, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	[0x48] = 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	[0x50] = 0x10, 0x11, 0x12, 0x13
};

static void init_gpios(void)
{
	/**
	 * GPIO32 Pcie Device DeAssert for APU
	 * GPIO25 Pcie LAN,       APU GPP2
	 * GPIO02 MINIPCIE SLOT1, APU GPP3
	 * GPIO50 Pcie Device DeAssert for Hudson Southbridge
	 * GPIO05 Express Card,     SB  GPP0
	 * GPIO26 NEC USB3.0GPPUSB, SB  GPP1
	 * GPIO00 MINIPCIE SLOT2,   SB  GPP2
	 * GPIO05 Pcie X1 Slot,     SB  GPP3
	 */

	/* Multi-function pins switch to GPIO0-35, these pins are shared with
	 * PCI pins, make sure Hudson PCI device is disabled.
	 */
	pm_write8(0xea, (pm_read8(0xea) & 0xfe) | 1);

	/* select IOMux to function1/2, corresponds to GPIO */
	iomux_write8(0x32, (iomux_read8(0x32) & 0xfc) | 1);
	iomux_write8(0x50, (iomux_read8(0x50) & 0xfc) | 2);

	/* output low */
	gpio_100_write8(0x20, 0x48);
	gpio_100_write8(0x32, 0x48);
}

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
	/* Initialize the PIRQ data structures for consumption */
	pirq_setup();

	/* Inagua mainboard specific setting */
	init_gpios();

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
