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

//#define USB_DEBUG

#include <stdlib.h>
#include <usb/usb.h>
#include "generic_hub.h"

void
generic_hub_destroy(usbdev_t *const dev)
{
	generic_hub_t *const hub = GEN_HUB(dev);
	if (!hub)
		return;

	/* First, detach all devices behind this hub */
	int port;
	for (port = 1; port <= hub->num_ports; ++port) {
		if (hub->ports[port] >= 0) {
			usb_debug("generic_hub: Detachment at port %d\n", port);
			usb_detach_device(dev->controller, hub->ports[port]);
			hub->ports[port] = NO_DEV;
		}
	}

	/* Disable all ports */
	if (hub->ops->disable_port) {
		for (port = 1; port <= hub->num_ports; ++port)
			hub->ops->disable_port(dev, port);
	}

	free(hub->ports);
	free(hub);
}

static int
generic_hub_debounce(usbdev_t *const dev, const int port)
{
	generic_hub_t *const hub = GEN_HUB(dev);

	const int step_ms	= 1;	/* linux uses 25ms, we're busy anyway */
	const int at_least_ms	= 100;	/* 100ms as in usb20 spec 9.1.2 */
	const int timeout_ms	= 1500;	/* linux uses this value */

	int total_ms = 0;
	int stable_ms = 0;
	while (stable_ms < at_least_ms && total_ms < timeout_ms) {
		mdelay(step_ms);

		const int changed = hub->ops->port_status_changed(dev, port);
		const int connected = hub->ops->port_connected(dev, port);
		if (changed < 0 || connected < 0)
			return -1;

		if (!changed && connected) {
			stable_ms += step_ms;
		} else {
			usb_debug("generic_hub: Unstable connection at %d\n",
				  port);
			stable_ms = 0;
		}
		total_ms += step_ms;
	}
	if (total_ms >= timeout_ms)
		usb_debug("generic_hub: Debouncing timed out at %d\n", port);
	return 0; /* ignore timeouts, try to always go on */
}

int
generic_hub_wait_for_port(usbdev_t *const dev, const int port,
			  const int wait_for,
			  int (*const port_op)(usbdev_t *, int),
			  int timeout_steps, const int step_us)
{
	int state;
	do {
		state = port_op(dev, port);
		if (state < 0)
			return -1;
		else if (!!state == wait_for)
			return timeout_steps;
		udelay(step_us);
		--timeout_steps;
	} while (timeout_steps);
	return 0;
}

int
generic_hub_resetport(usbdev_t *const dev, const int port)
{
	generic_hub_t *const hub = GEN_HUB(dev);

	if (hub->ops->start_port_reset(dev, port) < 0)
		return -1;

	/* wait for 10ms (usb20 spec 11.5.1.5: reset should take 10 to 20ms) */
	mdelay(10);

	/* now wait 12ms for the hub to finish the reset */
	const int ret = generic_hub_wait_for_port(
			/* time out after 120 * 100us = 12ms */
			dev, port, 0, hub->ops->port_in_reset, 120, 100);
	if (ret < 0)
		return -1;
	else if (!ret)
		usb_debug("generic_hub: Reset timed out at port %d\n", port);

	return 0; /* ignore timeouts, try to always go on */
}

static int
generic_hub_detach_dev(usbdev_t *const dev, const int port)
{
	generic_hub_t *const hub = GEN_HUB(dev);

	usb_detach_device(dev->controller, hub->ports[port]);
	hub->ports[port] = NO_DEV;

	return 0;
}

static int
generic_hub_attach_dev(usbdev_t *const dev, const int port)
{
	generic_hub_t *const hub = GEN_HUB(dev);

	if (generic_hub_debounce(dev, port) < 0)
		return -1;

	if (hub->ops->reset_port) {
		if (hub->ops->reset_port(dev, port) < 0)
			return -1;

		if (!hub->ops->port_connected(dev, port)) {
			usb_debug(
				"generic_hub: Port %d disconnected after "
				"reset. Possibly upgraded, rescan required.\n",
				port);
			return 0;
		}

		/* after reset the port will be enabled automatically */
		const int ret = generic_hub_wait_for_port(
			/* time out after 1,000 * 10us = 10ms */
			dev, port, 1, hub->ops->port_enabled, 1000, 10);
		if (ret < 0)
			return -1;
		else if (!ret)
			usb_debug("generic_hub: Port %d still "
				  "disabled after 10ms\n", port);
	}

	const usb_speed speed = hub->ops->port_speed(dev, port);
	if (speed >= 0) {
		usb_debug("generic_hub: Success at port %d\n", port);
		if (hub->ops->reset_port)
			mdelay(10); /* Reset recovery time
				       (usb20 spec 7.1.7.5) */
		hub->ports[port] = usb_attach_device(
				dev->controller, dev->address, port, speed);
	}
	return 0;
}

int
generic_hub_scanport(usbdev_t *const dev, const int port)
{
	generic_hub_t *const hub = GEN_HUB(dev);

	if (hub->ports[port] >= 0) {
		usb_debug("generic_hub: Detachment at port %d\n", port);

		const int ret = generic_hub_detach_dev(dev, port);
		if (ret < 0)
			return ret;
	}

	if (hub->ops->port_connected(dev, port)) {
		usb_debug("generic_hub: Attachment at port %d\n", port);

		return generic_hub_attach_dev(dev, port);
	}

	return 0;
}

static void
generic_hub_poll(usbdev_t *const dev)
{
	generic_hub_t *const hub = GEN_HUB(dev);
	if (!hub)
		return;

	if (hub->ops->hub_status_changed &&
			hub->ops->hub_status_changed(dev) != 1)
		return;

	int port;
	for (port = 1; port <= hub->num_ports; ++port) {
		const int ret = hub->ops->port_status_changed(dev, port);
		if (ret < 0) {
			return;
		} else if (ret == 1) {
			usb_debug("generic_hub: Port change at %d\n", port);
			if (generic_hub_scanport(dev, port) < 0)
				return;
		}
	}
}

int
generic_hub_init(usbdev_t *const dev, const int num_ports,
		 const generic_hub_ops_t *const ops)
{
	int port;

	dev->destroy = generic_hub_destroy;
	dev->poll = generic_hub_poll;
	dev->data = malloc(sizeof(generic_hub_t));
	if (!dev->data) {
		usb_debug("generic_hub: ERROR: Out of memory\n");
		return -1;
	}

	generic_hub_t *const hub = GEN_HUB(dev);
	hub->num_ports = num_ports;
	hub->ports = malloc(sizeof(*hub->ports) * (num_ports + 1));
	hub->ops = ops;
	if (!hub->ports) {
		usb_debug("generic_hub: ERROR: Out of memory\n");
		free(dev->data);
		dev->data = NULL;
		return -1;
	}
	for (port = 1; port <= num_ports; ++port)
		hub->ports[port] = NO_DEV;

	/* Enable all ports */
	if (ops->enable_port) {
		for (port = 1; port <= num_ports; ++port)
			ops->enable_port(dev, port);
		/* wait once for all ports */
		mdelay(20);
	}

	return 0;
}
