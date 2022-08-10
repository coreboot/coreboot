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

#include <arch/virtual.h>
#include <inttypes.h>
#include <usb/usb.h>
#include "uhci.h"
#include "uhci_private.h"

static void uhci_start(hci_t *controller);
static void uhci_stop(hci_t *controller);
static void uhci_reset(hci_t *controller);
static void uhci_shutdown(hci_t *controller);
static int uhci_bulk(endpoint_t *ep, int size, u8 *data, int finalize);
static int uhci_control(usbdev_t *dev, direction_t dir, int drlen, void *devreq,
			 int dalen, u8 *data);
static void* uhci_create_intr_queue(endpoint_t *ep, int reqsize, int reqcount, int reqtiming);
static void uhci_destroy_intr_queue(endpoint_t *ep, void *queue);
static u8* uhci_poll_intr_queue(void *queue);

#if 0
/* dump uhci */
static void
uhci_dump(hci_t *controller)
{
	usb_debug("dump:\nUSBCMD: %x\n", uhci_reg_read16(controller, USBCMD));
	usb_debug("USBSTS: %x\n", uhci_reg_read16(controller, USBSTS));
	usb_debug("USBINTR: %x\n", uhci_reg_read16(controller, USBINTR));
	usb_debug("FRNUM: %x\n", uhci_reg_read16(controller, FRNUM));
	usb_debug("FLBASEADD: %x\n", uhci_reg_read32(controller, FLBASEADD));
	usb_debug("SOFMOD: %x\n", uhci_reg_read8(controller, SOFMOD));
	usb_debug("PORTSC1: %x\n", uhci_reg_read16(controller, PORTSC1));
	usb_debug("PORTSC2: %x\n", uhci_reg_read16(controller, PORTSC2));
}
#endif

