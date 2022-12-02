/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <console/console.h>
#include <cpu/x86/smm.h>
#include <southbridge/intel/bd82x6x/pch.h>
#include <southbridge/intel/bd82x6x/me.h>
#include <northbridge/intel/sandybridge/sandybridge.h>

/* Include for SIO helper functions */
#include <superio/ite/it8772f/it8772f.h>
#define SUPERIO_DEV PNP_DEV(0x2e, 0)

/*
 * Change LED_POWER# (SIO GPIO 45) state based on sleep type.
*/
void mainboard_smi_sleep(u8 slp_typ)
{
	printk(BIOS_DEBUG, "SMI: sleep S%d\n", slp_typ);
	switch (slp_typ) {
	case ACPI_S3:
	case ACPI_S4:
		it8772f_gpio_led(SUPERIO_DEV, 4 /* set */, (0x1 << 5) /* select */,
			(0x1 << 5) /* polarity */, (0x1 << 5) /* 1 = pullup */,
			(0x1 << 5) /* output */, 0x00, /* 0 = Alternate function */
			SIO_GPIO_BLINK_GPIO45, IT8772F_GPIO_BLINK_FREQUENCY_1_HZ);
		break;

	case ACPI_S5:
		it8772f_gpio_led(SUPERIO_DEV, 4 /* set */, (0x1 << 5) /* select */,
			0x00 /* polarity: non-inverting */, 0x00 /* 0 = pulldown */,
			(0x1 << 5) /* output */, (0x1 << 5) /* 1 = Simple IO function */,
			SIO_GPIO_BLINK_GPIO45, IT8772F_GPIO_BLINK_FREQUENCY_1_HZ);
		break;
	default:
		break;
	}
}
