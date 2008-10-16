/*
 * This file is part of the libpayload project.
 *
 * Copyright (C) 2008 coresystems GmbH
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
#include "uhci.h"

typedef struct {
	int port[2];
} rh_inst_t;

#define RH_INST(dev) ((rh_inst_t*)(dev)->data)

static void
uhci_rh_enable_port (usbdev_t *dev, int port)
{
	u16 value;
	hci_t *controller = dev->controller;
	if (port == 1)
		port = PORTSC1;
	else
		port = PORTSC2;
	uhci_reg_mask16 (controller, port, ~(1 << 12), 0);	/* wakeup */

	uhci_reg_mask16 (controller, port, ~0, 1 << 9);	/* reset */
	mdelay (30);		// >10ms
	uhci_reg_mask16 (controller, port, ~(1 << 9), 0);
	mdelay (1);		// >5.3us per spec, <3ms because some devices make trouble

	uhci_reg_mask16 (controller, port, ~0, 1 << 2);	/* enable */
	do {
		value = uhci_reg_read16 (controller, port);
		mdelay (1);
	} while (((value & (1 << 2)) == 0) && (value & 0x01));
}

/* disable root hub */
static void
uhci_rh_disable_port (usbdev_t *dev, int port)
{
	hci_t *controller = dev->controller;
	port = PORTSC2;
	if (port == 1)
		port = PORTSC1;
	uhci_reg_mask16 (controller, port, ~4, 0);
	int value;
	do {
		value = uhci_reg_read16 (controller, port);
		mdelay (1);
	} while ((value & (1 << 2)) != 0);
}

static void
uhci_rh_scanport (usbdev_t *dev, int port)
{
	int portsc, offset;
	if (port == 1) {
		portsc = PORTSC1;
		offset = 0;
	} else if (port == 2) {
		portsc = PORTSC2;
		offset = 1;
	} else
		return;
	int devno = RH_INST (dev)->port[offset];
	if ((dev->controller->devices[devno] != 0) && (devno != -1)) {
		usb_detach_device(dev->controller, devno);
		RH_INST (dev)->port[offset] = -1;
	}
	uhci_reg_mask16 (dev->controller, portsc, ~0, (1 << 3) | (1 << 2));	// clear port state change, enable port

	if ((uhci_reg_read16 (dev->controller, portsc) & 1) != 0) {
		// device attached

		uhci_rh_disable_port (dev, port);
		uhci_rh_enable_port (dev, port);

		int lowspeed =
			(uhci_reg_read16 (dev->controller, portsc) >> 8) & 1;

		RH_INST (dev)->port[offset] = usb_attach_device(dev->controller, dev->address, portsc, lowspeed);
	}
}

static int
uhci_rh_report_port_changes (usbdev_t *dev)
{
	int stored, real;

	stored = (RH_INST (dev)->port[0] == -1);
	real = ((uhci_reg_read16 (dev->controller, PORTSC1) & 1) == 0);
	if (stored != real)
		return 1;

	stored = (RH_INST (dev)->port[1] == -1);
	real = ((uhci_reg_read16 (dev->controller, PORTSC2) & 1) == 0);
	if (stored != real)
		return 2;

// maybe detach+attach happened between two scans?
	if ((uhci_reg_read16 (dev->controller, PORTSC1) & 2) > 0)
		return 1;
	if ((uhci_reg_read16 (dev->controller, PORTSC2) & 2) > 0)
		return 2;

// no change
	return -1;
}

static void
uhci_rh_destroy (usbdev_t *dev)
{
	uhci_rh_disable_port (dev, 1);
	uhci_rh_disable_port (dev, 2);
	free (RH_INST (dev));
}

static void
uhci_rh_poll (usbdev_t *dev)
{
	int port;
	while ((port = uhci_rh_report_port_changes (dev)) != -1)
		uhci_rh_scanport (dev, port);
}

void
uhci_rh_init (usbdev_t *dev)
{
	dev->destroy = uhci_rh_destroy;
	dev->poll = uhci_rh_poll;

	uhci_rh_enable_port (dev, 1);
	uhci_rh_enable_port (dev, 2);
	dev->data = malloc (sizeof (rh_inst_t));
	RH_INST (dev)->port[0] = -1;
	RH_INST (dev)->port[1] = -1;

	/* we can set them here because a root hub _really_ shouldn't
	   appear elsewhere */
	dev->address = 0;
	dev->hub = -1;
	dev->port = -1;
}
