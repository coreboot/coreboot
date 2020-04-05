/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <soc/ramstage.h>

void board_silicon_USB2_override(SILICON_INIT_UPD *params)
{
	if (SocStepping() >= SocD0) {

		//Follow Intel recommendation to set
		//BSW D-stepping PERPORTRXISET 2 (low strength)
		params->D0Usb2Port0PerPortRXISet = 2;
		params->D0Usb2Port1PerPortRXISet = 2;
		params->D0Usb2Port2PerPortRXISet = 2;
		params->D0Usb2Port3PerPortRXISet = 2;
		params->D0Usb2Port4PerPortRXISet = 2;
	}
}
