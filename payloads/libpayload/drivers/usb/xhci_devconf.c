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

//#define XHCI_SPEW_DEBUG

#include <arch/virtual.h>
#include "xhci_private.h"

static u32
xhci_gen_route(xhci_t *const xhci, const int hubport, const int hubaddr)
{
	if (!hubaddr)
		return 0;
	volatile const devctx_t *const devctx =
		phys_to_virt(xhci->dcbaa[hubaddr]);
	u32 route_string = SC_GET(ROUTE, devctx->slot);
	int i;
	for (i = 0; i < 20; i += 4) {
		if (!(route_string & (0xf << i))) {
			route_string |= (hubport & 0xf) << i;
			break;
		}
	}
	return route_string;
}

static int
xhci_get_rh_port(xhci_t *const xhci, const int hubport, const int hubaddr)
{
	if (!hubaddr)
		return hubport;
	volatile const devctx_t *const devctx =
		phys_to_virt(xhci->dcbaa[hubaddr]);
	return SC_GET(RHPORT, devctx->slot);
}

static int
xhci_get_tt(xhci_t *const xhci, const int xhci_speed,
	    const int hubport, const int hubaddr,
	    int *const tt, int *const tt_port)
{
	if (!hubaddr)
		return 0;
	volatile const devctx_t *const devctx =
		phys_to_virt(xhci->dcbaa[hubaddr]);
	if ((*tt = SC_GET(TTID, devctx->slot))) {
		*tt_port = SC_GET(TTPORT, devctx->slot);
	} else if (xhci_speed < XHCI_HIGH_SPEED &&
			SC_GET(SPEED, devctx->slot) == XHCI_HIGH_SPEED) {
		*tt = hubaddr;
		*tt_port = hubport;
	}
	return *tt != 0;
}

static long
xhci_decode_mps0(const int xhci_speed, const u8 b_mps)
{
	switch (xhci_speed) {
	case XHCI_LOW_SPEED:
	case XHCI_FULL_SPEED:
	case XHCI_HIGH_SPEED:
		switch (b_mps) {
		case 8: case 16: case 32: case 64:
			return b_mps;
		default:
			xhci_debug("Invalid MPS0: 0x%02x\n", b_mps);
			return 8;
		}
		break;
	case XHCI_SUPER_SPEED:
		if (b_mps == 9) {
			return 2 << b_mps;
		} else {
			xhci_debug("Invalid MPS0: 0x%02x\n", b_mps);
			return 2 << 9;
		}
		break;
	default:
		xhci_debug("Invalid speed for MPS0: %d\n", xhci_speed);
		return 8;
	}
}


static long
xhci_get_mps0(usbdev_t *const dev, const int xhci_speed)
{
	u8 buf[8];
	dev_req_t dr = {
		.bmRequestType	= gen_bmRequestType(
					device_to_host, standard_type, dev_recp),
		.data_dir	= device_to_host,
		.bRequest	= GET_DESCRIPTOR,
		.wValue		= (1 << 8),
		.wIndex		= 0,
		.wLength	= 8,
	};
	if (dev->controller->control(dev, IN, sizeof(dr), &dr, 8, buf)) {
		xhci_debug("Failed to read MPS0\n");
		return COMMUNICATION_ERROR;
	} else {
		return xhci_decode_mps0(xhci_speed, buf[7]);
	}
}

