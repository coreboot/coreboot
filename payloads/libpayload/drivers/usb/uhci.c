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
	printf ("dump:\nUSBCMD: %x\n", uhci_reg_read16 (controller, USBCMD));
	printf ("USBSTS: %x\n", uhci_reg_read16 (controller, USBSTS));
	printf ("USBINTR: %x\n", uhci_reg_read16 (controller, USBINTR));
	printf ("FRNUM: %x\n", uhci_reg_read16 (controller, FRNUM));
	printf ("FLBASEADD: %x\n", uhci_reg_read32 (controller, FLBASEADD));
	printf ("SOFMOD: %x\n", uhci_reg_read8 (controller, SOFMOD));
	printf ("PORTSC1: %x\n", uhci_reg_read16 (controller, PORTSC1));
	printf ("PORTSC2: %x\n", uhci_reg_read16 (controller, PORTSC2));
}
#endif

static void
td_dump (td_t *td)
{
	char td_value[3];
	char *td_type;
	switch (td->pid) {
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
			sprintf(td_value, "%x", td->pid);
			td_type=td_value;
	}
	printf ("%s packet (at %lx) to %x.%x failed\n", td_type,
		virt_to_phys (td), td->dev_addr, td->endp);
	printf ("td (counter at %x) returns: ", td->counter);
	printf (" bitstuff err: %x, ", td->status_bitstuff_err);
	printf (" CRC err: %x, ", td->status_crc_err);
	printf (" NAK rcvd: %x, ", td->status_nakrcvd);
	printf (" Babble: %x, ", td->status_babble);
	printf (" Data Buffer err: %x, ", td->status_databuf_err);
	printf (" Stalled: %x, ", td->status_stalled);
	printf (" Active: %x\n", td->status_active);
	if (td->status_babble)
		printf (" Babble because of %s\n",
			td->status_bitstuff_err ? "host" : "device");
	if (td->status_active)
		printf (" still active - timeout?\n");
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
	//printf ("framelist at %p\n",UHCI_INST(controller)->framelistptr);

	/* disable irqs */
	uhci_reg_write16 (controller, USBINTR, 0);

	/* reset framelist index */
	uhci_reg_write16 (controller, FRNUM, 0);

	uhci_reg_mask16 (controller, USBCMD, ~0, 0xc0);	// max packets, configure flag

	uhci_start (controller);
}

