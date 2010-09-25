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
	volatile portsc_t *p = &(RH_INST(dev)->ports[port]);

	printf("giving up port %x, it's USB1\n", port+1);

	/* Lowspeed device. Hand over to companion */
	p->port_owner = 1;
	do {} while (!p->conn_status_change);
	/* RW/C register, so clear it by writing 1 */
	p->conn_status_change = 1;
	return;
}

static void
ehci_rh_scanport (usbdev_t *dev, int port)
{
	volatile portsc_t *p = &(RH_INST(dev)->ports[port]);
	if (RH_INST(dev)->devices[port]!=-1) {
		printf("Unregister device at port %x\n", port+1);
		usb_detach_device(dev->controller, RH_INST(dev)->devices[port]);
		RH_INST(dev)->devices[port]=-1;
	}
	/* device connected, handle */
	if (p->current_conn_status) {
		mdelay(100);
		if (p->line_status == 0x1) {
			ehci_rh_hand_over_port(dev, port);
			return;
		}
		p->port_enable = 0;
		p->port_reset = 1;
		mdelay(50);
		p->port_reset = 0;
		/* Wait for flag change to finish. The controller might take a while */
		while (p->port_reset) ;
		if (!p->port_enable) {
			ehci_rh_hand_over_port(dev, port);
			return;
		}
		printf("port %x hosts a USB2 device\n", port+1);
		RH_INST(dev)->devices[port] = usb_attach_device(dev->controller, dev->address, port, 2);
	}
	/* RW/C register, so clear it by writing 1 */
	p->conn_status_change = 1;
}

static int
ehci_rh_report_port_changes (usbdev_t *dev)
{
	int i;
	for (i=0; i<RH_INST(dev)->n_ports; i++) {
		if (RH_INST(dev)->ports[i].conn_status_change)
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

	RH_INST(dev)->n_ports = EHCI_INST(dev->controller)->capabilities->n_ports;
	RH_INST(dev)->ports = EHCI_INST(dev->controller)->operation->portsc;

	printf("root hub has %x ports\n", RH_INST(dev)->n_ports);

	RH_INST(dev)->devices = malloc(RH_INST(dev)->n_ports * sizeof(int));
	for (i=0; i < RH_INST(dev)->n_ports; i++) {
		RH_INST(dev)->devices[i] = -1;
		RH_INST(dev)->ports[i].pp = 1;
	}

	dev->address = 0;
	dev->hub = -1;
	dev->port = -1;
}
