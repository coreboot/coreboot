/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008-2009 coresystems GmbH
 * Copyright 2014 Google Inc.
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

#include <arch/io.h>
#include <console/console.h>
#include <cpu/x86/smm.h>
#include <soc/pm.h>
#include <soc/smm.h>
#include <elog.h>
#include <ec/google/chromeec/ec.h>
#include <soc/gpio.h>
#include <soc/iomap.h>
#include <soc/nvs.h>
#include <soc/pm.h>
#include <soc/smm.h>
#include <superio/ite/it8772f/it8772f.h>
#include "onboard.h"


int mainboard_io_trap_handler(int smif)
{
	switch (smif) {
	case 0x99:
		printk(BIOS_DEBUG, "Sample\n");
		smm_get_gnvs()->smif = 0;
		break;
	default:
		return 0;
	}

	/* On success, the IO Trap Handler returns 0
	 * On failure, the IO Trap Handler returns a value != 0
	 *
	 * For now, we force the return value to 0 and log all traps to
	 * see what's going on.
	 */
	return 1;
}

/* gpi_sts is GPIO 47:32 */
void mainboard_smi_gpi(u32 gpi_sts)
{
}

void mainboard_smi_sleep(u8 slp_typ)
{
	/* Disable USB charging if required */
	switch (slp_typ) {
	case 3:
		it8772f_gpio_led(IT8772F_GPIO_DEV, 1 /* set */, 0x01 /* select */,
			0x01 /* polarity */, 0x01 /* 1=pullup */,
			0x01 /* output */, 0x00, /* 0=Alternate function */
			SIO_GPIO_BLINK_GPIO10, IT8772F_GPIO_BLINK_FREQUENCY_1_HZ);
		break;
	case 5:
		it8772f_gpio_led(IT8772F_GPIO_DEV, 1 /* set */, 0x01 /* select */,
			0x00 /* polarity: non-inverting */, 0x00 /* 0=pulldown */,
			0x01 /* output */, 0x01 /* 1=Simple IO function */,
			SIO_GPIO_BLINK_GPIO10, IT8772F_GPIO_BLINK_FREQUENCY_1_HZ);
		break;
	default:
		break;
	}
	return;
}

int mainboard_smi_apmc(u8 apmc)
{
	switch (apmc) {
	case APM_CNT_ACPI_ENABLE:
		break;
	case APM_CNT_ACPI_DISABLE:
		break;
	}
	return 0;
}
