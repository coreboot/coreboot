/*
 * This file is part of the libpayload project.
 *
 * Copyright (C) 2010 Patrick Georgi
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

#define USB_DEBUG

#include <arch/virtual.h>
#include <usb/usb.h>
#include "ohci_private.h"
#include "ohci.h"

static void ohci_start (hci_t *controller);
static void ohci_stop (hci_t *controller);
static void ohci_reset (hci_t *controller);
static void ohci_shutdown (hci_t *controller);
static int ohci_bulk (endpoint_t *ep, int size, u8 *data, int finalize);
static int ohci_control (usbdev_t *dev, direction_t dir, int drlen, void *devreq,
			 int dalen, u8 *data);
static void* ohci_create_intr_queue (endpoint_t *ep, int reqsize, int reqcount, int reqtiming);
static void ohci_destroy_intr_queue (endpoint_t *ep, void *queue);
static u8* ohci_poll_intr_queue (void *queue);

static void
ohci_reset (hci_t *controller)
{
}

#ifdef USB_DEBUG
/* Section 4.3.3 */
static const char *completion_codes[] = {
	"No error",
	"CRC",
	"Bit stuffing",
	"Data toggle mismatch",
	"Stall",
	"Device not responding",
	"PID check failure",
	"Unexpected PID",
	"Data overrun",
	"Data underrun",
	"--- (10)",
	"--- (11)",
	"Buffer overrun",
	"Buffer underrun",
	"Not accessed (14)",
	"Not accessed (15)"
};

/* Section 4.3.1.2 */
static const char *direction[] = {
	"SETUP",
	"OUT",
	"IN",
	"reserved / from TD"
};
#endif

hci_t *
ohci_init (pcidev_t addr)
{
	int i;

	hci_t *controller = new_controller ();

	if (!controller)
		usb_fatal("Could not create USB controller instance.\n");

	controller->instance = malloc (sizeof (ohci_t));
	if(!controller->instance)
		usb_fatal("Not enough memory creating USB controller instance.\n");

	controller->start = ohci_start;
	controller->stop = ohci_stop;
	controller->reset = ohci_reset;
	controller->shutdown = ohci_shutdown;
	controller->bulk = ohci_bulk;
	controller->control = ohci_control;
	controller->create_intr_queue = ohci_create_intr_queue;
	controller->destroy_intr_queue = ohci_destroy_intr_queue;
	controller->poll_intr_queue = ohci_poll_intr_queue;
	for (i = 0; i < 128; i++) {
		controller->devices[i] = 0;
	}
	init_device_entry (controller, 0);
	OHCI_INST (controller)->roothub = controller->devices[0];

	controller->bus_address = addr;
	controller->reg_base = pci_read_config32 (controller->bus_address, 0x10); // OHCI mandates MMIO, so bit 0 is clear
	OHCI_INST (controller)->opreg = (opreg_t*)phys_to_virt(controller->reg_base);
	printf("OHCI Version %x.%x\n", (OHCI_INST (controller)->opreg->HcRevision >> 4) & 0xf, OHCI_INST (controller)->opreg->HcRevision & 0xf);

	if ((OHCI_INST (controller)->opreg->HcControl & HostControllerFunctionalStateMask) == USBReset) {
		/* cold boot */
		OHCI_INST (controller)->opreg->HcControl &= ~RemoteWakeupConnected;
		OHCI_INST (controller)->opreg->HcFmInterval = (11999 * FrameInterval) | ((((11999 - 210)*6)/7) * FSLargestDataPacket);
		/* TODO: right value for PowerOnToPowerGoodTime ? */
		OHCI_INST (controller)->opreg->HcRhDescriptorA = NoPowerSwitching | NoOverCurrentProtection | (10 * PowerOnToPowerGoodTime);
		OHCI_INST (controller)->opreg->HcRhDescriptorB = (0 * DeviceRemovable);
		udelay(100); /* TODO: reset asserting according to USB spec */
	} else if ((OHCI_INST (controller)->opreg->HcControl & HostControllerFunctionalStateMask) != USBOperational) {
		OHCI_INST (controller)->opreg->HcControl = (OHCI_INST (controller)->opreg->HcControl & ~HostControllerFunctionalStateMask) | USBResume;
		udelay(100); /* TODO: resume time according to USB spec */
	}
	int interval = OHCI_INST (controller)->opreg->HcFmInterval;

	td_t *periodic_td = memalign(sizeof(*periodic_td), sizeof(*periodic_td));
	memset((void*)periodic_td, 0, sizeof(*periodic_td));
	for (i=0; i<32; i++) OHCI_INST (controller)->hcca->HccaInterruptTable[i] = virt_to_phys(periodic_td);
	/* TODO: build HCCA data structures */

	OHCI_INST (controller)->opreg->HcCommandStatus = HostControllerReset;
	udelay (10); /* at most 10us for reset to complete. State must be set to Operational within 2ms (5.1.1.4) */
	OHCI_INST (controller)->opreg->HcFmInterval = interval;
	OHCI_INST (controller)->hcca = memalign(256, 256);
	memset((void*)OHCI_INST (controller)->hcca, 0, 256);

	OHCI_INST (controller)->opreg->HcHCCA = virt_to_phys(OHCI_INST (controller)->hcca);
	OHCI_INST (controller)->opreg->HcControl &= ~IsochronousEnable; // unused by this driver
	OHCI_INST (controller)->opreg->HcControl |= BulkListEnable; // always enabled. OHCI still sleeps on BulkListFilled
	OHCI_INST (controller)->opreg->HcControl |= ControlListEnable; // dito
	OHCI_INST (controller)->opreg->HcControl |= PeriodicListEnable; // FIXME: setup interrupt data structures and enable all the time
	// disable everything, contrary to what OHCI spec says in 5.1.1.4, as we don't need IRQs
	OHCI_INST (controller)->opreg->HcInterruptEnable = 1<<31;
	OHCI_INST (controller)->opreg->HcInterruptDisable = ~(1<<31);
	OHCI_INST (controller)->opreg->HcInterruptStatus = ~0;
	OHCI_INST (controller)->opreg->HcPeriodicStart = (((OHCI_INST (controller)->opreg->HcFmInterval & FrameIntervalMask) / 10) * 9);
	OHCI_INST (controller)->opreg->HcControl = (OHCI_INST (controller)->opreg->HcControl & ~HostControllerFunctionalStateMask) | USBOperational;

	mdelay(100);

	controller->devices[0]->controller = controller;
	controller->devices[0]->init = ohci_rh_init;
	controller->devices[0]->init (controller->devices[0]);
	ohci_reset (controller);
	return controller;
}

