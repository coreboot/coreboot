/*
 * This file is part of the libpayload project.
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

#define USB_DEBUG

#include <libpayload.h>
#include "xhci_private.h"
#include "xhci.h"

typedef struct {
	int numports;
	int *port;
} rh_inst_t;

#define RH_INST(dev) ((rh_inst_t*)(dev)->data)

static void
xhci_rh_enable_port (usbdev_t *dev, int port)
{
	// FIXME: check power situation?
	// enable slot
	// attach device context to slot
	// address device
}

/* disable root hub */
static void
xhci_rh_disable_port (usbdev_t *dev, int port)
{
}

static void
xhci_rh_scanport (usbdev_t *dev, int port)
{
	// clear CSC
	int val = XHCI_INST (dev->controller)->opreg->prs[port].portsc;
	val &= PORTSC_RW_MASK;
	val |= PORTSC_CSC;
	XHCI_INST (dev->controller)->opreg->prs[port].portsc = val;

	debug("device attach status on port %x: %x\n", port, XHCI_INST (dev->controller)->opreg->prs[port].portsc & PORTSC_CCS);
}

static int
xhci_rh_report_port_changes (usbdev_t *dev)
{
	int i;
	// no change
	if (!(XHCI_INST (dev->controller)->opreg->usbsts & USBSTS_PCD))
		return -1;

	for (i = 0; i < RH_INST (dev)->numports; i++) {
		if (XHCI_INST (dev->controller)->opreg->prs[i].portsc & PORTSC_CSC) {
			debug("found connect status change on port %d\n", i);
			return i;
		}
	}

	return -1; // shouldn't ever happen
}

static void
xhci_rh_destroy (usbdev_t *dev)
{
	int i;
	for (i = 0; i < RH_INST (dev)->numports; i++)
		xhci_rh_disable_port (dev, i);
	free (RH_INST (dev));
}

static void
xhci_rh_poll (usbdev_t *dev)
{
	int port;
	while ((port = xhci_rh_report_port_changes (dev)) != -1)
		xhci_rh_scanport (dev, port);
}

void
xhci_rh_init (usbdev_t *dev)
{
	int i;

	dev->destroy = xhci_rh_destroy;
	dev->poll = xhci_rh_poll;

	dev->data = malloc (sizeof (rh_inst_t));
	if (!dev->data)
		usb_fatal ("Not enough memory for XHCI RH.\n");

	RH_INST (dev)->numports = XHCI_INST (dev->controller)->capreg->MaxPorts;
	RH_INST (dev)->port = malloc(sizeof(int) * RH_INST (dev)->numports);
	debug("%d ports registered\n", RH_INST (dev)->numports);

	for (i = 0; i < RH_INST (dev)->numports; i++) {
		xhci_rh_enable_port (dev, i);
		RH_INST (dev)->port[i] = -1;
	}

	/* we can set them here because a root hub _really_ shouldn't
	   appear elsewhere */
	dev->address = 0;
	dev->hub = -1;
	dev->port = -1;

	debug("rh init done\n");
}
