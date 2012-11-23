/*
 * This file is part of the libpayload project.
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

#include <usb/usb.h>

// assume that host_to_device is overwritten if necessary
#define DR_PORT gen_bmRequestType(host_to_device, class_type, other_recp)
/* status bits */
#define PORT_CONNECTION 0x1
#define PORT_ENABLE 0x2
#define PORT_RESET 0x10
/* status change bits */
#define C_PORT_CONNECTION 0x1
/* feature selectors (for setting / clearing features) */
#define SEL_PORT_RESET 0x4
#define SEL_PORT_POWER 0x8
#define SEL_C_PORT_CONNECTION 0x10

typedef struct {
	int num_ports;
	int *ports;
	hub_descriptor_t *descriptor;
} usbhub_inst_t;

#define HUB_INST(dev) ((usbhub_inst_t*)(dev)->data)

static void
usb_hub_destroy (usbdev_t *dev)
{
	int i;

	/* First, detach all devices behind this hub. */
	int *const ports = HUB_INST (dev)->ports;
	for (i = 1; i <= HUB_INST (dev)->num_ports; i++) {
		if (ports[i] != -1) {
			usb_detach_device(dev->controller, ports[i]);
			ports[i] = -1;
		}
	}

	free (HUB_INST (dev)->ports);
	free (HUB_INST (dev)->descriptor);
	free (HUB_INST (dev));
}

static void
usb_hub_scanport (usbdev_t *dev, int port)
{
	int timeout;

	unsigned short buf[2];
	get_status (dev, port, DR_PORT, 4, buf);
	if (!(buf[1] & C_PORT_CONNECTION))
		/* no change */
		return;

	/* clear Port Connection status change */
	clear_feature (dev, port, SEL_C_PORT_CONNECTION, DR_PORT);

	int devno = HUB_INST (dev)->ports[port];
	if (devno != -1) {
		/* detach device, either because of re-/ or disconnect */
		usb_detach_device(dev->controller, devno);
		HUB_INST (dev)->ports[port] = -1;
	}

	if (!(buf[0] & PORT_CONNECTION))
		/* no device connected, nothing to do */
		return;

	/* wait 100ms for port to become stable */
	mdelay (100); // usb20 spec 9.1.2

	/* reset port */
	set_feature (dev, port, SEL_PORT_RESET, DR_PORT);
	/* wait at least 10ms (usb2.0 spec 11.5.1.5: 10ms to 20ms) */
	mdelay (10);
	/* wait for hub to finish reset */
	timeout = 30; /* time out after 10ms (spec) + 20ms (kindly) */
	do {
		get_status (dev, port, DR_PORT, 4, buf);
		mdelay(1); timeout--;
	} while ((buf[0] & PORT_RESET) && timeout);
	if (!timeout)
		usb_debug("Warning: usbhub: port reset timed out.\n");

	/* wait for port to be enabled. the hub is responsible for this */
	timeout = 500; /* time out after 500ms */
	do {
		get_status (dev, port, DR_PORT, 4, buf);
		mdelay(1); timeout--;
	} while (!(buf[0] & PORT_ENABLE) && timeout);
	if (!timeout)
		usb_debug("Warning: usbhub: port enabling timed out.\n");

	get_status (dev, port, DR_PORT, 4, buf);
	/* bit  10  9
	 *      0   0  full speed
	 *      0   1  low speed
	 *      1   0  high speed
	 */
	int speed = ((buf[0] >> 9) & 3) ;

	HUB_INST (dev)->ports[port] = usb_attach_device(dev->controller, dev->address, port, speed);

	/* clear Port Connection status change */
	clear_feature (dev, port, SEL_C_PORT_CONNECTION, DR_PORT);
}

static int
usb_hub_report_port_changes (usbdev_t *dev)
{
	int port;
	unsigned short buf[2];
	for (port = 1; port <= HUB_INST (dev)->num_ports; port++) {
		get_status (dev, port, DR_PORT, 4, buf);
		if (buf[1] & C_PORT_CONNECTION)
			return port;
	}

	// no change
	return -1;
}

static void
usb_hub_enable_port (usbdev_t *dev, int port)
{
	set_feature (dev, port, SEL_PORT_POWER, DR_PORT);
	mdelay (20);
}

#if 0
static void
usb_hub_disable_port (usbdev_t *dev, int port)
{
}
#endif

static void
usb_hub_poll (usbdev_t *dev)
{
	int port;
	while ((port = usb_hub_report_port_changes (dev)) != -1)
		usb_hub_scanport (dev, port);
}

void
usb_hub_init (usbdev_t *dev)
{
	int i;
	dev->destroy = usb_hub_destroy;
	dev->poll = usb_hub_poll;

	dev->data = malloc (sizeof (usbhub_inst_t));

	if (!dev->data)
		fatal("Not enough memory for USB hub.\n");

	HUB_INST (dev)->descriptor = (hub_descriptor_t *) get_descriptor(dev,
		gen_bmRequestType(device_to_host, class_type, dev_recp), 0x29, 0, 0);
	HUB_INST (dev)->num_ports = HUB_INST (dev)->descriptor->bNbrPorts;
	HUB_INST (dev)->ports =
		malloc (sizeof (int) * (HUB_INST (dev)->num_ports + 1));
	if (! HUB_INST (dev)->ports)
		fatal("Not enough memory for USB hub ports.\n");

	for (i = 1; i <= HUB_INST (dev)->num_ports; i++)
		HUB_INST (dev)->ports[i] = -1;
	for (i = 1; i <= HUB_INST (dev)->num_ports; i++)
		usb_hub_enable_port (dev, i);
}