static void
ohci_shutdown (hci_t *controller)
{
	if (controller == 0)
		return;
	detach_controller (controller);
	ohci_stop(controller);
	OHCI_INST (controller)->roothub->destroy (OHCI_INST (controller)->
						  roothub);
	free (OHCI_INST (controller));
	free (controller);
}

static void
ohci_start (hci_t *controller)
{
// TODO: turn on all operation of OHCI, but assume that it's initialized.
}

static void
ohci_stop (hci_t *controller)
{
// TODO: turn off all operation of OHCI
}

static void
dump_td(td_t *cur, int level)
{
#ifdef USB_DEBUG
	static const char *spaces="          ";
	const char *spc=spaces+(10-level);
#endif
	debug("%std at %x (%s), condition code: %s\n", spc, cur, direction[cur->direction], completion_codes[cur->condition_code & 0xf]);
	debug("%s toggle: %x\n", spc, cur->toggle);
}

static int
wait_for_ed(usbdev_t *dev, ed_t *head)
{
	td_t *cur;

	/* wait for results */
	while (((head->head_pointer & ~3) != head->tail_pointer) &&
		!(head->head_pointer & 1) &&
		((((td_t*)phys_to_virt(head->head_pointer & ~3))->condition_code & 0xf)>=0xe)) {
		debug("intst: %x; ctrl: %x; cmdst: %x; head: %x -> %x, tail: %x, condition: %x\n",
			OHCI_INST(dev->controller)->opreg->HcInterruptStatus,
			OHCI_INST(dev->controller)->opreg->HcControl,
			OHCI_INST(dev->controller)->opreg->HcCommandStatus,
			head->head_pointer,
			((td_t*)phys_to_virt(head->head_pointer & ~3))->next_td,
			head->tail_pointer,
			((td_t*)phys_to_virt(head->head_pointer & ~3))->condition_code);
		mdelay(1);
	}
	if (OHCI_INST(dev->controller)->opreg->HcInterruptStatus & WritebackDoneHead) {
		debug("done queue:\n");
		debug("%x, %x\n", OHCI_INST(dev->controller)->hcca->HccaDoneHead, phys_to_virt(OHCI_INST(dev->controller)->hcca->HccaDoneHead));
		if ((OHCI_INST(dev->controller)->hcca->HccaDoneHead & ~1) == 0) {
			debug("HcInterruptStatus %x\n", OHCI_INST(dev->controller)->opreg->HcInterruptStatus);
		}
		td_t *done_queue = NULL;
		td_t *done_head = (td_t*)phys_to_virt(OHCI_INST(dev->controller)->hcca->HccaDoneHead);
		OHCI_INST(dev->controller)->opreg->HcInterruptStatus = WritebackDoneHead;
		while (1) {
			td_t *oldnext = (td_t*)phys_to_virt(done_head->next_td);
			if (oldnext == done_queue) break; /* last element refers to second to last, ie. endless loop */
			if (oldnext == phys_to_virt(0)) break; /* last element of done list == first element of real list */
			debug("head is %x, pointing to %x. requeueing to %x\n", done_head, oldnext, done_queue);
			done_head->next_td = (u32)done_queue;
			done_queue = done_head;
			done_head = oldnext;
		}
		for (cur = done_queue; cur != 0; cur = (td_t*)cur->next_td) {
			dump_td(cur, 1);
		}
	}

	if (head->head_pointer & 1) {
		debug("HALTED!\n");
		return 1;
	}
	return 0;
}