static void td_dump(td_t *td)
{
	usb_debug("+---------------------------------------------------+\n");
	if ((td->token & TD_PID_MASK) == UHCI_SETUP)
		usb_debug("|..[SETUP]..........................................|\n");
	else if ((td->token & TD_PID_MASK) == UHCI_IN)
		usb_debug("|..[IN].............................................|\n");
	else if ((td->token & TD_PID_MASK) == UHCI_OUT)
		usb_debug("|..[OUT]............................................|\n");
	else
		usb_debug("|..[]...............................................|\n");
	usb_debug("|:|============ UHCI TD at [0x%08lx] ==========|:|\n", virt_to_phys(td));
	usb_debug("|:+-----------------------------------------------+:|\n");
	usb_debug("|:| Next  TD/QH     [0x%08lx]                  |:|\n", td->ptr & ~0xFUL);
	usb_debug("|:+-----------------------------------------------+:|\n");
	usb_debug("|:| Depth/Breath [%lx]  | QH/TD [%lx] | TERMINATE [%lx] |:|\n",
	(td->ptr & (1UL << 2)) >> 2, (td->ptr & (1UL << 1)) >> 1, td->ptr & 1UL);
	usb_debug("|:+-----------------------------------------------+:|\n");
	usb_debug("|:|   T   | Maximum Length               | [%04lx] |:|\n", (td->token & (0x7FFUL << 21)) >> 21);
	usb_debug("|:|   O   | PID CODE                     | [%04"PRIx32"] |:|\n", td->token & 0xFF);
	usb_debug("|:|   K   | Endpoint                     | [%04"PRIx32"] |:|\n", (td->token & TD_EP_MASK) >> TD_EP_SHIFT);
	usb_debug("|:|   E   | Device Address               | [%04lx] |:|\n", (td->token & (0x7FUL << 8)) >> 8);
	usb_debug("|:|   N   | Data Toggle                  |    [%lx] |:|\n", (td->token & (1UL << 19)) >> 19);
	usb_debug("|:+-----------------------------------------------+:|\n");
	usb_debug("|:|   C   | Short Packet Detector        |    [%lx] |:|\n", (td->ctrlsts & (1UL << 29)) >> 29);
	usb_debug("|:|   O   | Error Counter                |    [%lx] |:|\n",
		  (td->ctrlsts & (3UL << TD_COUNTER_SHIFT)) >> TD_COUNTER_SHIFT);
	usb_debug("|:|   N   | Low Speed Device             |    [%lx] |:|\n", (td->ctrlsts & (1UL << 26)) >> 26);
	usb_debug("|:|   T   | Isochronous Select           |    [%lx] |:|\n", (td->ctrlsts & (1UL << 25)) >> 25);
	usb_debug("|:|   R   | Interrupt on Complete (IOC)  |    [%lx] |:|\n", (td->ctrlsts & (1UL << 24)) >> 24);
	usb_debug("|:+   O   ----------------------------------------+:|\n");
	usb_debug("|:|   L   | Active                       |    [%lx] |:|\n", (td->ctrlsts & (1UL << 23)) >> 23);
	usb_debug("|:|   &   | Stalled                      |    [%lx] |:|\n", (td->ctrlsts & (1UL << 22)) >> 22);
	usb_debug("|:|   S   | Data Buffer Error            |    [%lx] |:|\n", (td->ctrlsts & (1UL << 21)) >> 21);
	usb_debug("|:|   T   | Bubble Detected              |    [%lx] |:|\n", (td->ctrlsts & (1UL << 20)) >> 20);
	usb_debug("|:|   A   | NAK Received                 |    [%lx] |:|\n", (td->ctrlsts & (1UL << 19)) >> 19);
	usb_debug("|:|   T   | CRC/Timeout Error            |    [%lx] |:|\n", (td->ctrlsts & (1UL << 18)) >> 18);
	usb_debug("|:|   U   | Bitstuff Error               |    [%lx] |:|\n", (td->ctrlsts & (1UL << 17)) >> 17);
	usb_debug("|:|   S   ----------------------------------------|:|\n");
	usb_debug("|:|       | Actual Length                | [%04lx] |:|\n", td->ctrlsts & 0x7FFUL);
	usb_debug("|:+-----------------------------------------------+:|\n");
	usb_debug("|:| Buffer pointer  [0x%08"PRIx32"]                  |:|\n", td->bufptr);
	usb_debug("|:|-----------------------------------------------|:|\n");
	usb_debug("|...................................................|\n");
	usb_debug("+---------------------------------------------------+\n");
}

static void
uhci_reset(hci_t *controller)
{
	/* reset */
	uhci_reg_write16(controller, USBCMD, 4); /* Global Reset */
	mdelay(50); /* uhci spec 2.1.1: at least 10ms */
	uhci_reg_write16(controller, USBCMD, 0);
	mdelay(10);
	uhci_reg_write16(controller, USBCMD, 2); /* Host Controller Reset */
	/* wait for controller to finish reset */
	/* TOTEST: how long to wait? 100ms for now */
	int timeout = 200; /* time out after 200 * 500us == 100ms */
	while (((uhci_reg_read16(controller, USBCMD) & 2) != 0) && timeout--)
		udelay(500);
	if (timeout < 0)
		usb_debug("Warning: uhci: host controller reset timed out.\n");
}

static void
uhci_reinit(hci_t *controller)
{
	uhci_reg_write32(controller, FLBASEADD,
			  (u32) virt_to_phys(UHCI_INST(controller)->
					      framelistptr));
	//usb_debug ("framelist at %p\n",UHCI_INST(controller)->framelistptr);

	/* disable irqs */
	uhci_reg_write16(controller, USBINTR, 0);

	/* reset framelist index */
	uhci_reg_write16(controller, FRNUM, 0);

	uhci_reg_write16(controller, USBCMD,
			 uhci_reg_read16(controller, USBCMD) | 0xc0);	// max packets, configure flag

	uhci_start(controller);
}

