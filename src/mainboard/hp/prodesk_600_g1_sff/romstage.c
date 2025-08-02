/* SPDX-License-Identifier: GPL-2.0-only */

#include <southbridge/intel/lynxpoint/pch.h>

void mainboard_config_rcba(void)
{
}

const struct usb2_port_config mainboard_usb2_ports[MAX_USB2_PORTS] = {
	/* FIXME: Length and Location are computed from IOBP values, may be inaccurate */
	/* Length, Enable, OCn#, Location */
	{ 0x0040, 1, 0, USB_PORT_FLEX },
	{ 0x0040, 1, 0, USB_PORT_FLEX },
	{ 0x0110, 1, 0, USB_PORT_BACK_PANEL },
	{ 0x0040, 1, 0, USB_PORT_BACK_PANEL },
	{ 0x0040, 1, 3, USB_PORT_FLEX },
	{ 0x0040, 1, 3, USB_PORT_FLEX },
	{ 0x0040, 1, 3, USB_PORT_BACK_PANEL },
	{ 0x0040, 1, 3, USB_PORT_BACK_PANEL },
	{ 0x0040, 1, 5, USB_PORT_BACK_PANEL },
	{ 0x0040, 1, 5, USB_PORT_BACK_PANEL },
	{ 0x0040, 1, 5, USB_PORT_FLEX },
	{ 0x0040, 1, 5, USB_PORT_FLEX },
	{ 0x0110, 1, 7, USB_PORT_BACK_PANEL },
	{ 0x0110, 1, 7, USB_PORT_BACK_PANEL },
};

const struct usb3_port_config mainboard_usb3_ports[MAX_USB3_PORTS] = {
	{ 1, 0 },
	{ 1, 0 },
	{ 0, 3 },
	{ 0, 3 },
	{ 1, 5 },
	{ 1, 5 },
};
