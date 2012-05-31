/*
 * This file is part of the libpayload project.
 *
 * Copyright (C) 2010 coresystems GmbH
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

#include <libpayload.h>
#include "ehci.h"
#include "ehci_private.h"

typedef struct {
	int n_ports;
	/* typical C, n_ports is the number
	 * of ports, while ports[] spans [0,n_ports-1],
	 * even though the spec counts from 1.
	 */
	volatile portsc_t *ports;
	int *devices;
} rh_inst_t;

#define RH_INST(dev) ((rh_inst_t*)(dev)->data)

static void
ehci_rh_destroy (usbdev_t *dev)
{
        free (RH_INST(dev)->devices);
        free (RH_INST(dev));
}

static void
ehci_rh_hand_over_port (usbdev_t *dev, int port)
{
	debug("giving up port %x, it's USB1\n", port+1);

	/* Clear ConnectStatusChange before evaluation */
	/* RW/C register, so clear it by writing 1 */
	RH_INST(dev)->ports[port] |= P_CONN_STATUS_CHANGE;

	/* Lowspeed device. Hand over to companion */
	RH_INST(dev)->ports[port] |= P_PORT_OWNER;

	/* TOTEST: how long to wait? trying 100ms for now */
	int timeout = 10; /* timeout after 10 * 10ms == 100ms */
	while (!(RH_INST(dev)->ports[port] & P_CONN_STATUS_CHANGE) && timeout--)
		mdelay(10);
	if (!(RH_INST(dev)->ports[port] & P_CONN_STATUS_CHANGE)) {
		debug("Warning: Handing port over to companion timed out.\n");
	}

	/* RW/C register, so clear it by writing 1 */
	RH_INST(dev)->ports[port] |= P_CONN_STATUS_CHANGE;
	return;
}

static void
ehci_rh_scanport (usbdev_t *dev, int port)
{
	if (RH_INST(dev)->devices[port]!=-1) {
		debug("Unregister device at port %x\n", port+1);
		usb_detach_device(dev->controller, RH_INST(dev)->devices[port]);
		RH_INST(dev)->devices[port]=-1;
	}
	/* device connected, handle */
	if (RH_INST(dev)->ports[port] & P_CURR_CONN_STATUS) {
		mdelay(100); // usb20 spec 9.1.2
		if ((RH_INST(dev)->ports[port] & P_LINE_STATUS) == P_LINE_STATUS_LOWSPEED) {
			ehci_rh_hand_over_port(dev, port);
			return;
		}

		/* Deassert enable, assert reset.  These must change
		 * atomically.
		 */
		RH_INST(dev)->ports[port] = (RH_INST(dev)->ports[port] & ~P_PORT_ENABLE) | P_PORT_RESET;

		/* Wait a bit while reset is active. */
		mdelay(50); // usb20 spec 7.1.7.5 (TDRSTR)

		/* Deassert reset. */
		RH_INST(dev)->ports[port] &= ~P_PORT_RESET;

		/* Wait max. 2ms (ehci spec 2.3.9) for flag change to finish. */
		int timeout = 20; /* time out after 20 * 100us == 2ms */
		while ((RH_INST(dev)->ports[port] & P_PORT_RESET) && timeout--)
			udelay(100);
		if (RH_INST(dev)->ports[port] & P_PORT_RESET) {
			printf("Error: ehci_rh: port reset timed out.\n");
			return;
		}

		/* If the host controller enabled the port, it's a high-speed
		 * device, otherwise it's full-speed.
		 */
		if (!(RH_INST(dev)->ports[port] & P_PORT_ENABLE)) {
			ehci_rh_hand_over_port(dev, port);
			return;
		}
		debug("port %x hosts a USB2 device\n", port+1);
		RH_INST(dev)->devices[port] = usb_attach_device(dev->controller, dev->address, port, 2);
	}
	/* RW/C register, so clear it by writing 1 */
	RH_INST(dev)->ports[port] |= P_CONN_STATUS_CHANGE;
}

static int
ehci_rh_report_port_changes (usbdev_t *dev)
{
	int i;
	for (i=0; i<RH_INST(dev)->n_ports; i++) {
		if (RH_INST(dev)->ports[i] & P_CONN_STATUS_CHANGE)
			return i;
	}
	return -1;
}

static void
ehci_rh_poll (usbdev_t *dev)
{
	int port;
	while ((port = ehci_rh_report_port_changes (dev)) != -1)
		ehci_rh_scanport (dev, port);
}


void
ehci_rh_init (usbdev_t *dev)
{
	int i;
	dev->destroy = ehci_rh_destroy;
	dev->poll = ehci_rh_poll;

	dev->data = malloc(sizeof(rh_inst_t));

	RH_INST(dev)->n_ports = EHCI_INST(dev->controller)->capabilities->hcsparams & HCS_NPORTS_MASK;
	RH_INST(dev)->ports = EHCI_INST(dev->controller)->operation->portsc;

	debug("root hub has %x ports\n", RH_INST(dev)->n_ports);

	/* If the host controller has port power control, enable power on
	 * all ports and wait 20ms.
	 */
	if (EHCI_INST(dev->controller)->capabilities->hcsparams
			& HCS_PORT_POWER_CONTROL) {
		debug("host controller has port power control, "
				"giving power to all ports.\n");
		for (i=0; i < RH_INST(dev)->n_ports; i++)
			RH_INST(dev)->ports[i] |= P_PP;
	}
	mdelay(20); // ehci spec 2.3.9

	RH_INST(dev)->devices = malloc(RH_INST(dev)->n_ports * sizeof(int));
	for (i=0; i < RH_INST(dev)->n_ports; i++) {
		RH_INST(dev)->devices[i] = -1;
		ehci_rh_scanport(dev, i);
	}

	dev->address = 0;
	dev->hub = -1;
	dev->port = -1;
}
