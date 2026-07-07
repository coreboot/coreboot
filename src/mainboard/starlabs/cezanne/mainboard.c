/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/acpi.h>
#include <amdblocks/amd_pci_util.h>
#include <amdblocks/acpimmio.h>
#include <acpi/acpi.h>
#include <arch/smp/mpspec.h>
#include <commonlib/helpers.h>
#include <device/device.h>

#define CEZANNE_GPIO_IRQ 11

/* The IRQ mapping in fch_irq_map ends up getting written to the indirect address space that is
   accessed via I/O ports 0xc00/0xc01. */

/*
 * This controls the device -> IRQ routing.
 *
 * Hardcoded IRQs:
 *  0: timer < soc/amd/common/acpi/lpc.asl
 *  1: i8042 - Keyboard
 *  2: cascade
 *  8: rtc0 <- soc/amd/common/acpi/lpc.asl
 *  9: acpi <- soc/amd/common/acpi/lpc.asl
 */
static const struct fch_irq_routing fch_irq_map[] = {
	{ PIRQ_A,	15,		PIRQ_NC		},
	{ PIRQ_B,	5,		PIRQ_NC		},
	{ PIRQ_C,	10,		PIRQ_NC		},
	{ PIRQ_D,	11,		PIRQ_NC		},
	{ PIRQ_E,	15,		PIRQ_NC		},
	{ PIRQ_F,	5,		PIRQ_NC		},
	{ PIRQ_G,	10,		PIRQ_NC		},
	{ PIRQ_H,	11,		PIRQ_NC		},

	{ PIRQ_SCI,	ACPI_SCI_IRQ,	ACPI_SCI_IRQ	},
	{ PIRQ_SD,	PIRQ_NC,	PIRQ_NC		},
	{ PIRQ_SDIO,	PIRQ_NC,	PIRQ_NC		},
	{ PIRQ_SATA,	PIRQ_NC,	PIRQ_NC		},
	{ PIRQ_EMMC,	PIRQ_NC,	PIRQ_NC		},
	{ PIRQ_GPIO,	CEZANNE_GPIO_IRQ, CEZANNE_GPIO_IRQ	},
	{ PIRQ_I2C0,	10,		10		},
	{ PIRQ_I2C1,	7,		7		},
	{ PIRQ_I2C2,	6,		6		},
	{ PIRQ_I2C3,	5,		5		},
	{ PIRQ_UART0,	4,		4		},
	{ PIRQ_UART1,	3,		3		},

	// The MISC registers are not interrupt numbers
	{ PIRQ_MISC,	0xfa,		0x00		},
	{ PIRQ_MISC0,	0x91,		0x00		},
	{ PIRQ_HPET_L,	0x00,		0x00		},
	{ PIRQ_HPET_H,	0x00,		0x00		},
};

const struct fch_irq_routing *mb_get_fch_irq_mapping(size_t *length)
{
	*length = ARRAY_SIZE(fch_irq_map);
	return fch_irq_map;
}

unsigned long mainboard_write_madt_irq_overrides(unsigned long current)
{
	/* FCH PIRQ routing maps the AMD GPIO controller to IRQ 11. */
	current += acpi_create_madt_irqoverride((void *)current, MP_BUS_ISA,
						CEZANNE_GPIO_IRQ, CEZANNE_GPIO_IRQ,
						MP_IRQ_TRIGGER_LEVEL | MP_IRQ_POLARITY_LOW);

	return current;
}

struct chip_operations mainboard_ops = {};