hci_t *
uhci_pci_init(pcidev_t addr)
{
	int i;
	u16 reg16;

	hci_t *controller = new_controller();
	controller->pcidev = addr;
	controller->instance = xzalloc(sizeof(uhci_t));
	controller->type = UHCI;
	controller->start = uhci_start;
	controller->stop = uhci_stop;
	controller->reset = uhci_reset;
	controller->init = uhci_reinit;
	controller->shutdown = uhci_shutdown;
	controller->bulk = uhci_bulk;
	controller->control = uhci_control;
	controller->set_address = generic_set_address;
	controller->finish_device_config = NULL;
	controller->destroy_device = NULL;
	controller->create_intr_queue = uhci_create_intr_queue;
	controller->destroy_intr_queue = uhci_destroy_intr_queue;
	controller->poll_intr_queue = uhci_poll_intr_queue;
	init_device_entry(controller, 0);
	UHCI_INST(controller)->roothub = controller->devices[0];

	/* ~1 clears the register type indicator that is set to 1
	 * for IO space */
	controller->reg_base = pci_read_config32(addr, 0x20) & ~1;

	/* kill legacy support handler */
	uhci_stop(controller);
	mdelay(1);
	uhci_reg_write16(controller, USBSTS, 0x3f);
	reg16 = pci_read_config16(addr, 0xc0);
	reg16 &= 0xdf80;
	pci_write_config16(addr, 0xc0, reg16);

	UHCI_INST(controller)->framelistptr = memalign(0x1000, 1024 * sizeof(flistp_t));	/* 4kb aligned to 4kb */
	if (!UHCI_INST (controller)->framelistptr)
		fatal("Not enough memory for USB frame list pointer.\n");

	memset(UHCI_INST(controller)->framelistptr, 0,
		1024 * sizeof(flistp_t));

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

	UHCI_INST(controller)->qh_prei = memalign(16, sizeof(qh_t));
	UHCI_INST(controller)->qh_intr = memalign(16, sizeof(qh_t));
	UHCI_INST(controller)->qh_data = memalign(16, sizeof(qh_t));
	UHCI_INST(controller)->qh_last = memalign(16, sizeof(qh_t));

	if (!UHCI_INST (controller)->qh_prei ||
	    !UHCI_INST (controller)->qh_intr ||
	    !UHCI_INST (controller)->qh_data ||
	    !UHCI_INST (controller)->qh_last)
		fatal("Not enough memory for USB controller queues.\n");

	UHCI_INST(controller)->qh_prei->headlinkptr =
		virt_to_phys(UHCI_INST(controller)->qh_intr) | FLISTP_QH;
	UHCI_INST(controller)->qh_prei->elementlinkptr = 0 | FLISTP_TERMINATE;

	UHCI_INST(controller)->qh_intr->headlinkptr =
		virt_to_phys(UHCI_INST(controller)->qh_data) | FLISTP_QH;
	UHCI_INST(controller)->qh_intr->elementlinkptr = 0 | FLISTP_TERMINATE;

	UHCI_INST(controller)->qh_data->headlinkptr =
		virt_to_phys(UHCI_INST(controller)->qh_last) | FLISTP_QH;
	UHCI_INST(controller)->qh_data->elementlinkptr = 0 | FLISTP_TERMINATE;

	UHCI_INST(controller)->qh_last->headlinkptr = virt_to_phys(UHCI_INST(controller)->qh_data) | FLISTP_TERMINATE;
	UHCI_INST(controller)->qh_last->elementlinkptr = virt_to_phys(antiberserk) | FLISTP_TERMINATE;

	for (i = 0; i < 1024; i++) {
		UHCI_INST(controller)->framelistptr[i] =
			virt_to_phys(UHCI_INST(controller)->qh_prei) | FLISTP_QH;
	}
	controller->devices[0]->controller = controller;
	controller->devices[0]->init = uhci_rh_init;
	controller->devices[0]->init(controller->devices[0]);
	uhci_reset(controller);
	uhci_reinit(controller);
	return controller;
}

