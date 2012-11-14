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

//#define USB_DEBUG

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
static void ohci_process_done_queue(ohci_t *ohci, int spew_debug);

static void
ohci_reset (hci_t *controller)
{
	if (controller == NULL)
		return;

	OHCI_INST(controller)->opreg->HcCommandStatus = HostControllerReset;
	mdelay(2); /* wait 2ms */
	OHCI_INST(controller)->opreg->HcControl = 0;
	mdelay(10); /* wait 10ms */
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
		fatal("Could not create USB controller instance.\n");

	controller->instance = malloc (sizeof (ohci_t));
	if(!controller->instance)
		fatal("Not enough memory creating USB controller instance.\n");

	controller->type = OHCI;

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
	/* regarding OHCI spec, Appendix A, BAR_OHCI register description, Table A-4
	 * BASE ADDRESS only [31-12] bits. All other usually 0, but not all */
	controller->reg_base = pci_read_config32 (controller->bus_address, 0x10) & 0xfffff000; // OHCI mandates MMIO, so bit 0 is clear
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

	OHCI_INST (controller)->opreg->HcCommandStatus = HostControllerReset;
	udelay (10); /* at most 10us for reset to complete. State must be set to Operational within 2ms (5.1.1.4) */
	OHCI_INST (controller)->opreg->HcFmInterval = interval;
	OHCI_INST (controller)->hcca = memalign(256, 256);
	memset((void*)OHCI_INST (controller)->hcca, 0, 256);

	/* Initialize interrupt table. */
	u32 *const intr_table = OHCI_INST(controller)->hcca->HccaInterruptTable;
	ed_t *const periodic_ed = memalign(sizeof(ed_t), sizeof(ed_t));
	memset((void *)periodic_ed, 0, sizeof(*periodic_ed));
	for (i = 0; i < 32; ++i)
		intr_table[i] = virt_to_phys(periodic_ed);
	OHCI_INST (controller)->periodic_ed = periodic_ed;

	OHCI_INST (controller)->opreg->HcHCCA = virt_to_phys(OHCI_INST (controller)->hcca);
	/* Make sure periodic schedule is enabled. */
	OHCI_INST (controller)->opreg->HcControl |= PeriodicListEnable;
	OHCI_INST (controller)->opreg->HcControl &= ~IsochronousEnable; // unused by this driver
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
	free ((void *)OHCI_INST (controller)->periodic_ed);
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
	usb_debug("%std at %x (%s), condition code: %s\n", spc, cur, direction[(cur->config & TD_DIRECTION_MASK) >> TD_DIRECTION_SHIFT],
		completion_codes[(cur->config & TD_CC_MASK) >> TD_CC_SHIFT]);
	usb_debug("%s toggle: %x\n", spc, !!(cur->config & TD_TOGGLE_DATA1));
#endif
}

static int
wait_for_ed(usbdev_t *dev, ed_t *head, int pages)
{
	/* wait for results */
	/* TOTEST: how long to wait?
	 *         give 2s per TD (2 pages) plus another 2s for now
	 */
	int timeout = pages*1000 + 2000;
	while (((head->head_pointer & ~3) != head->tail_pointer) &&
		!(head->head_pointer & 1) &&
		((((td_t*)phys_to_virt(head->head_pointer & ~3))->config
				& TD_CC_MASK) >= TD_CC_NOACCESS) &&
		timeout--) {
		/* don't log every ms */
		if (!(timeout % 100))
		usb_debug("intst: %x; ctrl: %x; cmdst: %x; head: %x -> %x, tail: %x, condition: %x\n",
			OHCI_INST(dev->controller)->opreg->HcInterruptStatus,
			OHCI_INST(dev->controller)->opreg->HcControl,
			OHCI_INST(dev->controller)->opreg->HcCommandStatus,
			head->head_pointer,
			((td_t*)phys_to_virt(head->head_pointer & ~3))->next_td,
			head->tail_pointer,
			(((td_t*)phys_to_virt(head->head_pointer & ~3))->config & TD_CC_MASK) >> TD_CC_SHIFT);
		mdelay(1);
	}
	if (timeout < 0)
		printf("Error: ohci: endpoint "
			"descriptor processing timed out.\n");
	/* Clear the done queue. */
	ohci_process_done_queue(OHCI_INST(dev->controller), 1);

	if (head->head_pointer & 1) {
		usb_debug("HALTED!\n");
		return 1;
	}
	return 0;
}