static int
ohci_control (usbdev_t *dev, direction_t dir, int drlen, void *devreq, int dalen,
	      unsigned char *data)
{
	int i;

	td_t *cur;

	// pages are specified as 4K in OHCI, so don't use getpagesize()
	int first_page = (unsigned long)data / 4096;
	int last_page = (unsigned long)(data+dalen-1)/4096;
	if (last_page < first_page) last_page = first_page;
	int pages = (dalen==0)?0:(last_page - first_page + 1);
	int td_count = (pages+1)/2;

	td_t *tds = memalign(sizeof(td_t), (td_count+3)*sizeof(td_t));
	memset((void*)tds, 0, (td_count+3)*sizeof(td_t));

	for (i=0; i < td_count + 3; i++) {
		tds[i].next_td = virt_to_phys(&tds[i+1]);
	}
	tds[td_count + 3].next_td = 0;

	tds[0].direction = OHCI_SETUP;
	tds[0].toggle_from_td = 1;
	tds[0].toggle = 0;
	tds[0].error_count = 0;
	tds[0].delay_interrupt = 7;
	tds[0].condition_code = 0xf;
	tds[0].current_buffer_pointer = virt_to_phys(devreq);
	tds[0].buffer_end = virt_to_phys(devreq + drlen - 1);

	cur = &tds[0];

	while (pages > 0) {
		cur++;
		cur->direction = (dir==IN)?OHCI_IN:OHCI_OUT;
		cur->toggle_from_td = 0;
		cur->toggle = 1;
		cur->error_count = 0;
		cur->delay_interrupt = 7;
		cur->condition_code = 0xf;
		cur->current_buffer_pointer = virt_to_phys(data);
		pages--;
		int consumed = (4096 - ((unsigned long)data % 4096));
		if (consumed >= dalen) {
			// end of data is within same page
			cur->buffer_end = virt_to_phys(data + dalen - 1);
			dalen = 0;
			/* assert(pages == 0); */
		} else {
			dalen -= consumed;
			data += consumed;
			pages--;
			int second_page_size = dalen;
			if (dalen > 4096) {
				second_page_size = 4096;
			}
			cur->buffer_end = virt_to_phys(data + second_page_size - 1);
			dalen -= second_page_size;
			data += second_page_size;
		}
	}

	cur++;
	cur->direction = (dir==IN)?OHCI_OUT:OHCI_IN;
	cur->toggle_from_td = 1;
	cur->toggle = 1;
	cur->error_count = 0;
	cur->delay_interrupt = 7;
	cur->condition_code = 0xf;
	cur->current_buffer_pointer = 0;
	cur->buffer_end = 0;

	/* final dummy TD */
	cur++;

	/* Data structures */
	ed_t *head = memalign(sizeof(ed_t), sizeof(ed_t));
	memset((void*)head, 0, sizeof(*head));
	head->function_address = dev->address;
	head->endpoint_number = 0;
	head->direction = OHCI_FROM_TD;
	head->lowspeed = dev->speed;
	head->format = 0;
	head->maximum_packet_size = dev->endpoints[0].maxpacketsize;
	head->tail_pointer = virt_to_phys(cur);
	head->head_pointer = virt_to_phys(tds);
	head->halted = 0;
	head->toggle = 0;

	debug("doing control transfer with %x. first_td at %x\n", head->function_address, virt_to_phys(tds));

	/* activate schedule */
	OHCI_INST(dev->controller)->opreg->HcControlHeadED = virt_to_phys(head);
	OHCI_INST(dev->controller)->opreg->HcCommandStatus = ControlListFilled;

	int failure = wait_for_ed(dev, head);

	/* free memory */
	free((void*)tds);
	free((void*)head);

	return failure;
}