int
xhci_set_address (hci_t *controller, int speed, int hubport, int hubaddr)
{
	xhci_t *const xhci = XHCI_INST(controller);
	const int xhci_speed = speed + 1;

	int ret = -1;

	inputctx_t *const ic = xhci_align(64, sizeof(*ic));
	devinfo_t *const di = memalign(sizeof(di->devctx), sizeof(*di));
	transfer_ring_t *const tr = malloc(sizeof(*tr));
	if (tr)
		tr->ring = xhci_align(16, TRANSFER_RING_SIZE * sizeof(trb_t));
	if (!ic || !di || !tr || !tr->ring) {
		xhci_debug("Out of memory\n");
		goto _free_return;
	}

	int slot_id;
	int cc = xhci_cmd_enable_slot(xhci, &slot_id);
	if (cc != CC_SUCCESS) {
		xhci_debug("Enable slot failed: %d\n", cc);
		goto _free_return;
	} else {
		xhci_debug("Enabled slot %d\n", slot_id);
	}

	memset(ic, 0x00, sizeof(*ic));
	ic->control.add = (1 << 0) /* Slot Context */ |
			  (1 << 1) /* EP0 Context */ ;

	SC_SET(ROUTE,	ic->dev.slot, xhci_gen_route(xhci, hubport, hubaddr));
	SC_SET(SPEED,	ic->dev.slot, xhci_speed);
	SC_SET(CTXENT,	ic->dev.slot, 1); /* the endpoint 0 context */
	SC_SET(RHPORT,	ic->dev.slot, xhci_get_rh_port(xhci, hubport, hubaddr));

	int tt, tt_port;
	if (xhci_get_tt(xhci, xhci_speed, hubport, hubaddr, &tt, &tt_port)) {
		xhci_debug("TT for %d: %d[%d]\n", slot_id, tt, tt_port);
		volatile const devctx_t *const ttctx =
			phys_to_virt(xhci->dcbaa[tt]);
		SC_SET(MTT, ic->dev.slot, SC_GET(MTT, ttctx->slot));
		SC_SET(TTID, ic->dev.slot, tt);
		SC_SET(TTPORT, ic->dev.slot, tt_port);
	}

	memset(di, 0x00, sizeof(*di));
	di->transfer_rings[1] = tr;
	xhci_init_cycle_ring(tr, TRANSFER_RING_SIZE);

	ic->dev.ep0.tr_dq_low	= virt_to_phys(tr->ring);
	ic->dev.ep0.tr_dq_high	= 0;
	EC_SET(TYPE,	ic->dev.ep0, EP_CONTROL);
	EC_SET(AVRTRB,	ic->dev.ep0, 8);
	EC_SET(MPS,	ic->dev.ep0, 8);
	EC_SET(CERR,	ic->dev.ep0, 3);
	EC_SET(DCS,	ic->dev.ep0, 1);

	volatile devctx_t *const oc = &di->devctx;
	xhci->dcbaa[slot_id] = virt_to_phys(oc);

	cc = xhci_cmd_address_device(xhci, slot_id, ic);
	if (cc != CC_SUCCESS) {
		xhci_debug("Address device failed: %d\n", cc);
		goto _disable_return;
	} else {
		xhci_debug("Addressed device %d (USB: %d)\n",
			  slot_id, SC_GET(UADDR, oc->slot));
	}
	mdelay(2); /* SetAddress() recovery interval (usb20 spec 9.2.6.3) */

	init_device_entry(controller, slot_id);
	controller->devices[slot_id]->address = slot_id;

	const long mps0 = xhci_get_mps0(
			controller->devices[slot_id], xhci_speed);
	if (mps0 < 0) {
		goto _disable_return;
	} else if (mps0 != 8) {
		memset(&ic->control, 0x00, sizeof(ic->control));
		memset(&ic->dev.ep0, 0x00, sizeof(ic->dev.ep0));
		ic->control.add = (1 << 1); /* EP0 Context */
		EC_SET(MPS, ic->dev.ep0, mps0);
		cc = xhci_cmd_evaluate_context(xhci, slot_id, ic);
		if (cc != CC_SUCCESS) {
			xhci_debug("Context evaluation failed: %d\n", cc);
			goto _disable_return;
		} else {
			xhci_debug("Set MPS0 to %dB\n", mps0);
		}
	}

	ret = slot_id;
	goto _free_ic_return;

_disable_return:
	xhci_cmd_disable_slot(xhci, slot_id);
	xhci->dcbaa[slot_id] = 0;
_free_return:
	if (tr)
		free((void *)tr->ring);
	free(tr);
	free((void *)di);
_free_ic_return:
	free(ic);
	return ret;
}

static int
xhci_finish_hub_config(usbdev_t *const dev, inputctx_t *const ic)
{
	hub_descriptor_t *const descriptor = (hub_descriptor_t *)
		get_descriptor(
			dev,
			gen_bmRequestType(device_to_host, class_type, dev_recp),
			0x29, 0, 0);
	if (!descriptor) {
		xhci_debug("Failed to fetch hub descriptor\n");
		return COMMUNICATION_ERROR;
	}

	SC_SET(HUB,	ic->dev.slot, 1);
	SC_SET(MTT,	ic->dev.slot, 0); /* No support for Multi-TT */
	SC_SET(NPORTS,	ic->dev.slot, descriptor->bNbrPorts);
	if (dev->speed == HIGH_SPEED)
		SC_SET(TTT, ic->dev.slot,
		       (descriptor->wHubCharacteristics >> 5) & 0x0003);

	free(descriptor);
	return 0;
}

static size_t
xhci_bound_interval(const endpoint_t *const ep)
{
	if (	(ep->dev->speed == LOW_SPEED &&
			(ep->type == ISOCHRONOUS ||
			 ep->type == INTERRUPT)) ||
		(ep->dev->speed == FULL_SPEED &&
			 ep->type == INTERRUPT))
	{
		if (ep->interval < 3)
			return 3;
		else if (ep->interval > 11)
			return 11;
		else
			return ep->interval;
	} else {
		if (ep->interval < 0)
			return 0;
		else if (ep->interval > 15)
			return 15;
		else
			return ep->interval;
	}
}

