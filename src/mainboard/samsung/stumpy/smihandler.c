/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008-2009 coresystems GmbH
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <arch/acpi.h>
#include <arch/io.h>
#include <console/console.h>
#include <cpu/x86/smm.h>
#include <southbridge/intel/bd82x6x/nvs.h>
#include <southbridge/intel/bd82x6x/pch.h>
#include <southbridge/intel/bd82x6x/me.h>
#include <northbridge/intel/sandybridge/sandybridge.h>
#include <cpu/intel/model_206ax/model_206ax.h>

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
