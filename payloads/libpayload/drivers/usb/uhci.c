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

//#define USB_DEBUG

#include <arch/virtual.h>
#include <usb/usb.h>
#include "uhci.h"
#include "uhci_private.h"

static void uhci_start (hci_t *controller);
static void uhci_stop (hci_t *controller);
static void uhci_reset (hci_t *controller);
static void uhci_shutdown (hci_t *controller);
static int uhci_bulk (endpoint_t *ep, int size, u8 *data, int finalize);
static int uhci_control (usbdev_t *dev, direction_t dir, int drlen, void *devreq,
			 int dalen, u8 *data);
static void* uhci_create_intr_queue (endpoint_t *ep, int reqsize, int reqcount, int reqtiming);
static void uhci_destroy_intr_queue (endpoint_t *ep, void *queue);
static u8* uhci_poll_intr_queue (void *queue);

#if 0
/* dump uhci */
static void
uhci_dump (hci_t *controller)
{
	debug ("dump:\nUSBCMD: %x\n", uhci_reg_read16 (controller, USBCMD));
	debug ("USBSTS: %x\n", uhci_reg_read16 (controller, USBSTS));
	debug ("USBINTR: %x\n", uhci_reg_read16 (controller, USBINTR));
	debug ("FRNUM: %x\n", uhci_reg_read16 (controller, FRNUM));
	debug ("FLBASEADD: %x\n", uhci_reg_read32 (controller, FLBASEADD));
	debug ("SOFMOD: %x\n", uhci_reg_read8 (controller, SOFMOD));
	debug ("PORTSC1: %x\n", uhci_reg_read16 (controller, PORTSC1));
	debug ("PORTSC2: %x\n", uhci_reg_read16 (controller, PORTSC2));
}
#endif

static void
td_dump (td_t *td)
{
	char td_value[3];
	const char *td_type;
	switch (td->token & TD_PID_MASK) {
		case UHCI_SETUP:
			td_type="SETUP";
			break;
		case UHCI_IN:
			td_type="IN";
			break;
		case UHCI_OUT:
			td_type="OUT";
			break;
		default:
			sprintf(td_value, "%x", td->token & TD_PID_MASK);
			td_type=td_value;
	}
	debug ("%s packet (at %lx) to %x.%x failed\n", td_type,
		virt_to_phys (td), (td->token & TD_DEVADDR_MASK) >> TD_DEVADDR_SHIFT,
		(td->token & TD_EP_MASK) >> TD_EP_SHIFT);
	debug ("td (counter at %x) returns: ", td->ctrlsts >> TD_COUNTER_SHIFT);
	debug (" bitstuff err: %x, ", !!(td->ctrlsts & TD_STATUS_BITSTUFF_ERR));
	debug (" CRC err: %x, ", !!(td->ctrlsts & TD_STATUS_CRC_ERR));
	debug (" NAK rcvd: %x, ", !!(td->ctrlsts & TD_STATUS_NAK_RCVD));
	debug (" Babble: %x, ", !!(td->ctrlsts & TD_STATUS_BABBLE));
	debug (" Data Buffer err: %x, ", !!(td->ctrlsts & TD_STATUS_DATABUF_ERR));
	debug (" Stalled: %x, ", !!(td->ctrlsts & TD_STATUS_STALLED));
	debug (" Active: %x\n", !!(td->ctrlsts & TD_STATUS_ACTIVE));
	if (td->ctrlsts & TD_STATUS_BABBLE)
		debug (" Babble because of %s\n",
			(td->ctrlsts & TD_STATUS_BITSTUFF_ERR) ? "host" : "device");
	if (td->ctrlsts & TD_STATUS_ACTIVE)
		debug (" still active - timeout?\n");
}

