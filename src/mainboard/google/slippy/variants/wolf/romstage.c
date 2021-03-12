/* SPDX-License-Identifier: GPL-2.0-only */

#include <string.h>
#include <northbridge/intel/haswell/raminit.h>
#include <southbridge/intel/lynxpoint/pch.h>
#include <southbridge/intel/lynxpoint/lp_gpio.h>
#include "../../variant.h"

unsigned int variant_get_spd_index(void)
{
	const int gpio_vector[] = {13, 9, 47, -1};
	return get_gpios(gpio_vector);
}

bool variant_is_dual_channel(const unsigned int spd_index)
{
	/* Index 0-2 are 4GB config with both CH0 and CH1
	   Index 3-5 are 2GB config with CH0 only */
	switch (spd_index) {
	case 0: case 1: case 2:
		return true;
	case 3: case 4: case 5:
	default:
		return false;
	}
}

const struct usb2_port_config mainboard_usb2_ports[MAX_USB2_PORTS] = {
	/* Length, Enable, OCn#, Location */
	{ 0x0040, 1, 0,               /* P0: Port A, CN10 */
	  USB_PORT_BACK_PANEL },
	{ 0x0040, 1, 2,               /* P1: Port B, CN11  */
	  USB_PORT_BACK_PANEL },
	{ 0x0080, 1, USB_OC_PIN_SKIP, /* P2: CCD */
	  USB_PORT_INTERNAL },
	{ 0x0040, 1, USB_OC_PIN_SKIP, /* P3: BT */
	  USB_PORT_MINI_PCIE },
	{ 0x0040, 1, USB_OC_PIN_SKIP, /* P4: LTE */
	  USB_PORT_INTERNAL },
	{ 0x0000, 1, USB_OC_PIN_SKIP, /* P5: EMPTY */
	  USB_PORT_SKIP },
	{ 0x0040, 1, USB_OC_PIN_SKIP, /* P6: SD Card */
	  USB_PORT_INTERNAL },
	{ 0x0000, 0, USB_OC_PIN_SKIP, /* P7: EMPTY */
	  USB_PORT_SKIP  },
};

const struct usb3_port_config mainboard_usb3_ports[MAX_USB3_PORTS] = {
	/* Enable, OCn# */
	{ 1, 0               }, /* P1; Port A, CN10 */
	{ 1, 2               }, /* P2; Port B, CN11  */
	{ 0, USB_OC_PIN_SKIP }, /* P3; */
	{ 0, USB_OC_PIN_SKIP }, /* P4; */
};
