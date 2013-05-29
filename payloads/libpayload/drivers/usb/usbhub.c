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

#include <usb/usb.h>
#include "generic_hub.h"

/* assume that host_to_device is overwritten if necessary */
#define DR_PORT gen_bmRequestType(host_to_device, class_type, other_recp)
/* status (and status change) bits */
#define PORT_CONNECTION 0x1
#define PORT_ENABLE 0x2
#define PORT_RESET 0x10
/* feature selectors (for setting / clearing features) */
#define SEL_PORT_RESET 0x4
#define SEL_PORT_POWER 0x8
#define SEL_C_PORT_CONNECTION 0x10

static int
usb_hub_port_status_changed(usbdev_t *const dev, const int port)
{
	unsigned short buf[2] = { 0, 0 };
	get_status (dev, port, DR_PORT, 4, buf);
	clear_feature (dev, port, SEL_C_PORT_CONNECTION, DR_PORT);
	return buf[1] & PORT_CONNECTION;
}

static int
usb_hub_port_connected(usbdev_t *const dev, const int port)
{
	unsigned short buf[2] = { 0, 0 };
	get_status (dev, port, DR_PORT, 4, buf);
	return buf[0] & PORT_CONNECTION;
}

static int
usb_hub_port_in_reset(usbdev_t *const dev, const int port)
{
	unsigned short buf[2] = { 0, 0 };
	get_status (dev, port, DR_PORT, 4, buf);
	return buf[0] & PORT_RESET;
}

static int
usb_hub_port_enabled(usbdev_t *const dev, const int port)
{
	unsigned short buf[2] = { 0, 0 };
	get_status (dev, port, DR_PORT, 4, buf);
	return (buf[0] & PORT_ENABLE) != 0;
}

static int
usb_hub_port_speed(usbdev_t *const dev, const int port)
{
	unsigned short buf[2] = { 0, 0 };
	get_status (dev, port, DR_PORT, 4, buf);
	if (buf[0] & PORT_ENABLE) {
		/* bit  10  9
		 *      0   0  full speed
		 *      0   1  low speed
		 *      1   0  high speed
		 */
		return (buf[0] >> 9) & 0x3;
	} else {
		return -1;
	}
}

static int
usb_hub_enable_port(usbdev_t *const dev, const int port)
{
	set_feature(dev, port, SEL_PORT_POWER, DR_PORT);
	return 0;
}

static int
usb_hub_start_port_reset(usbdev_t *const dev, const int port)
{
	set_feature (dev, port, SEL_PORT_RESET, DR_PORT);
	return 0;
}

static const generic_hub_ops_t usb_hub_ops = {
	.hub_status_changed	= NULL,
	.port_status_changed	= usb_hub_port_status_changed,
	.port_connected		= usb_hub_port_connected,
	.port_in_reset		= usb_hub_port_in_reset,
	.port_enabled		= usb_hub_port_enabled,
	.port_speed		= usb_hub_port_speed,
	.enable_port		= usb_hub_enable_port,
	.disable_port		= NULL,
	.start_port_reset	= usb_hub_start_port_reset,
	.reset_port		= generic_hub_resetport,
};

void
usb_hub_init(usbdev_t *const dev)
{
	hub_descriptor_t *const descriptor = (hub_descriptor_t *)
		get_descriptor(
			dev,
			gen_bmRequestType(device_to_host, class_type, dev_recp),
			0x29, 0, 0);
	if (!descriptor) {
		usb_debug("usbhub: ERROR: Failed to fetch hub descriptor\n");
		return;
	}
	const int num_ports = descriptor->bNbrPorts;
	free(descriptor);

	generic_hub_init(dev, num_ports, &usb_hub_ops);
}