hci_t *
uhci_init (pcidev_t addr)
{
	int i;
	u16 reg16;

	hci_t *controller = new_controller ();

	if (!controller)
		usb_fatal("Could not create USB controller instance.\n");

	controller->instance = malloc (sizeof (uhci_t));
	if(!controller->instance)
		usb_fatal("Not enough memory creating USB controller instance.\n");

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
		usb_fatal("Not enough memory for USB frame list pointer.\n");

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
		usb_fatal("Not enough memory for chipset workaround.\n");
	memset(antiberserk, 0, sizeof(td_t));

	UHCI_INST (controller)->qh_prei = memalign (16, sizeof (qh_t));
	UHCI_INST (controller)->qh_intr = memalign (16, sizeof (qh_t));
	UHCI_INST (controller)->qh_data = memalign (16, sizeof (qh_t));
	UHCI_INST (controller)->qh_last = memalign (16, sizeof (qh_t));

	if (! UHCI_INST (controller)->qh_prei ||
	    ! UHCI_INST (controller)->qh_intr ||
	    ! UHCI_INST (controller)->qh_data ||
	    ! UHCI_INST (controller)->qh_last)
		usb_fatal ("Not enough memory for USB controller queues.\n");

	UHCI_INST (controller)->qh_prei->headlinkptr.ptr =
		virt_to_phys (UHCI_INST (controller)->qh_intr);
	UHCI_INST (controller)->qh_prei->headlinkptr.queue_head = 1;
	UHCI_INST (controller)->qh_prei->elementlinkptr.ptr = 0;
	UHCI_INST (controller)->qh_prei->elementlinkptr.terminate = 1;

	UHCI_INST (controller)->qh_intr->headlinkptr.ptr =
		virt_to_phys (UHCI_INST (controller)->qh_data);
	UHCI_INST (controller)->qh_intr->headlinkptr.queue_head = 1;
	UHCI_INST (controller)->qh_intr->elementlinkptr.ptr = 0;
	UHCI_INST (controller)->qh_intr->elementlinkptr.terminate = 1;

	UHCI_INST (controller)->qh_data->headlinkptr.ptr =
		virt_to_phys (UHCI_INST (controller)->qh_last);
	UHCI_INST (controller)->qh_data->headlinkptr.queue_head = 1;
	UHCI_INST (controller)->qh_data->elementlinkptr.ptr = 0;
	UHCI_INST (controller)->qh_data->elementlinkptr.terminate = 1;

	UHCI_INST (controller)->qh_last->headlinkptr.ptr = virt_to_phys (UHCI_INST (controller)->qh_data);
	UHCI_INST (controller)->qh_last->headlinkptr.terminate = 1;
	UHCI_INST (controller)->qh_last->elementlinkptr.ptr = virt_to_phys (antiberserk);
	UHCI_INST (controller)->qh_last->elementlinkptr.terminate = 1;

	for (i = 0; i < 1024; i++) {
		UHCI_INST (controller)->framelistptr[i].ptr =
			virt_to_phys (UHCI_INST (controller)->qh_prei);
		UHCI_INST (controller)->framelistptr[i].terminate = 0;
		UHCI_INST (controller)->framelistptr[i].queue_head = 1;
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
	uhci_reg_mask16 (controller, USBCMD, 0, 0);	// stop work
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
	uhci_reg_mask16 (controller, USBCMD, ~0, 1);	// start work on schedule
}

static void
uhci_stop (hci_t *controller)
{
	uhci_reg_mask16 (controller, USBCMD, ~1, 0);	// stop work on schedule
}

#define GET_TD(x) ((void*)(((unsigned int)(x))&~0xf))

static td_t *
wait_for_completed_qh (hci_t *controller, qh_t *qh)
{
	int timeout = 1000000;	/* max 30 ms. */
	void *current = GET_TD (qh->elementlinkptr.ptr);
	while ((qh->elementlinkptr.terminate == 0) && (timeout-- > 0)) {
		if (current != GET_TD (qh->elementlinkptr.ptr)) {
			current = GET_TD (qh->elementlinkptr.ptr);
			timeout = 1000000;
		}
		uhci_reg_mask16 (controller, USBSTS, ~0, 0);	// clear resettable registers
		udelay (30);
	}
	return (GET_TD (qh->elementlinkptr.ptr) ==
		0) ? 0 : GET_TD (phys_to_virt (qh->elementlinkptr.ptr));
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
		tds[i].ptr = virt_to_phys (&tds[i + 1]);
		tds[i].depth_first = 1;
		tds[i].terminate = 0;
	}
	tds[count].ptr = 0;
	tds[count].depth_first = 1;
	tds[count].terminate = 1;

	tds[0].pid = UHCI_SETUP;
	tds[0].dev_addr = dev->address;
	tds[0].endp = endp;
	tds[0].maxlen = maxlen (drlen);
	tds[0].counter = 3;
	tds[0].data_toggle = 0;
	tds[0].lowspeed = dev->speed;
	tds[0].bufptr = virt_to_phys (devreq);
	tds[0].status_active = 1;

	int toggle = 1;
	for (i = 1; i < count; i++) {
		switch (dir) {
			case SETUP: tds[i].pid = UHCI_SETUP; break;
			case IN:    tds[i].pid = UHCI_IN;    break;
			case OUT:   tds[i].pid = UHCI_OUT;   break;
		}
		tds[i].dev_addr = dev->address;
		tds[i].endp = endp;
		tds[i].maxlen = maxlen (min (mlen, dalen));
		tds[i].counter = 3;
		tds[i].data_toggle = toggle;
		tds[i].lowspeed = dev->speed;
		tds[i].bufptr = virt_to_phys (data);
		tds[i].status_active = 1;
		toggle ^= 1;
		dalen -= mlen;
		data += mlen;
	}

	tds[count].pid = (dir == OUT) ? UHCI_IN : UHCI_OUT;
	tds[count].dev_addr = dev->address;
	tds[count].endp = endp;
	tds[count].maxlen = maxlen (0);
	tds[count].counter = 0;	/* as per linux 2.4.10 */
	tds[count].data_toggle = 1;
	tds[count].lowspeed = dev->speed;
	tds[count].bufptr = 0;
	tds[count].status_active = 1;
	UHCI_INST (dev->controller)->qh_data->elementlinkptr.ptr =
		virt_to_phys (tds);
	UHCI_INST (dev->controller)->qh_data->elementlinkptr.queue_head = 0;
	UHCI_INST (dev->controller)->qh_data->elementlinkptr.terminate = 0;
	td_t *td = wait_for_completed_qh (dev->controller,
					  UHCI_INST (dev->controller)->
					  qh_data);
	int result;
	if (td == 0) {
		result = 0;
	} else {
		printf ("control packet, req %x\n", req);
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
		tds[i].ptr = virt_to_phys (&tds[i + 1]);
		tds[i].terminate = 0;
		tds[i].queue_head = 0;
		tds[i].depth_first = 1;
	}
	tds[numpackets - 1].ptr = 0;
	tds[numpackets - 1].terminate = 1;
	tds[numpackets - 1].queue_head = 0;
	tds[numpackets - 1].depth_first = 0;
	return tds;
}

