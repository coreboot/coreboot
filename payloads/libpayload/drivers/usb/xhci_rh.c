/*
 * This file is part of the libpayload project.
 *
 * Copyright (C) 2013 secunet Security Networks AG
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#define USB_DEBUG

#include <usb/usb.h>
#include "generic_hub.h"
#include "xhci_private.h"
#include "xhci.h"

static int
xhci_rh_hub_status_changed(usbdev_t *const dev)
{
	xhci_t *const xhci = XHCI_INST(dev->controller);
	const int changed = !!(xhci->opreg->usbsts & USBSTS_PCD);
	if (changed)
		xhci->opreg->usbsts =
			(xhci->opreg->usbsts & USBSTS_PRSRV_MASK) | USBSTS_PCD;
	return changed;
}

static int
xhci_rh_port_status_changed(usbdev_t *const dev, const int port)
{
	xhci_t *const xhci = XHCI_INST(dev->controller);
	volatile u32 *const portsc = &xhci->opreg->prs[port - 1].portsc;

	const int changed = !!(*portsc & PORTSC_CSC);
	/* always clear all the status change bits */
	*portsc = (*portsc & PORTSC_RW_MASK) | 0x00ef0000;
	return changed;
}

static int
xhci_rh_port_connected(usbdev_t *const dev, const int port)
{
	xhci_t *const xhci = XHCI_INST(dev->controller);
	volatile u32 *const portsc = &xhci->opreg->prs[port - 1].portsc;

	return *portsc & PORTSC_CCS;
}

static int
xhci_rh_port_in_reset(usbdev_t *const dev, const int port)
{
	xhci_t *const xhci = XHCI_INST(dev->controller);
	volatile u32 *const portsc = &xhci->opreg->prs[port - 1].portsc;

	return !!(*portsc & PORTSC_PR);
}

static int
xhci_rh_port_enabled(usbdev_t *const dev, const int port)
{
	xhci_t *const xhci = XHCI_INST(dev->controller);
	volatile u32 *const portsc = &xhci->opreg->prs[port - 1].portsc;

	return !!(*portsc & PORTSC_PED);
}

static int
xhci_rh_port_speed(usbdev_t *const dev, const int port)
{
	xhci_t *const xhci = XHCI_INST(dev->controller);
	volatile u32 *const portsc = &xhci->opreg->prs[port - 1].portsc;

	if (*portsc & PORTSC_PED) {
		return ((*portsc & PORTSC_PORT_SPEED_MASK)
				>> PORTSC_PORT_SPEED_START)
			- 1;
	} else {
		return -1;
	}
}

static int
xhci_rh_start_port_reset(usbdev_t *const dev, const int port)
{
	xhci_t *const xhci = XHCI_INST(dev->controller);
	volatile u32 *const portsc = &xhci->opreg->prs[port - 1].portsc;

	*portsc = (*portsc & PORTSC_RW_MASK) | PORTSC_PR;
	return 0;
}

static const generic_hub_ops_t xhci_rh_ops = {
	.hub_status_changed	= xhci_rh_hub_status_changed,
	.port_status_changed	= xhci_rh_port_status_changed,
	.port_connected		= xhci_rh_port_connected,
	.port_in_reset		= xhci_rh_port_in_reset,
	.port_enabled		= xhci_rh_port_enabled,
	.port_speed		= xhci_rh_port_speed,
	.enable_port		= NULL,
	.disable_port		= NULL,
	.start_port_reset	= xhci_rh_start_port_reset,
	.reset_port		= generic_hub_rh_resetport,
};

void
xhci_rh_init (usbdev_t *dev)
{
	/* we can set them here because a root hub _really_ shouldn't
	   appear elsewhere */
	dev->address = 0;
	dev->hub = -1;
	dev->port = -1;

	const int num_ports = /* TODO: maybe we need to read extended caps */
		(XHCI_INST(dev->controller)->capreg->hcsparams1 >> 24) & 0xff;
	generic_hub_init(dev, num_ports, &xhci_rh_ops);

	usb_debug("xHCI: root hub init done\n");
}
