/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/addressmap.h>
#include <soc/usb.h>

void setup_usb_secondary_host(void)
{
	/* We always consider USB2 port as the secondary UBS host regardless of the
	   register naming */
	update_usb_base_regs(SSUSB_IPPC_BASE_P0, SSUSB_SIF_BASE_P0);
	setup_usb_host_controller();
}