static void
fill_schedule (td_t *td, endpoint_t *ep, int length, unsigned char *data,
	       int *toggle)
{
	td->pid = ep->direction;
	td->dev_addr = ep->dev->address;
	td->endp = ep->endpoint & 0xf;
	td->maxlen = maxlen (length);
	if (ep->direction == SETUP)
		td->counter = 3;
	else
		td->counter = 0;
	td->data_toggle = *toggle & 1;
	td->lowspeed = ep->dev->speed;
	td->bufptr = virt_to_phys (data);

	td->status_active = 1;
	*toggle ^= 1;
}

static int
run_schedule (usbdev_t *dev, td_t *td)
{
	UHCI_INST (dev->controller)->qh_data->elementlinkptr.ptr =
		virt_to_phys (td);
	UHCI_INST (dev->controller)->qh_data->elementlinkptr.queue_head = 0;
	UHCI_INST (dev->controller)->qh_data->elementlinkptr.terminate = 0;
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
		usb_fatal ("MaxPacketSize == 0!!!");
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
		usb_fatal ("Not enough memory to create USB intr queue prerequisites.\n");

	qh->elementlinkptr.ptr = virt_to_phys(tds);
	qh->elementlinkptr.queue_head = 0;
	qh->elementlinkptr.terminate = 0;

	intr_q *q = malloc(sizeof(intr_q));
	if (!q)
		usb_fatal ("Not enough memory to create USB intr queue.\n");
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
		tds[i].terminate = 0;
		tds[i].queue_head = 0;
		tds[i].depth_first = 0;

		tds[i].pid = ep->direction;
		tds[i].dev_addr = ep->dev->address;
		tds[i].endp = ep->endpoint & 0xf;
		tds[i].maxlen = maxlen (reqsize);
		tds[i].counter = 0;
		tds[i].data_toggle = ep->toggle & 1;
		tds[i].lowspeed = ep->dev->speed;
		tds[i].bufptr = virt_to_phys (data);
		tds[i].status_active = 1;
		ep->toggle ^= 1;
		data += reqsize;
	}
	tds[reqcount - 1].ptr = 0;
	tds[reqcount - 1].terminate = 1;
	tds[reqcount - 1].queue_head = 0;
	tds[reqcount - 1].depth_first = 0;
	for (i = reqtiming; i < 1024; i += reqtiming) {
		/* FIXME: wrap in another qh, one for each occurance of the qh in the framelist */
		qh->headlinkptr.ptr = UHCI_INST (ep->dev->controller)->framelistptr[i].ptr;
		qh->headlinkptr.terminate = 0;
		UHCI_INST (ep->dev->controller)->framelistptr[i].ptr = virt_to_phys(qh);
		UHCI_INST (ep->dev->controller)->framelistptr[i].terminate = 0;
		UHCI_INST (ep->dev->controller)->framelistptr[i].queue_head = 1;
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
		u32 ptr = UHCI_INST (ep->dev->controller)->framelistptr[i].ptr;
		while (ptr != end) {
			if (((qh_t*)phys_to_virt(ptr))->elementlinkptr.ptr == val) {
				((qh_t*)phys_to_virt(oldptr))->headlinkptr.ptr = ((qh_t*)phys_to_virt(ptr))->headlinkptr.ptr;
				free(phys_to_virt(ptr));
				break;
			}
			oldptr = ptr;
			ptr = ((qh_t*)phys_to_virt(ptr))->headlinkptr.ptr;
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
	if (q->tds[q->lastread].status_active == 0) {
		/* FIXME: handle errors */
		int current = q->lastread;
		int previous;
		if (q->lastread == 0) {
			previous = q->total - 1;
		} else {
			previous = q->lastread - 1;
		}
		q->tds[previous].status = 0;
		q->tds[previous].ptr = 0;
		q->tds[previous].terminate = 1;
		if (q->last_td != &q->tds[previous]) {
			q->last_td->ptr = virt_to_phys(&q->tds[previous]);
			q->last_td->terminate = 0;
			q->last_td = &q->tds[previous];
		}
		q->tds[previous].status_active = 1;
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

void
uhci_reg_mask32 (hci_t *ctrl, usbreg reg, u32 andmask, u32 ormask)
{
	uhci_reg_write32 (ctrl, reg,
			  (uhci_reg_read32 (ctrl, reg) & andmask) | ormask);
}

void
uhci_reg_mask16 (hci_t *ctrl, usbreg reg, u16 andmask, u16 ormask)
{
	uhci_reg_write16 (ctrl, reg,
			  (uhci_reg_read16 (ctrl, reg) & andmask) | ormask);
}

void
uhci_reg_mask8 (hci_t *ctrl, usbreg reg, u8 andmask, u8 ormask)
{
	uhci_reg_write8 (ctrl, reg,
			 (uhci_reg_read8 (ctrl, reg) & andmask) | ormask);
}
