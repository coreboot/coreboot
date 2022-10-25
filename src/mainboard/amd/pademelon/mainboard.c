/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <amdblocks/agesawrapper.h>
#include <amdblocks/amd_pci_util.h>
#include <soc/gpio.h>
#include <soc/pci_devs.h>
#include <soc/southbridge.h>

#include "gpio.h"

/* The IRQ mapping in fch_irq_map ends up getting written to the indirect address space that is
   accessed via I/O ports 0xc00/0xc01. */
static const struct fch_irq_routing fch_irq_map[] = {
	{ PIRQ_A,	 3,		16 },
	{ PIRQ_B,	 4,		17 },
	{ PIRQ_C,	 5,		18 },
	{ PIRQ_D,	 7,		19 },
	{ PIRQ_E,	11,		20 },
	{ PIRQ_F,	10,		21 },
	{ PIRQ_G,	PIRQ_NC,	22 },
	{ PIRQ_H,	PIRQ_NC,	23 },
	{ PIRQ_SCI,	PIRQ_NC,	 9 },
	{ PIRQ_SMBUS,	PIRQ_NC,	PIRQ_NC },
	{ PIRQ_HDA,	 3,		16 },
	{ PIRQ_SD,	PIRQ_NC,	16 },
	{ PIRQ_SDIO,	PIRQ_NC,	PIRQ_NC },
	{ PIRQ_EHCI,	 5,		18 },
	{ PIRQ_XHCI,	 4,		18 },
	{ PIRQ_SATA,	PIRQ_NC,	19 },
	{ PIRQ_GPIO,	 7,		 7 },
	{ PIRQ_I2C0,	 3,		 3 },
	{ PIRQ_I2C1,	15,		15 },
	{ PIRQ_I2C2,	 6,		 6 },
	{ PIRQ_I2C3,	14,		14 },
	{ PIRQ_UART0,	10,		10 },
	{ PIRQ_UART1,	11,		11 },

	/* The MISC registers are not interrupt numbers */
	{ PIRQ_MISC,	0xfa,	0x00 },
	{ PIRQ_MISC0,	0xf1,	0x00 },
	{ PIRQ_MISC1,	0x00,	0x00 },
	{ PIRQ_MISC2,	0x00,	0x00 },
};

const struct fch_irq_routing *mb_get_fch_irq_mapping(size_t *length)
{
	*length = ARRAY_SIZE(fch_irq_map);
	return fch_irq_map;
}

/*
 * This table defines the index into the picr/intr_data tables for each
 * device.  Any enabled device and slot that uses hardware interrupts should
 * have an entry in this table to define its index into the FCH PCI_INTR
 * register 0xC00/0xC01.  This index will define the interrupt that it should
 *  use. Putting PIRQ_A into the PIN A index for a device will tell that
 * device to use PIC IRQ 10 if it uses PIN A for its hardware INT.
 */
static const struct pirq_struct mainboard_pirq_data[] = {
	{ GFX_DEVFN,	{ PIRQ_A, PIRQ_NC, PIRQ_NC, PIRQ_NC } },
	{ HDA0_DEVFN,	{ PIRQ_NC, PIRQ_HDA, PIRQ_NC, PIRQ_NC } },
	{ PCIE0_DEVFN,	{ PIRQ_A, PIRQ_B, PIRQ_C, PIRQ_D } },
	{ PCIE1_DEVFN,	{ PIRQ_B, PIRQ_C, PIRQ_D, PIRQ_A } },
	{ PCIE2_DEVFN,	{ PIRQ_C, PIRQ_D, PIRQ_A, PIRQ_B } },
	{ PCIE3_DEVFN,	{ PIRQ_D, PIRQ_A, PIRQ_B, PIRQ_C } },
	{ PCIE4_DEVFN,	{ PIRQ_A, PIRQ_B, PIRQ_C, PIRQ_D } },
	{ PSP_DEVFN,	{ PIRQ_A, PIRQ_NC, PIRQ_NC, PIRQ_NC } },
	{ HDA1_DEVFN,	{ PIRQ_HDA, PIRQ_NC, PIRQ_NC, PIRQ_NC } },
	{ SD_DEVFN,	{ PIRQ_SD, PIRQ_NC, PIRQ_NC, PIRQ_NC } },
	{ SMBUS_DEVFN,	{ PIRQ_SMBUS, PIRQ_NC, PIRQ_NC, PIRQ_NC } },
	{ SATA_DEVFN,	{ PIRQ_SATA, PIRQ_NC, PIRQ_NC, PIRQ_NC } },
	{ EHCI1_DEVFN,	{ PIRQ_EHCI, PIRQ_NC, PIRQ_NC, PIRQ_NC } },
	{ XHCI_DEVFN,	{ PIRQ_XHCI, PIRQ_NC, PIRQ_NC, PIRQ_NC } },
};

/* PIRQ Setup */
static void pirq_setup(void)
{
	pirq_data_ptr = mainboard_pirq_data;
	pirq_data_size = ARRAY_SIZE(mainboard_pirq_data);
}

static void mainboard_init(void *chip_info)
{
	size_t num_gpios;
	const struct soc_amd_gpio *gpios;
	gpios = gpio_table(&num_gpios);
	gpio_configure_pads(gpios, num_gpios);
}

/*************************************************
 * enable the dedicated function in pademelon board.
 *************************************************/
static void mainboard_enable(struct device *dev)
{
	/* Initialize the PIRQ data structures for consumption */
	pirq_setup();
}

struct chip_operations mainboard_ops = {
	.init = mainboard_init,
	.enable_dev = mainboard_enable,
};
