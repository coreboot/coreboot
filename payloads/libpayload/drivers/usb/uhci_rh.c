/*
 *
 * Copyright (C) 2008-2010 coresystems GmbH
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
#include "uhci.h"
#include "uhci_private.h"

typedef struct {
	int port[2];
} rh_inst_t;

#define RH_INST(dev) ((rh_inst_t*)(dev)->data)

static void
uhci_rh_enable_port(usbdev_t *dev, int port)
{
	u16 value;
	hci_t *controller = dev->controller;
	if (port == 1)
		port = PORTSC1;
	else if (port == 2)
		port = PORTSC2;
	else {
		usb_debug("Invalid port %d\n", port);
		return;
	}

	uhci_reg_write16(controller, port,
			 uhci_reg_read16(controller, port) & ~(1 << 12));	/* wakeup */

	uhci_reg_write16(controller, port,
			 uhci_reg_read16(controller, port) | 1 << 9);	/* reset */
	mdelay(30);		// >10ms
	uhci_reg_write16(controller, port,
			 uhci_reg_read16(controller, port) & ~(1 << 9));
	mdelay(1);		// >5.3us per spec, <3ms because some devices make trouble

	uhci_reg_write16(controller, port,
			 uhci_reg_read16(controller, port) | 1 << 2);	/* enable */
	/* wait for controller to enable port */
	/* TOTEST: how long to wait? 100ms for now */
	int timeout = 200; /* time out after 200 * 500us == 100ms */
	do {
		value = uhci_reg_read16(controller, port);
		udelay(500); timeout--;
	} while (((value & (1 << 2)) == 0) && (value & 0x01) && timeout);
	if (!timeout)
		usb_debug("Warning: uhci_rh: port enabling timed out.\n");
}

/* disable root hub */
static void
uhci_rh_disable_port(usbdev_t *dev, int port)
{
	hci_t *controller = dev->controller;
	if (port == 1)
		port = PORTSC1;
	else if (port == 2)
		port = PORTSC2;
	else {
		usb_debug("Invalid port %d\n", port);
		return;
	}
	uhci_reg_write16(controller, port,
			 uhci_reg_read16(controller, port) & ~4);
	u16 value;
	/* wait for controller to disable port */
	/* TOTEST: how long to wait? 100ms for now */
	int timeout = 200; /* time out after 200 * 500us == 100ms */
	do {
		value = uhci_reg_read16(controller, port);
		udelay(500); timeout--;
	} while (((value & (1 << 2)) != 0) && timeout);
	if (!timeout)
		usb_debug("Warning: uhci_rh: port disabling timed out.\n");
}

static void
uhci_rh_scanport(usbdev_t *dev, int port)
{
	int portsc, offset;
	if (port == 1) {
		portsc = PORTSC1;
		offset = 0;
	} else if (port == 2) {
		portsc = PORTSC2;
		offset = 1;
	} else {
		usb_debug("Invalid port %d\n", port);
		return;
	}
	int devno = RH_INST(dev)->port[offset];
	if ((devno != -1) && (dev->controller->devices[devno] != 0)) {
		usb_detach_device(dev->controller, devno);
		RH_INST(dev)->port[offset] = -1;
	}
	uhci_reg_write16(dev->controller, portsc,
			 uhci_reg_read16(dev->controller, portsc) | (1 << 3) | (1 << 2));	// clear port state change, enable port

	mdelay(100); // wait for signal to stabilize

	if ((uhci_reg_read16(dev->controller, portsc) & 1) != 0) {
		// device attached

		uhci_rh_disable_port(dev, port);
		uhci_rh_enable_port(dev, port);

		usb_speed speed = ((uhci_reg_read16(dev->controller, portsc) >> 8) & 1);

		RH_INST(dev)->port[offset] = usb_attach_device(dev->controller, dev->address, portsc, speed);
	}
}

static int
uhci_rh_report_port_changes(usbdev_t *dev)
{
	u16 stored, real;

	stored = (RH_INST(dev)->port[0] == -1);
	real = ((uhci_reg_read16(dev->controller, PORTSC1) & 1) == 0);
	if (stored != real) {
		usb_debug("change on port 1\n");
		return 1;
	}

	stored = (RH_INST(dev)->port[1] == -1);
	real = ((uhci_reg_read16(dev->controller, PORTSC2) & 1) == 0);
	if (stored != real) {
		usb_debug("change on port 2\n");
		return 2;
	}

	// maybe detach+attach happened between two scans?

	if ((uhci_reg_read16(dev->controller, PORTSC1) & 2) > 0) {
		usb_debug("possibly re-attached on port 1\n");
		return 1;
	}
	if ((uhci_reg_read16(dev->controller, PORTSC2) & 2) > 0) {
		usb_debug("possibly re-attached on port 2\n");
		return 2;
	}

	// no change
	return -1;
}

static void
uhci_rh_destroy(usbdev_t *dev)
{
	usb_detach_device(dev->controller, 1);
	usb_detach_device(dev->controller, 2);
	uhci_rh_disable_port(dev, 1);
	uhci_rh_disable_port(dev, 2);
	free(RH_INST(dev));
}

static void
uhci_rh_poll(usbdev_t *dev)
{
	int port;
	while ((port = uhci_rh_report_port_changes(dev)) != -1)
		uhci_rh_scanport(dev, port);
}

void
uhci_rh_init(usbdev_t *dev)
{
	dev->destroy = uhci_rh_destroy;
	dev->poll = uhci_rh_poll;

	uhci_rh_enable_port(dev, 1);
	uhci_rh_enable_port(dev, 2);
	dev->data = xmalloc(sizeof(rh_inst_t));

	RH_INST(dev)->port[0] = -1;
	RH_INST(dev)->port[1] = -1;

	/* we can set them here because a root hub _really_ shouldn't
	   appear elsewhere */
	dev->address = 0;
	dev->hub = -1;
	dev->port = -1;
}
