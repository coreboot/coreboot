/* SPDX-License-Identifier: GPL-2.0-only */

#include <intelblocks/xhci.h>

#define XHCI_USB2_PORT_STATUS_REG	0x480
#if CONFIG(SOC_INTEL_GEMINILAKE)
#define XHCI_USB3_PORT_STATUS_REG	0x510
#define XHCI_USB2_PORT_NUM		9
#else
#define XHCI_USB3_PORT_STATUS_REG	0x500
#define XHCI_USB2_PORT_NUM		8
#endif
#define XHCI_USB3_PORT_NUM		7

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
