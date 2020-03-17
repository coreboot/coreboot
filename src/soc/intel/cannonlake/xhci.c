/*
 * This file is part of the coreboot project.
 *
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

#include <intelblocks/xhci.h>

#define XHCI_USB2_PORT_STATUS_REG	0x480
#define XHCI_USB3_PORT_STATUS_REG	0x580
#define XHCI_USB2_PORT_NUM		14
#define XHCI_USB3_PORT_NUM		10

static const struct xhci_usb_info usb_info = {
	.usb2_port_status_reg = XHCI_USB2_PORT_STATUS_REG,
	.num_usb2_ports = XHCI_USB2_PORT_NUM,
	.usb3_port_status_reg = XHCI_USB3_PORT_STATUS_REG,
	.num_usb3_ports = XHCI_USB3_PORT_NUM,
};

const struct xhci_usb_info *soc_get_xhci_usb_info(void)
{
	return &usb_info;
}