static void
uhci_shutdown(hci_t *controller)
{
	if (controller == 0)
		return;
	detach_controller(controller);
	uhci_reg_write16(controller, USBCMD,
			 uhci_reg_read16(controller, USBCMD) & 0);	// stop work
	free(UHCI_INST(controller)->framelistptr);
	free(UHCI_INST(controller)->qh_prei);
	free(UHCI_INST(controller)->qh_intr);
	free(UHCI_INST(controller)->qh_data);
	free(UHCI_INST(controller)->qh_last);
	free(UHCI_INST(controller));
	free(controller);
}

static void
uhci_start(hci_t *controller)
{
	uhci_reg_write16(controller, USBCMD,
			 uhci_reg_read16(controller, USBCMD) | 1);	// start work on schedule
}

static void
uhci_stop(hci_t *controller)
{
	uhci_reg_write16(controller, USBCMD,
			 uhci_reg_read16(controller, USBCMD) & ~1);	// stop work on schedule
}

#define UHCI_SLEEP_TIME_US 30
#define UHCI_TIMEOUT (USB_MAX_PROCESSING_TIME_US / UHCI_SLEEP_TIME_US)
#define GET_TD(x) ((void*)(((unsigned int)(x))&~0xf))

static td_t *
wait_for_completed_qh(hci_t *controller, qh_t *qh)
{
	int timeout = UHCI_TIMEOUT;
	void *current = GET_TD(qh->elementlinkptr);
	while (((qh->elementlinkptr & FLISTP_TERMINATE) == 0) && (timeout-- > 0)) {
		if (current != GET_TD(qh->elementlinkptr)) {
			current = GET_TD(qh->elementlinkptr);
			timeout = UHCI_TIMEOUT;
		}
		uhci_reg_write16(controller, USBSTS,
				 uhci_reg_read16(controller, USBSTS) | 0);	// clear resettable registers
		udelay(UHCI_SLEEP_TIME_US);
	}
	return (GET_TD(qh->elementlinkptr) ==
		0) ? 0 : GET_TD(phys_to_virt(qh->elementlinkptr));
}

static int
maxlen(int size)
{
	return (size - 1) & 0x7ff;
}

static int
min(int a, int b)
{
	if (a < b)
		return a;
	else
		return b;
}

static int
uhci_control(usbdev_t *dev, direction_t dir, int drlen, void *devreq, int dalen,
	      unsigned char *data)
{
	int endp = 0;		/* this is control: always 0 */
	int mlen = dev->endpoints[0].maxpacketsize;
	int count = (2 + (dalen + mlen - 1) / mlen);
	unsigned short req = ((unsigned short *) devreq)[0];
	int i;
	td_t *tds = memalign(16, sizeof(td_t) * count);
	if (!tds)
		fatal("Not enough memory for uhci control.\n");
	memset(tds, 0, sizeof(td_t) * count);
	count--;		/* to compensate for 0-indexed array */
	for (i = 0; i < count; i++) {
		tds[i].ptr = virt_to_phys(&tds[i + 1]) | TD_DEPTH_FIRST;
	}
	tds[count].ptr = 0 | TD_DEPTH_FIRST | TD_TERMINATE;

	tds[0].token = UHCI_SETUP |
		dev->address << TD_DEVADDR_SHIFT |
		endp << TD_EP_SHIFT |
		TD_TOGGLE_DATA0 |
		maxlen(drlen) << TD_MAXLEN_SHIFT;
	tds[0].bufptr = virt_to_phys(devreq);
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
			maxlen(min(mlen, dalen)) << TD_MAXLEN_SHIFT |
			toggle << TD_TOGGLE_SHIFT;
		tds[i].bufptr = virt_to_phys(data);
		tds[i].ctrlsts = (3 << TD_COUNTER_SHIFT) |
			(dev->speed?TD_LOWSPEED:0) |
			TD_STATUS_ACTIVE;
		toggle ^= 1;
		dalen -= mlen;
		data += mlen;
	}

	tds[count].token = ((dir == OUT) ? UHCI_IN : UHCI_OUT) |
		dev->address << TD_DEVADDR_SHIFT |
		endp << TD_EP_SHIFT |
		maxlen(0) << TD_MAXLEN_SHIFT |
		TD_TOGGLE_DATA1;
	tds[count].bufptr = 0;
	tds[count].ctrlsts = (0 << TD_COUNTER_SHIFT) | /* as Linux 2.4.10 does */
		(dev->speed?TD_LOWSPEED:0) |
		TD_STATUS_ACTIVE;
	UHCI_INST(dev->controller)->qh_data->elementlinkptr =
		virt_to_phys(tds) & ~(FLISTP_QH | FLISTP_TERMINATE);
	td_t *td = wait_for_completed_qh(dev->controller,
					  UHCI_INST(dev->controller)->
					  qh_data);
	int result;
	if (td == 0) {
		result = 0;
	} else {
		usb_debug("control packet, req %x\n", req);
		td_dump(td);
		result = -1;
	}
	free(tds);
	return result;
}