static void
uhci_reset (hci_t *controller)
{
	/* reset */
	uhci_reg_write16 (controller, USBCMD, 4);
	mdelay (50);
	uhci_reg_write16 (controller, USBCMD, 0);
	mdelay (10);
	uhci_reg_write16 (controller, USBCMD, 2);
	while ((uhci_reg_read16 (controller, USBCMD) & 2) != 0)
		mdelay (1);

	uhci_reg_write32 (controller, FLBASEADD,
			  (u32) virt_to_phys (UHCI_INST (controller)->
					      framelistptr));
	//debug ("framelist at %p\n",UHCI_INST(controller)->framelistptr);

	/* disable irqs */
	uhci_reg_write16 (controller, USBINTR, 0);

	/* reset framelist index */
	uhci_reg_write16 (controller, FRNUM, 0);

	uhci_reg_write16(controller, USBCMD,
			 uhci_reg_read16(controller, USBCMD) | 0xc0);	// max packets, configure flag

	uhci_start (controller);
}

hci_t *
uhci_init (pcidev_t addr)
{
	int i;
	u16 reg16;

	hci_t *controller = new_controller ();

	if (!controller)
		fatal("Could not create USB controller instance.\n");

	controller->instance = malloc (sizeof (uhci_t));
	if(!controller->instance)
		fatal("Not enough memory creating USB controller instance.\n");

	controller->start = uhci_start;
	controller->stop = uhci_stop;
	controller->reset = uhci_reset;
	controller->shutdown = uhci_shutdown;
	controller->bulk = uhci_bulk;
	controller->control = uhci_control;
	controller->create_intr_queue = uhci_create_intr_queue;
	controller->destroy_intr_queue = uhci_destroy_intr_queue;
	controller->poll_intr_queue = uhci_poll_intr_queue;
	for (i = 0; i < 128; i++) {
		controller->devices[i] = 0;
	}
	init_device_entry (controller, 0);
	UHCI_INST (controller)->roothub = controller->devices[0];

	controller->bus_address = addr;
	controller->reg_base = pci_read_config32 (controller->bus_address, 0x20) & ~1;	/* ~1 clears the register type indicator that is set to 1 for IO space */

	/* kill legacy support handler */
	uhci_stop (controller);
	mdelay (1);
	uhci_reg_write16 (controller, USBSTS, 0x3f);
	reg16 = pci_read_config16(controller->bus_address, 0xc0);
	reg16 &= 0xdf80;
	pci_write_config16 (controller->bus_address, 0xc0, reg16);

	UHCI_INST (controller)->framelistptr = memalign (0x1000, 1024 * sizeof (flistp_t *));	/* 4kb aligned to 4kb */
	if (! UHCI_INST (controller)->framelistptr)
		fatal("Not enough memory for USB frame list pointer.\n");

	memset (UHCI_INST (controller)->framelistptr, 0,
		1024 * sizeof (flistp_t));

	/* According to the *BSD UHCI code, this one is needed on some
	   PIIX chips, because otherwise they misbehave. It must be
	   added to the last chain.

	   FIXME: this leaks, if the driver should ever be reinited
	          for some reason. Not a problem now.
	   */
	td_t *antiberserk = memalign(16, sizeof(td_t));
	if (!antiberserk)
		fatal("Not enough memory for chipset workaround.\n");
	memset(antiberserk, 0, sizeof(td_t));

	UHCI_INST (controller)->qh_prei = memalign (16, sizeof (qh_t));
	UHCI_INST (controller)->qh_intr = memalign (16, sizeof (qh_t));
	UHCI_INST (controller)->qh_data = memalign (16, sizeof (qh_t));
	UHCI_INST (controller)->qh_last = memalign (16, sizeof (qh_t));

	if (! UHCI_INST (controller)->qh_prei ||
	    ! UHCI_INST (controller)->qh_intr ||
	    ! UHCI_INST (controller)->qh_data ||
	    ! UHCI_INST (controller)->qh_last)
		fatal("Not enough memory for USB controller queues.\n");

	UHCI_INST (controller)->qh_prei->headlinkptr =
		virt_to_phys (UHCI_INST (controller)->qh_intr) | FLISTP_QH;
	UHCI_INST (controller)->qh_prei->elementlinkptr = 0 | FLISTP_TERMINATE;

	UHCI_INST (controller)->qh_intr->headlinkptr =
		virt_to_phys (UHCI_INST (controller)->qh_data) | FLISTP_QH;
	UHCI_INST (controller)->qh_intr->elementlinkptr = 0 | FLISTP_TERMINATE;

	UHCI_INST (controller)->qh_data->headlinkptr =
		virt_to_phys (UHCI_INST (controller)->qh_last) | FLISTP_QH;
	UHCI_INST (controller)->qh_data->elementlinkptr = 0 | FLISTP_TERMINATE;

	UHCI_INST (controller)->qh_last->headlinkptr = virt_to_phys (UHCI_INST (controller)->qh_data) | FLISTP_TERMINATE;
	UHCI_INST (controller)->qh_last->elementlinkptr = virt_to_phys (antiberserk) | FLISTP_TERMINATE;

	for (i = 0; i < 1024; i++) {
		UHCI_INST (controller)->framelistptr[i] =
			virt_to_phys (UHCI_INST (controller)->qh_prei) | FLISTP_QH;
	}
	controller->devices[0]->controller = controller;
	controller->devices[0]->init = uhci_rh_init;
	controller->devices[0]->init (controller->devices[0]);
	uhci_reset (controller);
	return controller;
}

