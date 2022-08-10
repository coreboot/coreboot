/*
 *
 * Copyright (C) 2010 Patrick Georgi
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

//#define USB_DEBUG

#include <libpayload.h>
#include "ohci_private.h"
#include "ohci.h"

typedef struct {
	int numports;
	int *port;
} rh_inst_t;

#define RH_INST(dev) ((rh_inst_t*)(dev)->data)

static void
ohci_rh_enable_port(usbdev_t *dev, int port)
{
	/* Reset RH port should hold 50ms with pulses of at least 10ms and
	 * gaps of at most 3ms (usb20 spec 7.1.7.5).
	 * After reset, the port will be enabled automatically (ohci spec
	 * 7.4.4).
	 */
	int total_delay = 100; /* 100 * 500us == 50ms */
	while (total_delay > 0) {
		if (!(OHCI_INST(dev->controller)->opreg->HcRhPortStatus[port]
					& CurrentConnectStatus))
			return;

		/* start reset */
		OHCI_INST(dev->controller)->opreg->HcRhPortStatus[port] =
			SetPortReset;
		int timeout = 200; /* timeout after 200 * 500us == 100ms */
		while ((OHCI_INST(dev->controller)->opreg->HcRhPortStatus[port]
					& PortResetStatus)
				&& timeout--) {
			udelay(500); total_delay--;
		}
		if (OHCI_INST(dev->controller)->opreg->HcRhPortStatus[port]
				& PortResetStatus) {
			usb_debug("Warning: root-hub port reset timed out.\n");
			break;
		}
		if ((200-timeout) < 20)
			usb_debug("Warning: port reset too short: %dms; "
					"should be at least 10ms.\n",
					(200-timeout)/2);
		/* clear reset status change */
		OHCI_INST(dev->controller)->opreg->HcRhPortStatus[port] =
			PortResetStatusChange;
		usb_debug("rh port reset finished after %dms.\n", (200-timeout)/2);
	}
}

/* disable root hub */
static void
ohci_rh_disable_port(usbdev_t *dev, int port)
{
	if (RH_INST(dev)->port[port] != -1) {
		usb_detach_device(dev->controller, RH_INST(dev)->port[port]);
		RH_INST(dev)->port[port] = -1;
	}

	OHCI_INST(dev->controller)->opreg->HcRhPortStatus[port] = ClearPortEnable; // disable port
	int timeout = 50; /* timeout after 50 * 100us == 5ms */
	while ((OHCI_INST(dev->controller)->opreg->HcRhPortStatus[port]
				& PortEnableStatus)
			&& timeout--) {
		udelay(100);
	}
}

static void
ohci_rh_scanport(usbdev_t *dev, int port)
{
	if (port >= RH_INST(dev)->numports) {
		usb_debug("Invalid port %d\n", port);
		return;
	}

	/* device registered, and device change logged, so something must have happened */
	if (RH_INST(dev)->port[port] != -1) {
		usb_detach_device(dev->controller, RH_INST(dev)->port[port]);
		RH_INST(dev)->port[port] = -1;
	}

	/* no device attached
	   previously registered devices are detached, nothing left to do */
	if (!(OHCI_INST(dev->controller)->opreg->HcRhPortStatus[port] & CurrentConnectStatus))
		return;

	OHCI_INST(dev->controller)->opreg->HcRhPortStatus[port] = ConnectStatusChange; // clear port state change
	ohci_rh_enable_port(dev, port);

	mdelay(100); // wait for signal to stabilize

	if (!(OHCI_INST(dev->controller)->opreg->HcRhPortStatus[port] & PortEnableStatus)) {
		usb_debug("port enable failed\n");
		return;
	}

	usb_speed speed = (OHCI_INST(dev->controller)->opreg->HcRhPortStatus[port] & LowSpeedDeviceAttached) != 0;
	RH_INST(dev)->port[port] = usb_attach_device(dev->controller, dev->address, port, speed);
}

static int
ohci_rh_report_port_changes(usbdev_t *dev)
{
	ohci_t *const ohcic = OHCI_INST(dev->controller);

	int i;

	for (i = 0; i < RH_INST(dev)->numports; i++) {
		// maybe detach+attach happened between two scans?
		if (ohcic->opreg->HcRhPortStatus[i] & ConnectStatusChange) {
			ohcic->opreg->HcRhPortStatus[i] = ConnectStatusChange;
			usb_debug("attachment change on port %d\n", i);
			return i;
		}
	}

	// no change
	return -1;
}

static void
ohci_rh_destroy(usbdev_t *dev)
{
	int i;
	for (i = 0; i < RH_INST(dev)->numports; i++)
		ohci_rh_disable_port(dev, i);
	free(RH_INST(dev));
}

static void
ohci_rh_poll(usbdev_t *dev)
{
	ohci_t *const ohcic = OHCI_INST(dev->controller);

	int port;

	/* Check if anything changed. */
	if (!(ohcic->opreg->HcInterruptStatus & RootHubStatusChange))
		return;
	ohcic->opreg->HcInterruptStatus = RootHubStatusChange;
	usb_debug("root hub status change\n");

	/* Scan ports with changed connection status. */
	while ((port = ohci_rh_report_port_changes(dev)) != -1)
		ohci_rh_scanport(dev, port);
}

void
ohci_rh_init(usbdev_t *dev)
{
	int i;

	dev->destroy = ohci_rh_destroy;
	dev->poll = ohci_rh_poll;

	dev->data = xmalloc(sizeof(rh_inst_t));
	RH_INST(dev)->numports = OHCI_INST(dev->controller)->opreg->HcRhDescriptorA & NumberDownstreamPortsMask;
	RH_INST(dev)->port = xmalloc(sizeof(int) * RH_INST(dev)->numports);
	usb_debug("%d ports registered\n", RH_INST(dev)->numports);

	for (i = 0; i < RH_INST(dev)->numports; i++) {
		ohci_rh_enable_port(dev, i);
		RH_INST(dev)->port[i] = -1;
	}

	/* we can set them here because a root hub _really_ shouldn't
	   appear elsewhere */
	dev->address = 0;
	dev->hub = -1;
	dev->port = -1;

	usb_debug("rh init done\n");
}
