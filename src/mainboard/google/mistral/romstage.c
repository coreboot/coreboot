/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/stages.h>
#include <soc/usb.h>

static void prepare_usb(void)
{
	/*
	 * Do DWC3 core and phy reset. Kick these resets off early
	 * so they get at least 1msec to settle.
	 */
	reset_usb(HSUSB_HS_PORT_1);
}

void platform_romstage_main(void)
{
	prepare_usb();
}