static void
uhci_shutdown (hci_t *controller)
{
	if (controller == 0)
		return;
	detach_controller (controller);
	UHCI_INST (controller)->roothub->destroy (UHCI_INST (controller)->
						  roothub);
	uhci_reg_write16(controller, USBCMD,
			 uhci_reg_read16(controller, USBCMD) & 0);	// stop work
	free (UHCI_INST (controller)->framelistptr);
	free (UHCI_INST (controller)->qh_prei);
	free (UHCI_INST (controller)->qh_intr);
	free (UHCI_INST (controller)->qh_data);
	free (UHCI_INST (controller)->qh_last);
	free (UHCI_INST (controller));
	free (controller);
}

static void
uhci_start (hci_t *controller)
{
	uhci_reg_write16(controller, USBCMD,
			 uhci_reg_read16(controller, USBCMD) | 1);	// start work on schedule
}

static void
uhci_stop (hci_t *controller)
{
	uhci_reg_write16(controller, USBCMD,
			 uhci_reg_read16(controller, USBCMD) & ~1);	// stop work on schedule
}

#define GET_TD(x) ((void*)(((unsigned int)(x))&~0xf))

static td_t *
wait_for_completed_qh (hci_t *controller, qh_t *qh)
{
	int timeout = 1000000;	/* max 30 ms. */
	void *current = GET_TD (qh->elementlinkptr);
	while (((qh->elementlinkptr & FLISTP_TERMINATE) == 0) && (timeout-- > 0)) {
		if (current != GET_TD (qh->elementlinkptr)) {
			current = GET_TD (qh->elementlinkptr);
			timeout = 1000000;
		}
		uhci_reg_write16(controller, USBSTS,
				 uhci_reg_read16(controller, USBSTS) | 0);	// clear resettable registers
		udelay (30);
	}
	return (GET_TD (qh->elementlinkptr) ==
		0) ? 0 : GET_TD (phys_to_virt (qh->elementlinkptr));
}

static int
maxlen (int size)
{
	return (size - 1) & 0x7ff;
}

static int
min (int a, int b)
{
	if (a < b)
		return a;
	else
		return b;
}

