/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <arch/stages.h>
#include <soc/usb.h>

static void prepare_usb(void)
{
	/*
	 * Do DWC3 core and phy reset. Kick these resets off early
	 * so they get atleast 1msec to settle.
	 */
	reset_usb(HSUSB_HS_PORT_1);
}

void platform_romstage_main(void)
{
	prepare_usb();
}
