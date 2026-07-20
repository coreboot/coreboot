/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/acpi.h>
#include <amdblocks/amd_pci_util.h>
#include <commonlib/helpers.h>
#include <device/device.h>
#include <types.h>

/* IRQ routing for Rave2 (AMD Ryzen Embedded P100) - platform-standard
 * assignments for this SoC family */
static const struct fch_irq_routing fch_irq_map[] = {
	{ PIRQ_A,	12,		PIRQ_NC },
	{ PIRQ_B,	14,		PIRQ_NC },
	{ PIRQ_C,	15,		PIRQ_NC },
	{ PIRQ_D,	12,		PIRQ_NC },
	{ PIRQ_E,	14,		PIRQ_NC },
	{ PIRQ_F,	15,		PIRQ_NC },
	{ PIRQ_G,	12,		PIRQ_NC },
	{ PIRQ_H,	14,		PIRQ_NC },

	{ PIRQ_SCI,	ACPI_SCI_IRQ,	ACPI_SCI_IRQ },
	{ PIRQ_SDIO,	PIRQ_NC,	PIRQ_NC },
	{ PIRQ_GPIO,	11,		11 },
	{ PIRQ_I2C0,	10,		10 },
	{ PIRQ_I2C1,	 7,		 7 },
	{ PIRQ_I2C2,	 6,		 6 },
	{ PIRQ_I2C3,	 5,		 5 },
	{ PIRQ_UART0,	 4,		 4 },
	{ PIRQ_UART1,	 3,		 3 },

	/* The MISC registers are not interrupt numbers */
	{ PIRQ_MISC,	0xfa,		0x00 },
	{ PIRQ_MISC0,	0x91,		0x00 },
	{ PIRQ_HPET_L,	0x00,		0x00 },
	{ PIRQ_HPET_H,	0x00,		0x00 },
};

const struct fch_irq_routing *mb_get_fch_irq_mapping(size_t *length)
{
	*length = ARRAY_SIZE(fch_irq_map);
	return fch_irq_map;
}

struct chip_operations mainboard_ops = {
};
