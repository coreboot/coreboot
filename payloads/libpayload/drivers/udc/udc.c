/*
 * This file is part of the libpayload project.
 *
 * Copyright (C) 2015 Google Inc.
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
#include <arch/cache.h>
#include <assert.h>
#include <endian.h>
#include <queue.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <usb/usb.h>

#include <udc/udc.h>

#ifdef DEBUG
#define debug(x...) printf(x)
#else
#define debug(x...) do {} while (0)
#endif

#define min(a, b) (((a) < (b)) ? (a) : (b))

static unsigned short strings_lang_id = 0;
static unsigned char strings_count = 0;
static const char **strings;

void udc_add_strings(unsigned short lang_id, unsigned char count,
	const char **str)
{
	strings_lang_id = lang_id;
	strings_count = count;
	strings = str;
}

/* determine if an additional zero length packet is necessary for
 * a transfer */
static unsigned int zlp(struct usbdev_ctrl *this, const int epnum,
	const int len, const int explen)
{
	const unsigned int mps = this->ep_mps[epnum][1];

	/* zero length transfers are handled explicitly */
	if (len == 0)
		return 0;
	/* host expects exactly the right amount, so no zlp necessary */
	if (len == explen)
		return 0;
	/* last packet will be short -> host knows that transfer is over */
	if ((len % mps) != 0)
		return 0;

	/* otherwise we need an extra zero length packet */
	return 1;
}

static struct usbdev_configuration *fetch_config(struct usbdev_ctrl *this,
	int id)
{
	struct usbdev_configuration *config;
	SLIST_FOREACH(config, &this->configs, list) {
		debug("checking descriptor %d\n",
			config->descriptor.bConfigurationValue);
		if (config->descriptor.bConfigurationValue == id)
			return config;
	}
	return NULL;
}

static void cease_operation(struct usbdev_ctrl *this)
{
	int i;
	for (i = 1; i < 16; i++) {
		/* disable endpoints */
		this->halt_ep(this, i, 0);
		this->halt_ep(this, i, 1);
	}

}

static void enable_interface(struct usbdev_ctrl *this, int iface_num)
{
	struct usbdev_configuration *config = this->current_config;
	struct usbdev_interface *iface = &config->interfaces[iface_num];

	/* first: shut down all endpoints except EP0 */
	cease_operation(this);

	/* now enable all configured endpoints */
	int epcount = iface->descriptor.bNumEndpoints;
	int i;
	for (i = 0; i < epcount; i++) {
		int ep = iface->eps[i].bEndpointAddress;
		int mps = iface->eps[i].wMaxPacketSize;
		int in_dir = 0;
		if (ep & 0x80) {
			in_dir = 1;
			ep &= 0x7f;
		}
		int ep_type = iface->eps[i].bmAttributes & 0x3;
		this->start_ep(this, ep, in_dir, ep_type, mps);
	}

	this->current_iface = iface;

	// gadget specific configuration
	if (iface->init)
		iface->init(this);
}

/**
 * handle default control transfers on EP 0
 *
 * returns 1 if transfer was handled
 */