/* finalize == 1: if data is of packet aligned size, add a zero length packet */
static int
ohci_bulk (endpoint_t *ep, int dalen, u8 *data, int finalize)
{
	int i;
	debug("bulk: %x bytes from %x, finalize: %x, maxpacketsize: %x\n", dalen, data, finalize, ep->maxpacketsize);

	td_t *cur;

	// pages are specified as 4K in OHCI, so don't use getpagesize()
	int first_page = (unsigned long)data / 4096;
	int last_page = (unsigned long)(data+dalen-1)/4096;
	if (last_page < first_page) last_page = first_page;
	int pages = (dalen==0)?0:(last_page - first_page + 1);
	int td_count = (pages+1)/2;

	if (finalize && ((dalen % ep->maxpacketsize) == 0)) {
		td_count++;
	}

	td_t *tds = memalign(sizeof(td_t), (td_count+1)*sizeof(td_t));
	memset((void*)tds, 0, (td_count+1)*sizeof(td_t));

	for (i=0; i < td_count; i++) {
		tds[i].next_td = virt_to_phys(&tds[i+1]);
	}

	for (cur = tds; cur->next_td != 0; cur++) {
		cur->toggle_from_td = 0;
		cur->error_count = 0;
		cur->delay_interrupt = 7;
		cur->condition_code = 0xf;
		cur->direction = (ep->direction==IN)?OHCI_IN:OHCI_OUT;
		pages--;
		if (dalen == 0) {
			/* magic TD for empty packet transfer */
			cur->current_buffer_pointer = 0;
			cur->buffer_end = 0;
			/* assert((pages == 0) && finalize); */
		}
		int consumed = (4096 - ((unsigned long)data % 4096));
		if (consumed >= dalen) {
			// end of data is within same page
			cur->buffer_end = virt_to_phys(data + dalen - 1);
			dalen = 0;
			/* assert(pages == finalize); */
		} else {
			dalen -= consumed;
			data += consumed;
			pages--;
			int second_page_size = dalen;
			if (dalen > 4096) {
				second_page_size = 4096;
			}
			cur->buffer_end = virt_to_phys(data + second_page_size - 1);
			dalen -= second_page_size;
			data += second_page_size;
		}
	}

	/* Data structures */
	ed_t *head = memalign(sizeof(ed_t), sizeof(ed_t));
	memset((void*)head, 0, sizeof(*head));
	head->function_address = ep->dev->address;
	head->endpoint_number = ep->endpoint & 0xf;
	head->direction = (ep->direction==IN)?OHCI_IN:OHCI_OUT;
	head->lowspeed = ep->dev->speed;
	head->format = 0;
	head->maximum_packet_size = ep->maxpacketsize;
	head->tail_pointer = virt_to_phys(cur);
	head->head_pointer = virt_to_phys(tds);
	head->halted = 0;
	head->toggle = ep->toggle;

	debug("doing bulk transfer with %x(%x). first_td at %x, last %x\n", head->function_address, head->endpoint_number, virt_to_phys(tds), virt_to_phys(cur));

	/* activate schedule */
	OHCI_INST(ep->dev->controller)->opreg->HcBulkHeadED = virt_to_phys(head);
	OHCI_INST(ep->dev->controller)->opreg->HcCommandStatus = BulkListFilled;

	int failure = wait_for_ed(ep->dev, head);

	ep->toggle = head->toggle;

	/* free memory */
	free((void*)tds);
	free((void*)head);

	if (failure) {
		/* try cleanup */
		clear_stall(ep);
	}

	return failure;
}

/* create and hook-up an intr queue into device schedule */
static void*
ohci_create_intr_queue (endpoint_t *ep, int reqsize, int reqcount, int reqtiming)
{
	return NULL;
}

/* remove queue from device schedule, dropping all data that came in */
static void
ohci_destroy_intr_queue (endpoint_t *ep, void *q_)
{
}

/* read one intr-packet from queue, if available. extend the queue for new input.
   return NULL if nothing new available.
   Recommended use: while (data=poll_intr_queue(q)) process(data);
 */
static u8*
ohci_poll_intr_queue (void *q_)
{
	return NULL;
}

