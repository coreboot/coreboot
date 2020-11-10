/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/pci_type.h>
#include <intelblocks/xhci.h>
#include <soc/pci_devs.h>

/*
 * Information obtained from Intel doc# 630094, ADL-P PCH EDS Vol. 2,
 * as well as doc# 626817, ADL-P PCH EDS Vol. 1
 */

#define PCH_XHCI_USB2_PORT_STATUS_REG	0x480
#define PCH_XHCI_USB3_PORT_STATUS_REG	0x540
#define PCH_XHCI_USB2_PORT_NUM		10
#define PCH_XHCI_USB3_PORT_NUM		4

#define TCSS_XHCI_USB2_PORT_STATUS_REG	0x480
#define TCSS_XHCI_USB3_PORT_STATUS_REG	0x540
#define TCSS_XHCI_USB2_PORT_NUM		10
#define TCSS_XHCI_USB3_PORT_NUM		4

static const struct xhci_usb_info usb_info = {
	.usb2_port_status_reg = PCH_XHCI_USB2_PORT_STATUS_REG,
	.num_usb2_ports = PCH_XHCI_USB2_PORT_NUM,
	.usb3_port_status_reg = PCH_XHCI_USB3_PORT_STATUS_REG,
	.num_usb3_ports = PCH_XHCI_USB3_PORT_NUM,
};

static const struct xhci_usb_info tcss_usb_info = {
	.usb2_port_status_reg = TCSS_XHCI_USB2_PORT_STATUS_REG,
	.num_usb2_ports = TCSS_XHCI_USB2_PORT_NUM,
	.usb3_port_status_reg = TCSS_XHCI_USB3_PORT_STATUS_REG,
	.num_usb3_ports = TCSS_XHCI_USB3_PORT_NUM,
};

const struct xhci_usb_info *soc_get_xhci_usb_info(pci_devfn_t xhci_dev)
{
	if (xhci_dev == PCH_DEVFN_XHCI)
		return &usb_info;
	else if (xhci_dev == SA_DEVFN_TCSS_XHCI)
		return &tcss_usb_info;

	return NULL;
}