static int setup_ep0(struct usbdev_ctrl *this, dev_req_t *dr)
{
	if ((dr->bmRequestType == 0x00) &&
	    (dr->bRequest == SET_ADDRESS)) {
		this->set_address(this, dr->wValue & 0x7f);

		/* status phase IN */
		this->enqueue_packet(this, 0, 1, NULL, 0, 0, 0);
		return 1;
	} else
	if ((dr->bmRequestType == 0x00) &&
	    (dr->bRequest == SET_CONFIGURATION)) {
		struct usbdev_configuration *config =
			fetch_config(this, dr->wValue);

		if (dr->wValue == 0)
			cease_operation(this);

		if (config == NULL) {
			this->stall(this, 0, 0, 1);
			this->stall(this, 0, 1, 1);
			return 1;
		}

		/* status phase IN */
		this->enqueue_packet(this, 0, 1, NULL, 0, 0, 0);

		this->current_config = config;
		this->current_config_id = dr->wValue;

		/* activate first interface */
		enable_interface(this, 0);
		this->initialized = 1;
		return 1;
	} else
	if ((dr->bmRequestType == 0x80) &&
	    (dr->bRequest == GET_CONFIGURATION)) {
		unsigned char *res = dma_malloc(1);
		res[0] = this->current_config_id;

		/* data phase IN */
		this->enqueue_packet(this, 0, 1, res, min(1, dr->wLength),
			0, 1);

		/* status phase OUT */
		this->enqueue_packet(this, 0, 0, NULL, 0, 0, 0);
		return 1;
	} else
	// ENDPOINT_HALT
	if ((dr->bmRequestType == 0x02) && // endpoint
	    (dr->bRequest == CLEAR_FEATURE) &&
	    (dr->wValue == 0)) {
		int ep = dr->wIndex;
		/* clear STALL */
		this->stall(this, ep & 0xf, ep & 0x80, 0);

		/* status phase IN */
		this->enqueue_packet(this, 0, 1, NULL, 0, 0, 0);
		return 1;
	} else
	// ENDPOINT_HALT
	if ((dr->bmRequestType == 0x02) && // endpoint
	    (dr->bRequest == SET_FEATURE) &&
	    (dr->wValue == 0)) {
		int ep = dr->wIndex;
		/* set STALL */
		this->stall(this, ep & 0xf, ep & 0x80, 1);

		/* status phase IN */
		this->enqueue_packet(this, 0, 1, NULL, 0, 0, 0);
		return 1;
	} else
	// DEVICE_REMOTE_WAKEUP
	if ((dr->bmRequestType == 0x00) &&
	    (dr->bRequest == CLEAR_FEATURE) &&
	    (dr->wValue == 1)) {
		this->remote_wakeup = 0;

		/* status phase IN */
		this->enqueue_packet(this, 0, 1, NULL, 0, 0, 0);
		return 1;
	} else
	// DEVICE_REMOTE_WAKEUP
	if ((dr->bmRequestType == 0x00) &&
	    (dr->bRequest == SET_FEATURE) &&
	    (dr->wValue == 1)) {
		this->remote_wakeup = 1;

		/* status phase IN */
		this->enqueue_packet(this, 0, 1, NULL, 0, 0, 0);
		return 1;
	} else
	if ((dr->bmRequestType == 0x82) && // endpoint
	    (dr->bRequest == GET_STATUS)) {
		unsigned char *res = dma_malloc(2);
		int ep = dr->wIndex;
		/* is EP halted? */
		res[0] = this->ep_halted[ep & 0xf][(ep & 0x80) ? 1 : 0];
		res[1] = 0;

		/* data phase IN */
		this->enqueue_packet(this, 0, 1, res,
			min(2, dr->wLength), 0, 1);

		// status phase OUT
		this->enqueue_packet(this, 0, 0, NULL, 0, 0, 0);
		return 1;
	} else
	if ((dr->bmRequestType == 0x80) &&
	    (dr->bRequest == GET_STATUS)) {
		unsigned char *res = dma_malloc(2);
		res[0] = 1; // self powered
		if (this->remote_wakeup)
			res[0] |= 2;

		res[1] = 0;

		/* data phase IN */
		this->enqueue_packet(this, 0, 1, res,
			min(2, dr->wLength), 0, 1);

		// status phase OUT
		this->enqueue_packet(this, 0, 0, NULL, 0, 0, 0);
		return 1;
	} else
	if ((dr->bmRequestType == 0x80) &&
	    (dr->bRequest == GET_DESCRIPTOR) &&
	    ((dr->wValue & 0xff00) == 0x0200)) {
		int i, j;
		/* config descriptor #id
		 * since config = 0 is undefined, but descriptors
		 * should start at 0, add 1 to have them match up.
		 */
		int id = (dr->wValue & 0xff) + 1;
		struct usbdev_configuration *config = fetch_config(this, id);
		if (config == NULL) {
			this->stall(this, 0, 0, 1);
			this->stall(this, 0, 1, 1);
			return 1;
		}
		debug("descriptor found, should be %d bytes\n",
			config->descriptor.wTotalLength);

		uint8_t *data = dma_malloc(config->descriptor.wTotalLength);
		uint8_t *head = data;

		memcpy(head, &config->descriptor,
			sizeof(configuration_descriptor_t));
		head += sizeof(configuration_descriptor_t);

		for (i = 0; i < config->descriptor.bNumInterfaces; i++) {
			memcpy(head, &config->interfaces[i].descriptor,
				sizeof(interface_descriptor_t));
			head += sizeof(interface_descriptor_t);
			for (j = 0;
			     j < config->interfaces[i].descriptor.bNumEndpoints;
			     j++) {
				memcpy(head, &config->interfaces[i].eps[j],
					sizeof(endpoint_descriptor_t));
				head += sizeof(endpoint_descriptor_t);
			}
		}
		int size = config->descriptor.wTotalLength;
		assert((head - data) == config->descriptor.wTotalLength);

		/* data phase IN */
		this->enqueue_packet(this, 0, 1, data,
			min(size, dr->wLength),
			zlp(this, 0, size, dr->wLength), 1);

		/* status phase OUT */
		this->enqueue_packet(this, 0, 0, NULL, 0, 0, 0);
		return 1;
	} else
	if ((dr->bmRequestType == 0x80) &&
	    (dr->bRequest == GET_DESCRIPTOR) &&
	    ((dr->wValue & 0xff00) == 0x0300)) {
		int id = (dr->wValue & 0xff);
		if (id == 0) {
			if (strings_lang_id == 0)
				return 0;

			uint8_t *data = dma_malloc(4);
			data[0] = 0x04; // length
			data[1] = 0x03; // string descriptor
			data[2] = strings_lang_id & 0xff;
			data[3] = strings_lang_id >> 8;
			/* data phase IN */
			this->enqueue_packet(this, 0, 1,
				data,
				min(data[0], dr->wLength),
				zlp(this, 0, data[0], dr->wLength),
				1);

			/* status phase OUT */
			this->enqueue_packet(this, 0, 0, NULL, 0, 0, 0);
		} else {
			if (strings_lang_id == 0)
				return 0;

			int lang = dr->wIndex;
			if (lang != strings_lang_id)
				return 0;

			if (id > strings_count)
				return 0;

			int s_len = strlen(strings[id]);
			int d_len = s_len * 2;

			uint8_t *data = dma_malloc(d_len + 2);
			memset(data, 0, d_len + 2);
			data[0] = d_len + 2; // length
			data[1] = 0x03; // string descriptor
			int i;
			for (i = 0; i < s_len; i++)
				data[i * 2 + 2] = strings[id][i];

			/* data phase IN */
			this->enqueue_packet(this, 0, 1,
				data,
				min(d_len + 2, dr->wLength),
				zlp(this, 0, d_len + 2, dr->wLength),
				1);

			/* status phase OUT */
			this->enqueue_packet(this, 0, 0, NULL, 0, 0, 0);
		}
		return 1;
	} else
	if ((dr->bmRequestType == 0x80) &&
	    (dr->bRequest == GET_DESCRIPTOR) &&
	    ((dr->wValue & 0xff00) == 0x0100)) {
		device_descriptor_t *dd = dma_malloc(sizeof(*dd));
		memcpy(dd, &this->device_descriptor, sizeof(*dd));
		dd->bNumConfigurations = this->config_count;

		/* data phase IN */
		this->enqueue_packet(this, 0, 1, (void *)dd,
			min(sizeof(*dd), dr->wLength),
			zlp(this, 0, sizeof(*dd), dr->wLength), 1);

		/* status phase OUT */
		this->enqueue_packet(this, 0, 0, NULL, 0, 0, 0);
		return 1;
	}
	return 0;
}

void udc_add_gadget(struct usbdev_ctrl *this,
	struct usbdev_configuration *config)
{
	int i, size;
	SLIST_INSERT_HEAD(&this->configs, config, list);

	size = sizeof(configuration_descriptor_t);

	for (i = 0; i < config->descriptor.bNumInterfaces; i++) {
		size += sizeof(config->interfaces[i].descriptor);
		size += config->interfaces[i].descriptor.bNumEndpoints *
			sizeof(endpoint_descriptor_t);
	}
	config->descriptor.wTotalLength = size;
	config->descriptor.bConfigurationValue = ++this->config_count;
}

void udc_handle_setup(struct usbdev_ctrl *this, int ep, dev_req_t *dr)
{
	if ((ep == 0) && setup_ep0(this, dr))
		return;

	if (this->current_config &&
	    this->current_config->interfaces[0].handle_setup &&
	    this->current_config->interfaces[0].handle_setup(this, ep, dr))
		return;

	/* no successful SETUP transfer should end up here, report error */
	this->halt_ep(this, ep, 0);
	this->halt_ep(this, ep, 1);
}
