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

#ifndef __UDC_H__
#define __UDC_H__

#include <queue.h>
#include <usb/usb.h>

struct usbdev_ctrl;

struct usbdev_interface {
	interface_descriptor_t descriptor;
	void (*init)(struct usbdev_ctrl *);

	/**
	 * handle_packet: called by the controller driver for incoming packets
	 *
	 * @param ep endpoint the packet is for
	 *
	 * @param in_dir 1, if it's an IN or INTR transfer.
	 *               0 for OUT, SETUP is handled in handle_setup
	 *
	 * @param len Actual transfer length in bytes. Can differ from the
	 *            scheduled length if the host requested a smaller
	 *            transfer than we prepared for.
	 */
	void (*handle_packet)(struct usbdev_ctrl *, int ep, int in_dir,
		void *data, int len);

	/**
	 * handle_setup: called by the controller driver for setup packets
	 *
	 * @param ep endpoint the SETUP request came up on
	 * @param dr SETUP request data
	 */
	int (*handle_setup)(struct usbdev_ctrl *, int ep, dev_req_t *dr);
	endpoint_descriptor_t *eps;
};

struct usbdev_configuration {
	configuration_descriptor_t descriptor;
	SLIST_ENTRY(usbdev_configuration) list;
	struct usbdev_interface interfaces[];
};

SLIST_HEAD(configuration_list, usbdev_configuration);

struct usbdev_ctrl {
	/* private data */
	void *pdata;

	int initialized;
	int remote_wakeup;

	struct usbdev_configuration *current_config;
	struct usbdev_interface *current_iface;
	int current_config_id;

	struct configuration_list configs;
	int config_count;
	device_descriptor_t device_descriptor;

	int ep_halted[16][2];
	int ep_mps[16][2];

	/** returns 0 if an error occurred */
	int (*poll)(struct usbdev_ctrl *);

	/**
	 * Add a gadget driver that exposes properties described in config.
	 *
	 * Each gadget driver is registered and exposed as separate USB
	 * "configuration", so the host can choose between them.
	 */
	void (*add_gadget)(struct usbdev_ctrl *,
		struct usbdev_configuration *config);

	/**
	 * Add a set of strings to use for string descriptors.
         *
         * 's' must point to an array of strings of which the first
         * element is unused, with at most 255 elements.
         *
         * 'm' is the size of 'strings' (ie. the index of the last entry).
         *
         * 'l' is the USB language code, of which some are defined below,
         * eg. LANG_EN_US.
	 *
	 * For now, only one language is ever exposed: Calling add_strings overwrites
         * older configuration.
	 */
	void (*add_strings)(unsigned short l, unsigned char m, const char **s);

	/**
	 * Add packet to process by the controller.
	 * zlp: zero length packet, if such a termination is necessary
	 * autofree: free data after use
	 */
	void (*enqueue_packet)(struct usbdev_ctrl *this, int endpoint,
		int in_dir, void *data, int len, int zlp, int autofree);

	/**
	 * Tell the hardware that it should listen to a new address
	 */
	void (*set_address)(struct usbdev_ctrl *, int address);

	void (*halt_ep)(struct usbdev_ctrl *this, int ep, int in_dir);
	void (*start_ep)(struct usbdev_ctrl *this,
		int ep, int in_dir, int ep_type, int mps);

	/**
	 * Set or clear endpoint ep's STALL state
	 */
	void (*stall)(struct usbdev_ctrl *, uint8_t ep, int in_dir, int set);

	/**
	 * Disable controller and deallocate data structures.
	 */
	void (*force_shutdown)(struct usbdev_ctrl *this);

	/**
	 * Let queues run out, then disable controller and deallocate data
	 * structures.
	 */
	void (*shutdown)(struct usbdev_ctrl *this);

	/**
	 * Allocate n bytes for use as data
	 */
	void *(*alloc_data)(size_t n);
	/**
	 * Free memory object allocated with alloc_data()
	 */
	void (*free_data)(void *);
};

#define LANG_DE_DE 0x0407
#define LANG_EN_US 0x0409

void udc_add_gadget(struct usbdev_ctrl *this,
	struct usbdev_configuration *config);
void udc_add_strings(unsigned short id, unsigned char count,
	const char *strings[]);
void udc_handle_setup(struct usbdev_ctrl *this, int ep, dev_req_t *dr);

#endif
