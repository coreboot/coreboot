/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <amdblocks/agesawrapper.h>
#include <amdblocks/amd_pci_util.h>
#include <soc/gpio.h>
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

static void mainboard_init(void *chip_info)
{
	size_t num_gpios;
	const struct soc_amd_gpio *gpios;
	gpios = gpio_table(&num_gpios);
	gpio_configure_pads(gpios, num_gpios);
}

struct chip_operations mainboard_ops = {
	.init = mainboard_init,
};