static td_t *
create_schedule(int numpackets)
{
	if (numpackets == 0)
		return 0;
	td_t *tds = memalign(16, sizeof(td_t) * numpackets);
	if (!tds)
		fatal("Not enough memory for packets scheduling.\n");
	memset(tds, 0, sizeof(td_t) * numpackets);
	int i;
	for (i = 0; i < numpackets; i++) {
		tds[i].ptr = virt_to_phys(&tds[i + 1]) | TD_DEPTH_FIRST;
	}
	tds[numpackets - 1].ptr = 0 | TD_TERMINATE;
	return tds;
}

static void
fill_schedule(td_t *td, endpoint_t *ep, int length, unsigned char *data,
	       int *toggle)
{
	switch (ep->direction) {
		case IN: td->token = UHCI_IN; break;
		case OUT: td->token = UHCI_OUT; break;
		case SETUP: td->token = UHCI_SETUP; break;
	}
	td->token |= ep->dev->address << TD_DEVADDR_SHIFT |
		(ep->endpoint & 0xf) << TD_EP_SHIFT |
		maxlen(length) << TD_MAXLEN_SHIFT |
		(*toggle & 1) << TD_TOGGLE_SHIFT;
	td->bufptr = virt_to_phys(data);
	td->ctrlsts = ((ep->direction == SETUP?3:0) << TD_COUNTER_SHIFT) |
		(ep->dev->speed?TD_LOWSPEED:0) |
		TD_STATUS_ACTIVE;
	*toggle ^= 1;
}

static int
run_schedule(usbdev_t *dev, td_t *td)
{
	UHCI_INST(dev->controller)->qh_data->elementlinkptr =
		virt_to_phys(td) & ~(FLISTP_QH | FLISTP_TERMINATE);
	td = wait_for_completed_qh(dev->controller,
				    UHCI_INST(dev->controller)->qh_data);
	if (td == 0) {
		return 0;
	} else {
		td_dump(td);
		return 1;
	}
}