static void
ohci_free_ed (ed_t *const head)
{
	/* In case the transfer canceled, we have to free unprocessed TDs. */
	while ((head->head_pointer & ~0x3) != head->tail_pointer) {
		/* Save current TD pointer. */
		td_t *const cur_td =
			(td_t*)phys_to_virt(head->head_pointer & ~0x3);
		/* Advance head pointer. */
		head->head_pointer = cur_td->next_td;
		/* Free current TD. */
		free((void *)cur_td);
	}

	/* Always free the dummy TD */
	if ((head->head_pointer & ~0x3) == head->tail_pointer)
		free(phys_to_virt(head->head_pointer & ~0x3));
	/* and the ED. */
	free((void *)head);
}

static int
ohci_control (usbdev_t *dev, direction_t dir, int drlen, void *devreq, int dalen,
	      unsigned char *data)
{
	td_t *cur;

	// pages are specified as 4K in OHCI, so don't use getpagesize()
	int first_page = (unsigned long)data / 4096;
	int last_page = (unsigned long)(data+dalen-1)/4096;
	if (last_page < first_page) last_page = first_page;
	int pages = (dalen==0)?0:(last_page - first_page + 1);

	/* First TD. */
	td_t *const first_td = (td_t *)memalign(sizeof(td_t), sizeof(td_t));
	memset((void *)first_td, 0, sizeof(*first_td));
	cur = first_td;

	cur->config = TD_DIRECTION_SETUP |
		TD_DELAY_INTERRUPT_NOINTR |
		TD_TOGGLE_FROM_TD |
		TD_TOGGLE_DATA0 |
		TD_CC_NOACCESS;
	cur->current_buffer_pointer = virt_to_phys(devreq);
	cur->buffer_end = virt_to_phys(devreq + drlen - 1);

	while (pages > 0) {
		/* One more TD. */
		td_t *const next = (td_t *)memalign(sizeof(td_t), sizeof(td_t));
		memset((void *)next, 0, sizeof(*next));
		/* Linked to the previous. */
		cur->next_td = virt_to_phys(next);
		/* Advance to the new TD. */
		cur = next;

		cur->config = (dir == IN ? TD_DIRECTION_IN : TD_DIRECTION_OUT) |
			TD_DELAY_INTERRUPT_NOINTR |
			TD_TOGGLE_FROM_ED |
			TD_CC_NOACCESS;
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

	/* One more TD. */
	td_t *const next_td = (td_t *)memalign(sizeof(td_t), sizeof(td_t));
	memset((void *)next_td, 0, sizeof(*next_td));
	/* Linked to the previous. */
	cur->next_td = virt_to_phys(next_td);
	/* Advance to the new TD. */
	cur = next_td;
	cur->config = (dir == IN ? TD_DIRECTION_OUT : TD_DIRECTION_IN) |
		TD_DELAY_INTERRUPT_ZERO | /* Write done head after this TD. */
		TD_TOGGLE_FROM_TD |
		TD_TOGGLE_DATA1 |
		TD_CC_NOACCESS;
	cur->current_buffer_pointer = 0;
	cur->buffer_end = 0;

	/* Final dummy TD. */
	td_t *const final_td = (td_t *)memalign(sizeof(td_t), sizeof(td_t));
	memset((void *)final_td, 0, sizeof(*final_td));
	/* Linked to the previous. */
	cur->next_td = virt_to_phys(final_td);

	/* Data structures */
	ed_t *head = memalign(sizeof(ed_t), sizeof(ed_t));
	memset((void*)head, 0, sizeof(*head));
	head->config = (dev->address << ED_FUNC_SHIFT) |
		(0 << ED_EP_SHIFT) |
		(OHCI_FROM_TD << ED_DIR_SHIFT) |
		(dev->speed?ED_LOWSPEED:0) |
		(dev->endpoints[0].maxpacketsize << ED_MPS_SHIFT);
	head->tail_pointer = virt_to_phys(final_td);
	head->head_pointer = virt_to_phys(first_td);

	usb_debug("doing control transfer with %x. first_td at %x\n",
		head->config & ED_FUNC_MASK, virt_to_phys(first_td));

	/* activate schedule */
	OHCI_INST(dev->controller)->opreg->HcControlHeadED = virt_to_phys(head);
	OHCI_INST(dev->controller)->opreg->HcControl |= ControlListEnable;
	OHCI_INST(dev->controller)->opreg->HcCommandStatus = ControlListFilled;

	int failure = wait_for_ed(dev, head,
			(dalen==0)?0:(last_page - first_page + 1));
	/* Wait some frames before and one after disabling list access. */
	mdelay(4);
	OHCI_INST(dev->controller)->opreg->HcControl &= ~ControlListEnable;
	mdelay(1);

	/* free memory */
	ohci_free_ed(head);

	return failure;
}

/* finalize == 1: if data is of packet aligned size, add a zero length packet */
static int
ohci_bulk (endpoint_t *ep, int dalen, u8 *data, int finalize)
{
	int i;
	usb_debug("bulk: %x bytes from %x, finalize: %x, maxpacketsize: %x\n", dalen, data, finalize, ep->maxpacketsize);

	td_t *cur, *next;

	// pages are specified as 4K in OHCI, so don't use getpagesize()
	int first_page = (unsigned long)data / 4096;
	int last_page = (unsigned long)(data+dalen-1)/4096;
	if (last_page < first_page) last_page = first_page;
	int pages = (dalen==0)?0:(last_page - first_page + 1);
	int td_count = (pages+1)/2;

	if (finalize && ((dalen % ep->maxpacketsize) == 0)) {
		td_count++;
	}

	/* First TD. */
	td_t *const first_td = (td_t *)memalign(sizeof(td_t), sizeof(td_t));
	memset((void *)first_td, 0, sizeof(*first_td));
	cur = next = first_td;

	for (i = 0; i < td_count; ++i) {
		/* Advance to next TD. */
		cur = next;
		cur->config = (ep->direction == IN ? TD_DIRECTION_IN : TD_DIRECTION_OUT) |
                        TD_DELAY_INTERRUPT_NOINTR |
                        TD_TOGGLE_FROM_ED |
                        TD_CC_NOACCESS;
		cur->current_buffer_pointer = virt_to_phys(data);
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
		/* One more TD. */
		next = (td_t *)memalign(sizeof(td_t), sizeof(td_t));
		memset((void *)next, 0, sizeof(*next));
		/* Linked to the previous. */
		cur->next_td = virt_to_phys(next);
	}

	/* Write done head after last TD. */
	cur->config &= ~TD_DELAY_INTERRUPT_MASK;
	/* Advance to final, dummy TD. */
	cur = next;

	/* Data structures */
	ed_t *head = memalign(sizeof(ed_t), sizeof(ed_t));
	memset((void*)head, 0, sizeof(*head));
	head->config = (ep->dev->address << ED_FUNC_SHIFT) |
		((ep->endpoint & 0xf) << ED_EP_SHIFT) |
		(((ep->direction==IN)?OHCI_IN:OHCI_OUT) << ED_DIR_SHIFT) |
		(ep->dev->speed?ED_LOWSPEED:0) |
		(ep->maxpacketsize << ED_MPS_SHIFT);
	head->tail_pointer = virt_to_phys(cur);
	head->head_pointer = virt_to_phys(first_td) | (ep->toggle?ED_TOGGLE:0);

	usb_debug("doing bulk transfer with %x(%x). first_td at %x, last %x\n",
		head->config & ED_FUNC_MASK,
		(head->config & ED_EP_MASK) >> ED_EP_SHIFT,
		virt_to_phys(first_td), virt_to_phys(cur));

	/* activate schedule */
	OHCI_INST(ep->dev->controller)->opreg->HcBulkHeadED = virt_to_phys(head);
	OHCI_INST(ep->dev->controller)->opreg->HcControl |= BulkListEnable;
	OHCI_INST(ep->dev->controller)->opreg->HcCommandStatus = BulkListFilled;

	int failure = wait_for_ed(ep->dev, head,
			(dalen==0)?0:(last_page - first_page + 1));
	/* Wait some frames before and one after disabling list access. */
	mdelay(4);
	OHCI_INST(ep->dev->controller)->opreg->HcControl &= ~BulkListEnable;
	mdelay(1);

	ep->toggle = head->head_pointer & ED_TOGGLE;

	/* free memory */
	ohci_free_ed(head);

	if (failure) {
		/* try cleanup */
		clear_stall(ep);
	}

	return failure;
}


struct _intr_queue;

struct _intrq_td {
	volatile td_t		td;
	u8			*data;
	struct _intrq_td	*next;
	struct _intr_queue	*intrq;
};

struct _intr_queue {
	volatile ed_t		ed;
	struct _intrq_td	*head;
	struct _intrq_td	*tail;
	u8			*data;
	int			reqsize;
	endpoint_t		*endp;
	unsigned int		remaining_tds;
};

typedef struct _intrq_td intrq_td_t;
typedef struct _intr_queue intr_queue_t;

#define INTRQ_TD_FROM_TD(x) ((intrq_td_t *)x)

static void
ohci_fill_intrq_td(intrq_td_t *const td, intr_queue_t *const intrq,
		   u8 *const data)
{
	memset(td, 0, sizeof(*td));
	td->td.config = TD_QUEUETYPE_INTR |
		(intrq->endp->direction == IN
			? TD_DIRECTION_IN : TD_DIRECTION_OUT) |
		TD_DELAY_INTERRUPT_ZERO |
		TD_TOGGLE_FROM_ED |
		TD_CC_NOACCESS;
	td->td.current_buffer_pointer = virt_to_phys(data);
	td->td.buffer_end = td->td.current_buffer_pointer + intrq->reqsize - 1;
	td->intrq = intrq;
	td->data = data;
}

/* create and hook-up an intr queue into device schedule */
static void *
ohci_create_intr_queue(endpoint_t *const ep, const int reqsize,
		       const int reqcount, const int reqtiming)
{
	int i;
	intrq_td_t *first_td = NULL, *last_td = NULL;

	if (reqsize > 4096)
		return NULL;

	intr_queue_t *const intrq =
		(intr_queue_t *)memalign(sizeof(intrq->ed), sizeof(*intrq));
	memset(intrq, 0, sizeof(*intrq));
	intrq->data = (u8 *)malloc(reqcount * reqsize);
	intrq->reqsize = reqsize;
	intrq->endp = ep;

	/* Create #reqcount TDs. */
	u8 *cur_data = intrq->data;
	for (i = 0; i < reqcount; ++i) {
		intrq_td_t *const td = memalign(sizeof(td->td), sizeof(*td));
		++intrq->remaining_tds;
		ohci_fill_intrq_td(td, intrq, cur_data);
		cur_data += reqsize;
		if (!first_td)
			first_td = td;
		else
			last_td->td.next_td = virt_to_phys(&td->td);
		last_td = td;
	}

	/* Create last, dummy TD. */
	intrq_td_t *dummy_td = memalign(sizeof(dummy_td->td), sizeof(*dummy_td));
	memset(dummy_td, 0, sizeof(*dummy_td));
	dummy_td->intrq = intrq;
	if (last_td)
		last_td->td.next_td = virt_to_phys(&dummy_td->td);
	last_td = dummy_td;

	/* Initialize ED. */
	intrq->ed.config =  (ep->dev->address << ED_FUNC_SHIFT) |
		((ep->endpoint & 0xf) << ED_EP_SHIFT) |
		(((ep->direction == IN) ? OHCI_IN : OHCI_OUT) << ED_DIR_SHIFT) |
		(ep->dev->speed ? ED_LOWSPEED : 0) |
		(ep->maxpacketsize << ED_MPS_SHIFT);
	intrq->ed.tail_pointer = virt_to_phys(last_td);
	intrq->ed.head_pointer = virt_to_phys(first_td) |
		(ep->toggle ? ED_TOGGLE : 0);

	/* Insert ED into periodic table. */
	int nothing_placed	= 1;
	ohci_t *const ohci	= OHCI_INST(ep->dev->controller);
	u32 *const intr_table	= ohci->hcca->HccaInterruptTable;
	const u32 dummy_ptr	= virt_to_phys(ohci->periodic_ed);
	for (i = 0; i < 32; i += reqtiming) {
		/* Advance to the next free position. */
		while ((i < 32) && (intr_table[i] != dummy_ptr)) ++i;
		if (i < 32) {
			intr_table[i] = virt_to_phys(&intrq->ed);
			nothing_placed = 0;
		}
	}
	if (nothing_placed) {
		printf("Error: Failed to place ohci interrupt endpoint "
			"descriptor into periodic table: no space left\n");
		ohci_destroy_intr_queue(ep, intrq);
		return NULL;
	}

	return intrq;
}

/* remove queue from device schedule, dropping all data that came in */
static void
ohci_destroy_intr_queue(endpoint_t *const ep, void *const q_)
{
	intr_queue_t *const intrq = (intr_queue_t *)q_;

	int i;

	/* Remove interrupt queue from periodic table. */
	ohci_t *const ohci	= OHCI_INST(ep->dev->controller);
	u32 *const intr_table	= ohci->hcca->HccaInterruptTable;
	for (i=0; i < 32; ++i) {
		if (intr_table[i] == virt_to_phys(intrq))
			intr_table[i] = virt_to_phys(ohci->periodic_ed);
	}
	/* Wait for frame to finish. */
	mdelay(1);

	/* Free unprocessed TDs. */
	while ((intrq->ed.head_pointer & ~0x3) != intrq->ed.tail_pointer) {
		td_t *const cur_td =
			(td_t *)phys_to_virt(intrq->ed.head_pointer & ~0x3);
		intrq->ed.head_pointer = cur_td->next_td;
		free(INTRQ_TD_FROM_TD(cur_td));
		--intrq->remaining_tds;
	}
	/* Free final, dummy TD. */
	free(phys_to_virt(intrq->ed.head_pointer & ~0x3));
	/* Free data buffer. */
	free(intrq->data);

	/* Process done queue and free processed TDs. */
	ohci_process_done_queue(ohci, 1);
	while (intrq->head) {
		intrq_td_t *const cur_td = intrq->head;
		intrq->head = intrq->head->next;
		free(cur_td);
		--intrq->remaining_tds;
	}
	if (intrq->remaining_tds) {
		printf("error: ohci_destroy_intr_queue(): "
			"freed all but %d TDs.\n", intrq->remaining_tds);
	}

	free(intrq);

	/* Save data toggle. */
	ep->toggle = intrq->ed.head_pointer & ED_TOGGLE;
}

/* read one intr-packet from queue, if available. extend the queue for new input.
   return NULL if nothing new available.
   Recommended use: while (data=poll_intr_queue(q)) process(data);
 */
static u8 *
ohci_poll_intr_queue(void *const q_)
{
	intr_queue_t *const intrq = (intr_queue_t *)q_;

	u8 *data = NULL;

	/* Process done queue first, then check if we have work to do. */
	ohci_process_done_queue(OHCI_INST(intrq->endp->dev->controller), 0);

	if (intrq->head) {
		/* Save pointer to processed TD and advance. */
		intrq_td_t *const cur_td = intrq->head;
		intrq->head = cur_td->next;

		/* Return data buffer of this TD. */
		data = cur_td->data;

		/* Requeue this TD (i.e. copy to dummy and requeue as dummy). */
		intrq_td_t *const dummy_td =
			INTRQ_TD_FROM_TD(phys_to_virt(intrq->ed.tail_pointer));
		ohci_fill_intrq_td(dummy_td, intrq, cur_td->data);
		/* Reset all but intrq pointer (i.e. init as dummy). */
		memset(cur_td, 0, sizeof(*cur_td));
		cur_td->intrq = intrq;
		/* Insert into interrupt queue as dummy. */
		dummy_td->td.next_td = virt_to_phys(&cur_td->td);
		intrq->ed.tail_pointer = virt_to_phys(&cur_td->td);
	}

	return data;
}

static void
ohci_process_done_queue(ohci_t *const ohci, const int spew_debug)
{
	int i, j;

	/* Temporary queue of interrupt queue TDs (to reverse order). */
	intrq_td_t *temp_tdq = NULL;

	/* Check if done head has been written. */
	if (!(ohci->opreg->HcInterruptStatus & WritebackDoneHead))
		return;
	/* Fetch current done head.
	   Lsb is only interesting for hw interrupts. */
	u32 phys_done_queue = ohci->hcca->HccaDoneHead & ~1;
	/* Tell host controller, he may overwrite the done head pointer. */
	ohci->opreg->HcInterruptStatus = WritebackDoneHead;

	i = 0;
	/* Process done queue (it's in reversed order). */
	while (phys_done_queue) {
		td_t *const done_td = (td_t *)phys_to_virt(phys_done_queue);

		/* Advance pointer to next TD. */
		phys_done_queue = done_td->next_td;

		switch (done_td->config & TD_QUEUETYPE_MASK) {
		case TD_QUEUETYPE_ASYNC:
			/* Free processed async TDs. */
			free((void *)done_td);
			break;
		case TD_QUEUETYPE_INTR:
			/* Save done TD if it comes from an interrupt queue. */
			INTRQ_TD_FROM_TD(done_td)->next = temp_tdq;
			temp_tdq = INTRQ_TD_FROM_TD(done_td);
			break;
		default:
			break;
		}
		++i;
	}
	if (spew_debug)
		usb_debug("Processed %d done TDs.\n", i);

	j = 0;
	/* Process interrupt queue TDs in right order. */
	while (temp_tdq) {
		/* Save pointer of current TD and advance. */
		intrq_td_t *const cur_td = temp_tdq;
		temp_tdq = temp_tdq->next;

		/* The interrupt queue for the current TD. */
		intr_queue_t *const intrq = cur_td->intrq;
		/* Append to interrupt queue. */
		if (!intrq->head) {
			/* First element. */
			intrq->head = intrq->tail = cur_td;
		} else {
			/* Insert at tail. */
			intrq->tail->next = cur_td;
			intrq->tail = cur_td;
		}
		/* It's always the last element. */
		cur_td->next = NULL;
		++j;
	}
	if (spew_debug)
		usb_debug("processed %d done tds, %d intr tds thereof.\n", i, j);
}