static int
uhci_control (usbdev_t *dev, direction_t dir, int drlen, void *devreq, int dalen,
	      unsigned char *data)
{
	int endp = 0;		/* this is control: always 0 */
	int mlen = dev->endpoints[0].maxpacketsize;
	int count = (2 + (dalen + mlen - 1) / mlen);
	unsigned short req = ((unsigned short *) devreq)[0];
	int i;
	td_t *tds = memalign (16, sizeof (td_t) * count);
	memset (tds, 0, sizeof (td_t) * count);
	count--;		/* to compensate for 0-indexed array */
	for (i = 0; i < count; i++) {
		tds[i].ptr = virt_to_phys (&tds[i + 1]) | TD_DEPTH_FIRST;
	}
	tds[count].ptr = 0 | TD_DEPTH_FIRST | TD_TERMINATE;

	tds[0].token = UHCI_SETUP |
		dev->address << TD_DEVADDR_SHIFT |
		endp << TD_EP_SHIFT |
		TD_TOGGLE_DATA0 |
		maxlen(drlen) << TD_MAXLEN_SHIFT;
	tds[0].bufptr = virt_to_phys (devreq);
	tds[0].ctrlsts = (3 << TD_COUNTER_SHIFT) |
		(dev->speed?TD_LOWSPEED:0) |
		TD_STATUS_ACTIVE;

	int toggle = 1;
	for (i = 1; i < count; i++) {
		switch (dir) {
			case SETUP: tds[i].token = UHCI_SETUP; break;
			case IN:    tds[i].token = UHCI_IN;    break;
			case OUT:   tds[i].token = UHCI_OUT;   break;
		}
		tds[i].token |= dev->address << TD_DEVADDR_SHIFT |
			endp << TD_EP_SHIFT |
			maxlen (min (mlen, dalen)) << TD_MAXLEN_SHIFT |
			toggle << TD_TOGGLE_SHIFT;
		tds[i].bufptr = virt_to_phys (data);
		tds[i].ctrlsts = (3 << TD_COUNTER_SHIFT) |
			(dev->speed?TD_LOWSPEED:0) |
			TD_STATUS_ACTIVE;
		toggle ^= 1;
		dalen -= mlen;
		data += mlen;
	}

	tds[count].token = (dir == OUT) ? UHCI_IN : UHCI_OUT |
		dev->address << TD_DEVADDR_SHIFT |
		endp << TD_EP_SHIFT |
		maxlen(0) << TD_MAXLEN_SHIFT |
		TD_TOGGLE_DATA1;
	tds[count].bufptr = 0;
	tds[0].ctrlsts = (0 << TD_COUNTER_SHIFT) | /* as Linux 2.4.10 does */
		(dev->speed?TD_LOWSPEED:0) |
		TD_STATUS_ACTIVE;
	UHCI_INST (dev->controller)->qh_data->elementlinkptr =
		virt_to_phys (tds) & ~(FLISTP_QH | FLISTP_TERMINATE);
	td_t *td = wait_for_completed_qh (dev->controller,
					  UHCI_INST (dev->controller)->
					  qh_data);
	int result;
	if (td == 0) {
		result = 0;
	} else {
		debug ("control packet, req %x\n", req);
		td_dump (td);
		result = 1;
	}
	free (tds);
	return result;
}

static td_t *
create_schedule (int numpackets)
{
	if (numpackets == 0)
		return 0;
	td_t *tds = memalign (16, sizeof (td_t) * numpackets);
	memset (tds, 0, sizeof (td_t) * numpackets);
	int i;
	for (i = 0; i < numpackets; i++) {
		tds[i].ptr = virt_to_phys (&tds[i + 1]) | TD_DEPTH_FIRST;
	}
	tds[numpackets - 1].ptr = 0 | TD_TERMINATE;
	return tds;
}