/* finalize == 1: if data is of packet aligned size, add a zero length packet */
static int
uhci_bulk(endpoint_t *ep, int size, u8 *data, int finalize)
{
	int maxpsize = ep->maxpacketsize;
	if (maxpsize == 0)
		fatal("MaxPacketSize == 0!!!");
	int numpackets = (size + maxpsize - 1) / maxpsize;
	if (finalize && ((size % maxpsize) == 0)) {
		numpackets++;
	}
	if (numpackets == 0)
		return 0;
	td_t *tds = create_schedule(numpackets);
	int i = 0, toggle = ep->toggle;
	while ((size > 0) || ((size == 0) && (finalize != 0))) {
		fill_schedule(&tds[i], ep, min(size, maxpsize), data,
			       &toggle);
		i++;
		data += maxpsize;
		size -= maxpsize;
	}
	if (run_schedule(ep->dev, tds) == 1) {
		free(tds);
		return -1;
	}
	ep->toggle = toggle;
	free(tds);
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
uhci_create_intr_queue(endpoint_t *ep, int reqsize, int reqcount, int reqtiming)
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

	memset(tds, 0, sizeof(td_t) * reqcount);
	int i;
	for (i = 0; i < reqcount; i++) {
		tds[i].ptr = virt_to_phys(&tds[i + 1]);

		switch (ep->direction) {
			case IN: tds[i].token = UHCI_IN; break;
			case OUT: tds[i].token = UHCI_OUT; break;
			case SETUP: tds[i].token = UHCI_SETUP; break;
		}
		tds[i].token |= ep->dev->address << TD_DEVADDR_SHIFT |
			(ep->endpoint & 0xf) << TD_EP_SHIFT |
			maxlen(reqsize) << TD_MAXLEN_SHIFT |
			(ep->toggle & 1) << TD_TOGGLE_SHIFT;
		tds[i].bufptr = virt_to_phys(data);
		tds[i].ctrlsts = (0 << TD_COUNTER_SHIFT) |
			(ep->dev->speed?TD_LOWSPEED:0) |
			TD_STATUS_ACTIVE;
		ep->toggle ^= 1;
		data += reqsize;
	}
	tds[reqcount - 1].ptr = 0 | TD_TERMINATE;

	/* insert QH into framelist */
	uhci_t *const uhcic = UHCI_INST(ep->dev->controller);
	const u32 def_ptr = virt_to_phys(uhcic->qh_prei) | FLISTP_QH;
	int nothing_placed = 1;
	qh->headlinkptr = def_ptr;
	for (i = 0; i < 1024; i += reqtiming) {
		/* advance to the next free position */
		while ((i < 1024) && (uhcic->framelistptr[i] != def_ptr)) ++i;
		if (i < 1024) {
			uhcic->framelistptr[i] = virt_to_phys(qh) | FLISTP_QH;
			nothing_placed = 0;
		}
	}
	if (nothing_placed) {
		usb_debug("Error: Failed to place UHCI interrupt queue "
			      "head into framelist: no space left\n");
		uhci_destroy_intr_queue(ep, q);
		return NULL;
	}

	return q;
}

/* remove queue from device schedule, dropping all data that came in */
static void
uhci_destroy_intr_queue(endpoint_t *ep, void *q_)
{
	intr_q *const q = (intr_q*)q_;

	/* remove QH from framelist */
	uhci_t *const uhcic = UHCI_INST(ep->dev->controller);
	const u32 qh_ptr = virt_to_phys(q->qh) | FLISTP_QH;
	const u32 def_ptr = virt_to_phys(uhcic->qh_prei) | FLISTP_QH;
	int i;
	for (i = 0; i < 1024; ++i) {
		if (uhcic->framelistptr[i] == qh_ptr)
			uhcic->framelistptr[i] = def_ptr;
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
uhci_poll_intr_queue(void *q_)
{
	intr_q *q = (intr_q*)q_;
	if ((q->tds[q->lastread].ctrlsts & TD_STATUS_ACTIVE) == 0) {
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
		if (!(q->tds[current].ctrlsts & TD_STATUS_MASK))
			return &q->data[current*q->reqsize];
	}
	/* reset queue if we fully processed it after underrun */
	else if (q->qh->elementlinkptr & FLISTP_TERMINATE) {
		usb_debug("resetting underrun uhci interrupt queue.\n");
		q->qh->elementlinkptr = virt_to_phys(q->tds + q->lastread);
	}
	return NULL;
}

void
uhci_reg_write32(hci_t *ctrl, usbreg reg, u32 value)
{
	outl(value, ctrl->reg_base + reg);
}

u32
uhci_reg_read32(hci_t *ctrl, usbreg reg)
{
	return inl(ctrl->reg_base + reg);
}

void
uhci_reg_write16(hci_t *ctrl, usbreg reg, u16 value)
{
	outw(value, ctrl->reg_base + reg);
}

u16
uhci_reg_read16(hci_t *ctrl, usbreg reg)
{
	return inw(ctrl->reg_base + reg);
}

void
uhci_reg_write8(hci_t *ctrl, usbreg reg, u8 value)
{
	outb(value, ctrl->reg_base + reg);
}

u8
uhci_reg_read8(hci_t *ctrl, usbreg reg)
{
	return inb(ctrl->reg_base + reg);
}
