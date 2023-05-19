/*
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

#include <stdint.h>
#include <stdbool.h>
#include <usb/usb.h>

#include "generic_hub.h"
#include "uhci_private.h"
#include "uhci.h"

#define PORTSC(p) (PORTSC1 + ((p) - 1) * sizeof(uint16_t))
#define  PORTSC_CCS	(1 << 0)
#define  PORTSC_CSC	(1 << 1)
#define  PORTSC_PED	(1 << 2)
#define  PORTSC_PEDC	(1 << 3)
#define  PORTSC_LSDA	(1 << 8)
#define  PORTSC_PR	(1 << 9)
#define  PORTSC_SUSP	(1 << 12)
#define  PORTSC_SC_BITS (PORTSC_CSC | PORTSC_PEDC)

static int
uhci_rh_port_status_changed(usbdev_t *const dev, const int port)
{
	const u16 portsc = uhci_reg_read16(dev->controller, PORTSC(port));
	const bool changed = portsc & (PORTSC_CSC | PORTSC_PEDC);

	/* always clear all the status change bits */
	uhci_reg_write16(dev->controller, PORTSC(port), portsc);

	return changed;
}

static int
uhci_rh_port_connected(usbdev_t *const dev, const int port)
{
	const u16 portsc = uhci_reg_read16(dev->controller, PORTSC(port));
	return !!(portsc & PORTSC_CCS);
}

static int
uhci_rh_port_enabled(usbdev_t *const dev, const int port)
{
	const u16 portsc = uhci_reg_read16(dev->controller, PORTSC(port));
	return !(portsc & PORTSC_SUSP) && (portsc & PORTSC_PED);
}

static usb_speed
uhci_rh_port_speed(usbdev_t *const dev, const int port)
{
	const u16 portsc = uhci_reg_read16(dev->controller, PORTSC(port));
	const bool low_speed = portsc & PORTSC_LSDA;

	return low_speed ? LOW_SPEED : FULL_SPEED;
}

static int
uhci_rh_enable_port(usbdev_t *const dev, const int port)
{
	const u16 portsc = uhci_reg_read16(dev->controller, PORTSC(port)) & ~PORTSC_SUSP;
	uhci_reg_write16(dev->controller, PORTSC(port), portsc & ~(PORTSC_SC_BITS | PORTSC_SUSP));
	uhci_reg_write16(dev->controller, PORTSC(port), (portsc & ~PORTSC_SC_BITS) | PORTSC_PED);
	return 0;
}

static int
uhci_rh_disable_port(usbdev_t *const dev, const int port)
{
	const u16 portsc = uhci_reg_read16(dev->controller, PORTSC(port));
	uhci_reg_write16(dev->controller, PORTSC(port), portsc & ~(PORTSC_SC_BITS | PORTSC_PED));
	return 0;
}

static int
uhci_rh_reset_port(usbdev_t *const dev, const int port)
{
	const u16 portsc = uhci_reg_read16(dev->controller, PORTSC(port)) & ~PORTSC_PR;

	/* Trigger port reset. */
	uhci_reg_write16(dev->controller, PORTSC(port), portsc | PORTSC_PR);

	/* Wait for 15ms (usb20 spec 11.5.1.5: reset should take 10 to 20ms). */
	mdelay(15);

	/* Clear port reset. */
	uhci_reg_write16(dev->controller, PORTSC(port), portsc & ~PORTSC_PR);

	udelay(10); /* Linux waits this long. */

	/* Enable port. */
	uhci_reg_write16(dev->controller, PORTSC(port), portsc | PORTSC_PED);

	/* Clear status-change bits one last time. */
	uhci_reg_write16(dev->controller, PORTSC(port), portsc | PORTSC_PED | PORTSC_SC_BITS);

	return 0;
}

static const generic_hub_ops_t uhci_rh_ops = {
	.hub_status_changed	= NULL,
	.port_status_changed	= uhci_rh_port_status_changed,
	.port_connected		= uhci_rh_port_connected,
	.port_in_reset		= NULL,
	.port_enabled		= uhci_rh_port_enabled,
	.port_speed		= uhci_rh_port_speed,
	.enable_port		= uhci_rh_enable_port,
	.disable_port		= uhci_rh_disable_port,
	.start_port_reset	= NULL,
	.reset_port		= uhci_rh_reset_port,
};

void
uhci_rh_init(usbdev_t *dev)
{
	/* we can set them here because a root hub _really_ shouldn't appear elsewhere */
	dev->address = 0;
	dev->hub = -1;
	dev->port = -1;

	generic_hub_init(dev, 2, &uhci_rh_ops);

	usb_debug("UHCI: root hub init done\n");
}