static void
fill_schedule (td_t *td, endpoint_t *ep, int length, unsigned char *data,
	       int *toggle)
{
	switch (ep->direction) {
		case IN: td->token = UHCI_IN; break;
		case OUT: td->token = UHCI_OUT; break;
		case SETUP: td->token = UHCI_SETUP; break;
	}
	td->token |= ep->dev->address << TD_DEVADDR_SHIFT |
		(ep->endpoint & 0xf) << TD_EP_SHIFT |
		maxlen (length) << TD_MAXLEN_SHIFT |
		(*toggle & 1) << TD_TOGGLE_SHIFT;
	td->bufptr = virt_to_phys (data);
	td->ctrlsts = ((ep->direction == SETUP?3:0) << TD_COUNTER_SHIFT) |
		ep->dev->speed?TD_LOWSPEED:0 |
		TD_STATUS_ACTIVE;
	*toggle ^= 1;
}

static int
run_schedule (usbdev_t *dev, td_t *td)
{
	UHCI_INST (dev->controller)->qh_data->elementlinkptr =
		virt_to_phys (td) | ~(FLISTP_QH | FLISTP_TERMINATE);
	td = wait_for_completed_qh (dev->controller,
				    UHCI_INST (dev->controller)->qh_data);
	if (td == 0) {
		return 0;
	} else {
		td_dump (td);
		return 1;
	}
}

/* finalize == 1: if data is of packet aligned size, add a zero length packet */
static int
uhci_bulk (endpoint_t *ep, int size, u8 *data, int finalize)
{
	int maxpsize = ep->maxpacketsize;
	if (maxpsize == 0)
		fatal("MaxPacketSize == 0!!!");
	int numpackets = (size + maxpsize - 1 + finalize) / maxpsize;
	if (numpackets == 0)
		return 0;
	td_t *tds = create_schedule (numpackets);
	int i = 0, toggle = ep->toggle;
	while ((size > 0) || ((size == 0) && (finalize != 0))) {
		fill_schedule (&tds[i], ep, min (size, maxpsize), data,
			       &toggle);
		i++;
		data += maxpsize;
		size -= maxpsize;
	}
	if (run_schedule (ep->dev, tds) == 1) {
		debug("Stalled. Trying to clean up.\n");
		clear_stall (ep);
		free (tds);
		return 1;
	}
	ep->toggle = toggle;
	free (tds);
	return 0;
}

typedef struct {
	qh_t *qh;
	td_t *tds;
	td_t *last_td;
	u8 *data;
	int lastread;
	int total;
	int reqsize;
} intr_q;

/* create and hook-up an intr queue into device schedule */
static void*
uhci_create_intr_queue (endpoint_t *ep, int reqsize, int reqcount, int reqtiming)
{
	u8 *data = malloc(reqsize*reqcount);
	td_t *tds = memalign(16, sizeof(td_t) * reqcount);
	qh_t *qh = memalign(16, sizeof(qh_t));

	if (!data || !tds || !qh)
		fatal("Not enough memory to create USB intr queue prerequisites.\n");

	qh->elementlinkptr = virt_to_phys(tds);

	intr_q *q = malloc(sizeof(intr_q));
	if (!q)
		fatal("Not enough memory to create USB intr queue.\n");
	q->qh = qh;
	q->tds = tds;
	q->data = data;
	q->lastread = 0;
	q->total = reqcount;
	q->reqsize = reqsize;
	q->last_td = &tds[reqcount - 1];

	memset (tds, 0, sizeof (td_t) * reqcount);
	int i;
	for (i = 0; i < reqcount; i++) {
		tds[i].ptr = virt_to_phys (&tds[i + 1]);

		switch (ep->direction) {
			case IN: tds[i].token = UHCI_IN; break;
			case OUT: tds[i].token = UHCI_OUT; break;
			case SETUP: tds[i].token = UHCI_SETUP; break;
		}
		tds[i].token |= ep->dev->address << TD_DEVADDR_SHIFT |
			(ep->endpoint & 0xf) << TD_EP_SHIFT |
			maxlen (reqsize) << TD_MAXLEN_SHIFT |
			(ep->toggle & 1) << TD_TOGGLE_SHIFT;
		tds[i].bufptr = virt_to_phys (data);
		tds[i].ctrlsts = (0 << TD_COUNTER_SHIFT) |
			ep->dev->speed?TD_LOWSPEED:0 |
			TD_STATUS_ACTIVE;
		ep->toggle ^= 1;
		data += reqsize;
	}
	tds[reqcount - 1].ptr = 0 | TD_TERMINATE;
	for (i = reqtiming; i < 1024; i += reqtiming) {
		/* FIXME: wrap in another qh, one for each occurance of the qh in the framelist */
		qh->headlinkptr = UHCI_INST (ep->dev->controller)->framelistptr[i] & ~FLISTP_TERMINATE;
		UHCI_INST (ep->dev->controller)->framelistptr[i] = virt_to_phys(qh) | FLISTP_QH;
	}
	return q;
}