static int
xhci_finish_ep_config(const endpoint_t *const ep, inputctx_t *const ic)
{
	xhci_t *const xhci = XHCI_INST(ep->dev->controller);
	devinfo_t *const di = phys_to_virt(xhci->dcbaa[ep->dev->address]
					   - offsetof(devinfo_t, devctx));
	const int ep_id = xhci_ep_id(ep);
	xhci_debug("ep_id: %d\n", ep_id);
	if (ep_id <= 1 || 32 <= ep_id)
		return DRIVER_ERROR;

	transfer_ring_t *const tr = malloc(sizeof(*tr));
	if (tr)
		tr->ring = xhci_align(16, TRANSFER_RING_SIZE * sizeof(trb_t));
	if (!tr || !tr->ring) {
		free(tr);
		xhci_debug("Out of memory\n");
		return OUT_OF_MEMORY;
	}
	di->transfer_rings[ep_id] = tr;
	xhci_init_cycle_ring(tr, TRANSFER_RING_SIZE);

	ic->control.add |= (1 << ep_id);
	if (SC_GET(CTXENT, ic->dev.slot) < ep_id)
		SC_SET(CTXENT, ic->dev.slot, ep_id);

	epctx_t *const epctx = &ic->dev.eps[ep_id];
	xhci_debug("Filling epctx (@%p)\n", epctx);
	epctx->tr_dq_low	= virt_to_phys(tr->ring);
	epctx->tr_dq_high	= 0;
	EC_SET(INTVAL,	*epctx, xhci_bound_interval(ep));
	EC_SET(CERR,	*epctx, 3);
	EC_SET(TYPE,	*epctx, ep->type | ((ep->direction != OUT) << 2));
	EC_SET(MPS,	*epctx, ep->maxpacketsize);
	EC_SET(DCS,	*epctx, 1);
	size_t avrtrb;
	switch (ep->type) {
		case BULK: case ISOCHRONOUS:	avrtrb = 3 * 1024; break;
		case INTERRUPT:			avrtrb =     1024; break;
		default:			avrtrb =        8; break;
	}
	EC_SET(AVRTRB,	*epctx, avrtrb);
	EC_SET(MXESIT,  *epctx, EC_GET(MPS, *epctx) * EC_GET(MBS, *epctx));

	return 0;
}

int
xhci_finish_device_config(usbdev_t *const dev)
{
	xhci_t *const xhci = XHCI_INST(dev->controller);
	devinfo_t *const di = phys_to_virt(xhci->dcbaa[dev->address]
					   - offsetof(devinfo_t, devctx));

	int i, ret = 0;

	inputctx_t *const ic = xhci_align(64, sizeof(*ic));
	if (!ic) {
		xhci_debug("Out of memory\n");
		return OUT_OF_MEMORY;
	}
	memset(ic, 0x00, sizeof(*ic));

	ic->control.add = (1 << 0); /* Slot Context */

	xhci_dump_slotctx((const slotctx_t *)&di->devctx.slot);
	ic->dev.slot.f1 = di->devctx.slot.f1;
	ic->dev.slot.f2 = di->devctx.slot.f2;
	ic->dev.slot.f3 = di->devctx.slot.f3;

	if (((device_descriptor_t *)dev->descriptor)->bDeviceClass == 0x09) {
		ret = xhci_finish_hub_config(dev, ic);
		if (ret)
			goto _free_return;
	}

	for (i = 1; i < dev->num_endp; ++i) {
		ret = xhci_finish_ep_config(&dev->endpoints[i], ic);
		if (ret)
			goto _free_ep_ctx_return;
	}

	xhci_dump_inputctx(ic);

	const int config_id = ((configuration_descriptor_t *)
				dev->configuration)->bConfigurationValue;
	xhci_debug("config_id: %d\n", config_id);
	const int cc =
		xhci_cmd_configure_endpoint(xhci, dev->address, config_id, ic);
	if (cc != CC_SUCCESS) {
		xhci_debug("Configure endpoint failed: %d\n", cc);
		ret = CONTROLLER_ERROR;
		goto _free_ep_ctx_return;
	} else {
		xhci_debug("Endpoints configured\n");
	}

	goto _free_return;

_free_ep_ctx_return:
	for (i = 2; i < 31; ++i) {
		if (di->transfer_rings[i])
			free((void *)di->transfer_rings[i]->ring);
		free(di->transfer_rings[i]);
		di->transfer_rings[i] = NULL;
	}
_free_return:
	free(ic);
	return ret;
}

void
xhci_destroy_dev(hci_t *const controller, const int slot_id)
{
	xhci_t *const xhci = XHCI_INST(controller);

	if (slot_id <= 0 || xhci->max_slots_en > slot_id)
		return;

	int i;

	const int cc = xhci_cmd_disable_slot(xhci, slot_id);
	if (cc != CC_SUCCESS)
		xhci_debug("Failed to disable slot %d: %d\n", slot_id, cc);

	devinfo_t *const di = DEVINFO_FROM_XHCI(xhci, slot_id);
	for (i = 1; i < 31; ++i) {
		if (di->transfer_rings[i])
			free((void *)di->transfer_rings[i]->ring);
		free(di->transfer_rings[i]);

		free(di->interrupt_queues[i]);
	}
	free(di);
	xhci->dcbaa[slot_id] = 0;
}
