/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Nicola Corna <nicola@corna.info>
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

#include <ec/acpi/ec.h>
#include <pc80/mc146818rtc.h>

#include "h8.h"

enum usb_always_on {
	UAO_OFF = 0,
	UAO_AC_AND_BATTERY,
	UAO_AC_ONLY
};

void h8_usb_always_on(void)
{
	enum usb_always_on val;
	u8 reg;

	if (get_option(&val, "usb_always_on") != CB_SUCCESS)
		val = UAO_OFF;

	if (val == UAO_AC_AND_BATTERY) {
		reg = ec_read(H8_USB_ALWAYS_ON);
		reg &= ~H8_USB_ALWAYS_ON_AC_ONLY;
		reg |= H8_USB_ALWAYS_ON_ENABLE;
		ec_write(H8_USB_ALWAYS_ON, reg);
	} else if (val == UAO_AC_ONLY) {
		reg = ec_read(H8_USB_ALWAYS_ON);
		reg |= H8_USB_ALWAYS_ON_AC_ONLY;
		reg |= H8_USB_ALWAYS_ON_ENABLE;
		ec_write(H8_USB_ALWAYS_ON, reg);
		ec_set_bit(0x2, 3);
	}
}