/* remove queue from device schedule, dropping all data that came in */
static void
uhci_destroy_intr_queue (endpoint_t *ep, void *q_)
{
	intr_q *q = (intr_q*)q_;
	u32 val = virt_to_phys (q->qh);
	u32 end = virt_to_phys (UHCI_INST (ep->dev->controller)->qh_intr);
	int i;
	for (i=0; i<1024; i++) {
		u32 oldptr = 0;
		u32 ptr = UHCI_INST (ep->dev->controller)->framelistptr[i];
		while (ptr != end) {
			if (((qh_t*)phys_to_virt(ptr))->elementlinkptr == val) {
				((qh_t*)phys_to_virt(oldptr))->headlinkptr = ((qh_t*)phys_to_virt(ptr))->headlinkptr;
				free(phys_to_virt(ptr));
				break;
			}
			oldptr = ptr;
			ptr = ((qh_t*)phys_to_virt(ptr))->headlinkptr;
		}
	}
	free(q->data);
	free(q->tds);
	free(q->qh);
	free(q);
}

/* read one intr-packet from queue, if available. extend the queue for new input.
   return NULL if nothing new available.
   Recommended use: while (data=poll_intr_queue(q)) process(data);
 */
static u8*
uhci_poll_intr_queue (void *q_)
{
	intr_q *q = (intr_q*)q_;
	if ((q->tds[q->lastread].ctrlsts & TD_STATUS_ACTIVE) == 0) {
		/* FIXME: handle errors */
		int current = q->lastread;
		int previous;
		if (q->lastread == 0) {
			previous = q->total - 1;
		} else {
			previous = q->lastread - 1;
		}
		q->tds[previous].ctrlsts &= ~TD_STATUS_MASK;
		q->tds[previous].ptr = 0 | TD_TERMINATE;
		if (q->last_td != &q->tds[previous]) {
			q->last_td->ptr = virt_to_phys(&q->tds[previous]) & ~TD_TERMINATE;
			q->last_td = &q->tds[previous];
		}
		q->tds[previous].ctrlsts |= TD_STATUS_ACTIVE;
		q->lastread = (q->lastread + 1) % q->total;
		return &q->data[current*q->reqsize];
	}
	return NULL;
}

void
uhci_reg_write32 (hci_t *ctrl, usbreg reg, u32 value)
{
	outl (value, ctrl->reg_base + reg);
}

u32
uhci_reg_read32 (hci_t *ctrl, usbreg reg)
{
	return inl (ctrl->reg_base + reg);
}

void
uhci_reg_write16 (hci_t *ctrl, usbreg reg, u16 value)
{
	outw (value, ctrl->reg_base + reg);
}

u16
uhci_reg_read16 (hci_t *ctrl, usbreg reg)
{
	return inw (ctrl->reg_base + reg);
}

void
uhci_reg_write8 (hci_t *ctrl, usbreg reg, u8 value)
{
	outb (value, ctrl->reg_base + reg);
}

u8
uhci_reg_read8 (hci_t *ctrl, usbreg reg)
{
	return inb (ctrl->reg_base + reg);
}
