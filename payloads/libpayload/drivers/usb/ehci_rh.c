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
	volatile u32 tmp;

	printf("giving up port %x, it's USB1\n", port+1);

	/* Lowspeed device. Hand over to companion */
	tmp = p->val;
	tmp |= PORT_OWNER;
	p->val = tmp;
	while (!(p->val & PORT_CSC));
	/* RW/C register, so clear it by writing 1 */
	tmp = p->val;
	tmp |= PORT_CSC;
	p->val = tmp;
	return;
}

static void
ehci_rh_scanport (usbdev_t *dev, int port)
{
	volatile portsc_t *p = &(RH_INST(dev)->ports[port]);
	volatile u32 tmp;
	if (RH_INST(dev)->devices[port]!=-1) {
		printf("Unregister device at port %x\n", port+1);
		usb_detach_device(dev->controller, RH_INST(dev)->devices[port]);
		RH_INST(dev)->devices[port]=-1;
	}
	/* device connected, handle */
	if (p->val & PORT_CONNECT) {
		mdelay(100);
		if ((p->val & PORT_LINESTATUS_MASK) == PORT_LINESTATUS_KSTATE) {
			ehci_rh_hand_over_port(dev, port);
			return;
		}

		/* Remove port_enable, set port_reset.  This must be atomic. */
		tmp = p->val;
		tmp &= ~PORT_PE;
		tmp |= PORT_RESET;
		p->val = tmp;

		mdelay(50);
		tmp &= ~PORT_RESET;
		p->val = tmp;

		/* Wait for flag change to finish. The controller might take a while */
		while(p->val & PORT_RESET) ;
		if (!(p->val & PORT_PE)) {
			ehci_rh_hand_over_port(dev, port);
			return;
		}
		printf("port %x hosts a USB2 device\n", port+1);
		RH_INST(dev)->devices[port] = usb_attach_device(dev->controller, dev->address, port, 2);
	}

	/* RW/C register, so clear it by writing 1 */
	tmp = p->val;
	tmp |= PORT_CSC;
	p->val = tmp;
}

static int
ehci_rh_report_port_changes (usbdev_t *dev)
{
	volatile portsc_t *p;
	int i;
	for (i=0; i<RH_INST(dev)->n_ports; i++) {
	       	p = &(RH_INST(dev)->ports[i]);
		if (p->val & PORT_CSC)
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
	volatile portsc_t *p;
	volatile u32 tmp;
	int i;

	dev->destroy = ehci_rh_destroy;
	dev->poll = ehci_rh_poll;

	dev->data = malloc(sizeof(rh_inst_t));

	RH_INST(dev)->n_ports = EHCI_INST(dev->controller)->capabilities->n_ports;
	RH_INST(dev)->ports = EHCI_INST(dev->controller)->operation->portsc;

	printf("root hub has %x ports\n", RH_INST(dev)->n_ports);

	RH_INST(dev)->devices = malloc(RH_INST(dev)->n_ports * sizeof(int));
	for (i=0; i < RH_INST(dev)->n_ports; i++) {
		p = &(RH_INST(dev)->ports[i]);
		tmp = p->val;
		tmp |= PORT_POWER;
		p->val = tmp;
		RH_INST(dev)->devices[i] = -1;
	}

	dev->address = 0;
	dev->hub = -1;
	dev->port = -1;
}
