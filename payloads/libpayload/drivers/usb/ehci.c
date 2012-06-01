/*
 * This file is part of the libpayload project.
 *
 * Copyright (C) 2010 coresystems GmbH
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
#include "ehci.h"
#include "ehci_private.h"

static void dump_td(u32 addr)
{
	qtd_t *td = phys_to_virt(addr);
	debug("td at phys(%x): status: %x\n\n", addr, td->token & QTD_STATUS_MASK);
	debug("-   cerr: %x, total_len: %x\n\n", (td->token & QTD_CERR_MASK) >> QTD_CERR_SHIFT,
		(td->token & QTD_TOTAL_LEN_MASK) >> QTD_TOTAL_LEN_SHIFT);
}

static void ehci_start (hci_t *controller)
{
	EHCI_INST(controller)->operation->usbcmd |= HC_OP_RS;
}

static void ehci_stop (hci_t *controller)
{
	EHCI_INST(controller)->operation->usbcmd &= ~HC_OP_RS;
}

static void ehci_reset (hci_t *controller)
{

}

static int ehci_set_periodic_schedule(ehci_t *ehcic, int enable)
{
	/* Set periodic schedule status. */
	if (enable)
		ehcic->operation->usbcmd |= HC_OP_PERIODIC_SCHED_EN;
	else
		ehcic->operation->usbcmd &= ~HC_OP_PERIODIC_SCHED_EN;
	/* Wait for the controller to accept periodic schedule status.
	 * This shouldn't take too long, but we should timeout nevertheless.
	 */
	enable = enable ? HC_OP_PERIODIC_SCHED_STAT : 0;
	int timeout = 100; /* time out after 100ms */
	while (((ehcic->operation->usbsts & HC_OP_PERIODIC_SCHED_STAT) != enable)
			&& timeout--)
		mdelay(1);
	if (timeout < 0) {
		debug("ehci periodic schedule status change timed out.\n");
		return 1;
	}
	return 0;
}

static void ehci_shutdown (hci_t *controller)
{
	/* Make sure periodic schedule is disabled */
	ehci_set_periodic_schedule(EHCI_INST(controller), 0);
	/* Free periodic frame list */
	free(phys_to_virt(EHCI_INST(controller)->operation->periodiclistbase));

	EHCI_INST(controller)->operation->configflag = 0;
}

enum { EHCI_OUT=0, EHCI_IN=1, EHCI_SETUP=2 };

/*
 * returns the address of the closest USB2.0 hub, which is responsible for
 * split transactions, along with the number of the used downstream port
 */
static int closest_usb2_hub(const usbdev_t *dev, int *const addr, int *const port)
{
	const usbdev_t *usb1dev;
	do {
		usb1dev = dev;
		if ((dev->hub > 0) && (dev->hub < 128))
			dev = dev->controller->devices[dev->hub];
		else
			dev = NULL;
	} while (dev && (dev->speed < 2));
	if (dev) {
		*addr = usb1dev->hub;
		*port = usb1dev->port;
		return 0;
	} else {
		debug("ehci: Couldn't find closest USB2.0 hub.\n");
		return 1;
	}
}

/* returns handled bytes. assumes that the fields it writes are empty on entry */
static int fill_td(qtd_t *td, void* data, int datalen)
{
	u32 total_len = 0;
	u32 page_no = 0;

	u32 start = virt_to_phys(data);
	u32 page = start & ~4095;
	u32 offset = start & 4095;
	u32 page_len = 4096 - offset;

	td->token |= 0 << QTD_CPAGE_SHIFT;
	td->bufptrs[page_no++] = start;

	if (datalen <= page_len) {
		total_len = datalen;
	} else {
		datalen -= page_len;
		total_len += page_len;

		while (page_no < 5) {
			/* we have a continguous mapping between virtual and physical memory */
			page += 4096;

			td->bufptrs[page_no++] = page;
			if (datalen <= 4096) {
				total_len += datalen;
				break;
			}
			datalen -= 4096;
			total_len += 4096;
		}
	}
	td->token |= total_len << QTD_TOTAL_LEN_SHIFT;
	return total_len;
}

/* free up data structures */
static void free_qh_and_tds(ehci_qh_t *qh, qtd_t *cur)
{
	qtd_t *next;
	while (cur) {
		next = (qtd_t*)phys_to_virt(cur->next_qtd & ~31);
		free(cur);
		cur = next;
	}
	free(qh);
}

static int wait_for_tds(qtd_t *head)
{
	int result = 0;
	qtd_t *cur = head;
	while (1) {
		if (0) dump_td(virt_to_phys(cur));

		/* wait for results */
		/* how long to wait?
		 * tested with some USB2.0 flash sticks:
		 * TUR turn around took
		 *   about 2s for the slowest (14cd:121c)
		 *   max. 250ms for the others
		 * slowest non-TUR turn around took about 1.3s
		 * try 2s for now as a failed TUR is not fatal
		 */
		int timeout = 40000; /* time out after 40000 * 50us == 2s */
		while ((cur->token & QTD_ACTIVE) && !(cur->token & QTD_HALTED)
				&& timeout--)
			udelay(50);
		if (timeout < 0) {
			printf("Error: ehci: queue transfer "
				"processing timed out.\n");
			return 1;
		}
		if (cur->token & QTD_HALTED) {
			printf("ERROR with packet\n");
			dump_td(virt_to_phys(cur));
			debug("-----------------\n");
			return 1;
		}
		if (cur->next_qtd & 1) {
			return 0;
		}
		if (0) dump_td(virt_to_phys(cur));
		/* helps debugging the TD chain */
		if (0) debug("\nmoving from %x to %x\n", cur, phys_to_virt(cur->next_qtd));
		cur = phys_to_virt(cur->next_qtd);
	}
	return result;
}

static int ehci_set_async_schedule(ehci_t *ehcic, int enable)
{
	/* Set async schedule status. */
	if (enable)
		ehcic->operation->usbcmd |= HC_OP_ASYNC_SCHED_EN;
	else
		ehcic->operation->usbcmd &= ~HC_OP_ASYNC_SCHED_EN;
	/* Wait for the controller to accept async schedule status.
	 * This shouldn't take too long, but we should timeout nevertheless.
	 */
	enable = enable ? HC_OP_ASYNC_SCHED_STAT : 0;
	int timeout = 100; /* time out after 100ms */
	while (((ehcic->operation->usbsts & HC_OP_ASYNC_SCHED_STAT) != enable)
			&& timeout--)
		mdelay(1);
	if (timeout < 0) {
		debug("ehci async schedule status change timed out.\n");
		return 1;
	}
	return 0;
}

static int ehci_process_async_schedule(
		ehci_t *ehcic, ehci_qh_t *qhead, qtd_t *head)
{
	int result;

	/* make sure async schedule is disabled */
	if (ehci_set_async_schedule(ehcic, 0)) return 1;

	/* hook up QH */
	ehcic->operation->asynclistaddr = virt_to_phys(qhead);

	/* start async schedule */
	if (ehci_set_async_schedule(ehcic, 1)) return 1;

	/* wait for result */
	result = wait_for_tds(head);

	/* disable async schedule */
	ehci_set_async_schedule(ehcic, 0);

	return result;
}

static int ehci_bulk (endpoint_t *ep, int size, u8 *data, int finalize)
{
	int result = 0;
	int endp = ep->endpoint & 0xf;
	int pid = (ep->direction==IN)?EHCI_IN:EHCI_OUT;

	int hubaddr = 0, hubport = 0;
	if (ep->dev->speed < 2) {
		/* we need a split transaction */
		if (closest_usb2_hub(ep->dev, &hubaddr, &hubport))
			return 1;
	}

	qtd_t *head = memalign(32, sizeof(qtd_t));
	qtd_t *cur = head;
	while (1) {
		memset(cur, 0, sizeof(qtd_t));
		cur->token = QTD_ACTIVE |
			(pid << QTD_PID_SHIFT) |
			(0 << QTD_CERR_SHIFT);
		u32 chunk = fill_td(cur, data, size);
		size -= chunk;
		data += chunk;

		cur->alt_next_qtd = QTD_TERMINATE;
		if (size == 0) {
			cur->next_qtd = virt_to_phys(0) | QTD_TERMINATE;
			break;
		} else {
			qtd_t *next = memalign(32, sizeof(qtd_t));
			cur->next_qtd = virt_to_phys(next);
			cur = next;
		}
	}

	/* create QH */
	ehci_qh_t *qh = memalign(32, sizeof(ehci_qh_t));
	memset(qh, 0, sizeof(ehci_qh_t));
	qh->horiz_link_ptr = virt_to_phys(qh) | QH_QH;
	qh->epchar = ep->dev->address |
		(endp << QH_EP_SHIFT) |
		(ep->dev->speed << QH_EPS_SHIFT) |
		(0 << QH_DTC_SHIFT) |
		(1 << QH_RECLAIM_HEAD_SHIFT) |
		(ep->maxpacketsize << QH_MPS_SHIFT) |
		(0 << QH_NAK_CNT_SHIFT);
	qh->epcaps = (3 << QH_PIPE_MULTIPLIER_SHIFT) |
		(hubport << QH_PORT_NUMBER_SHIFT) |
		(hubaddr << QH_HUB_ADDRESS_SHIFT);

	qh->td.next_qtd = virt_to_phys(head);
	qh->td.token |= (ep->toggle?QTD_TOGGLE_DATA1:0);
	head->token |= (ep->toggle?QTD_TOGGLE_DATA1:0);

	result = ehci_process_async_schedule(
			EHCI_INST(ep->dev->controller), qh, head);

	ep->toggle = (cur->token & QTD_TOGGLE_MASK) >> QTD_TOGGLE_SHIFT;

	free_qh_and_tds(qh, head);
	return result;
}


/* FIXME: Handle control transfers as 3 QHs, so the 2nd stage can be >0x4000 bytes */
static int ehci_control (usbdev_t *dev, direction_t dir, int drlen, void *devreq,
			 int dalen, u8 *data)
{
	int endp = 0; // this is control. always 0 (for now)
	int toggle = 0;
	int mlen = dev->endpoints[0].maxpacketsize;
	int result = 0;

	int hubaddr = 0, hubport = 0, non_hs_ctrl_ep = 0;
	if (dev->speed < 2) {
		/* we need a split transaction */
		if (closest_usb2_hub(dev, &hubaddr, &hubport))
			return 1;
		non_hs_ctrl_ep = 1;
	}

	/* create qTDs */
	qtd_t *head = memalign(32, sizeof(qtd_t));
	qtd_t *cur = head;
	memset(cur, 0, sizeof(qtd_t));
	cur->token = QTD_ACTIVE |
		(toggle?QTD_TOGGLE_DATA1:0) |
		(EHCI_SETUP << QTD_PID_SHIFT) |
		(3 << QTD_CERR_SHIFT);
	if (fill_td(cur, devreq, drlen) != drlen) {
		printf("ERROR: couldn't send the entire device request\n");
	}
	qtd_t *next = memalign(32, sizeof(qtd_t));
	cur->next_qtd = virt_to_phys(next);
	cur->alt_next_qtd = QTD_TERMINATE;

	/* FIXME: We're limited to 16-20K (depending on alignment) for payload for now.
	 * Figure out, how toggle can be set sensibly in this scenario */
	if (dalen > 0) {
		toggle ^= 1;
		cur = next;
		memset(cur, 0, sizeof(qtd_t));
		cur->token = QTD_ACTIVE |
			(toggle?QTD_TOGGLE_DATA1:0) |
			(((dir == OUT)?EHCI_OUT:EHCI_IN) << QTD_PID_SHIFT) |
			(3 << QTD_CERR_SHIFT);
		if (fill_td(cur, data, dalen) != dalen) {
			printf("ERROR: couldn't send the entire control payload\n");
		}
		next = memalign(32, sizeof(qtd_t));
		cur->next_qtd = virt_to_phys(next);
		cur->alt_next_qtd = QTD_TERMINATE;
	}

	toggle = 1;
	cur = next;
	memset(cur, 0, sizeof(qtd_t));
	cur->token = QTD_ACTIVE |
		(toggle?QTD_TOGGLE_DATA1:QTD_TOGGLE_DATA0) |
		((dir == OUT)?EHCI_IN:EHCI_OUT) << QTD_PID_SHIFT |
		(0 << QTD_CERR_SHIFT);
	fill_td(cur, NULL, 0);
	cur->next_qtd = virt_to_phys(0) | QTD_TERMINATE;
	cur->alt_next_qtd = QTD_TERMINATE;

	/* create QH */
	ehci_qh_t *qh = memalign(32, sizeof(ehci_qh_t));
	memset(qh, 0, sizeof(ehci_qh_t));
	qh->horiz_link_ptr = virt_to_phys(qh) | QH_QH;
	qh->epchar = dev->address |
		(endp << QH_EP_SHIFT) |
		(dev->speed << QH_EPS_SHIFT) |
		(1 << QH_DTC_SHIFT) | /* ctrl transfers are special: take toggle bit from TD */
		(1 << QH_RECLAIM_HEAD_SHIFT) |
		(mlen << QH_MPS_SHIFT) |
		(non_hs_ctrl_ep << QH_NON_HS_CTRL_EP_SHIFT) |
		(0 << QH_NAK_CNT_SHIFT);
	qh->epcaps = (3 << QH_PIPE_MULTIPLIER_SHIFT) |
		(hubport << QH_PORT_NUMBER_SHIFT) |
		(hubaddr << QH_HUB_ADDRESS_SHIFT);
	qh->td.next_qtd = virt_to_phys(head);

	result = ehci_process_async_schedule(
			EHCI_INST(dev->controller), qh, head);

	free_qh_and_tds(qh, head);
	return result;
}


typedef struct _intr_qtd_t intr_qtd_t;

struct _intr_qtd_t {
	volatile qtd_t	td;
	u8		*data;
	intr_qtd_t	*next;
};

typedef struct {
	volatile ehci_qh_t	qh;
	intr_qtd_t		*head;
	intr_qtd_t		*tail;
	u8			*data;
	endpoint_t		*endp;
	int			reqsize;
} intr_queue_t;

static void fill_intr_queue_td(
		intr_queue_t *const intrq,
		intr_qtd_t *const intr_qtd,
		u8 *const data)
{
	const int pid = (intrq->endp->direction == IN) ? EHCI_IN
		: (intrq->endp->direction == OUT) ? EHCI_OUT
		: EHCI_SETUP;
	const int cerr = (intrq->endp->dev->speed < 2) ? 1 : 0;

	memset(intr_qtd, 0, sizeof(*intr_qtd));
	intr_qtd->td.next_qtd = QTD_TERMINATE;
	intr_qtd->td.alt_next_qtd = QTD_TERMINATE;
	intr_qtd->td.token = QTD_ACTIVE |
		(pid << QTD_PID_SHIFT) |
		(cerr << QTD_CERR_SHIFT) |
		((intrq->endp->toggle & 1) << QTD_TOGGLE_SHIFT);
	fill_td(&intr_qtd->td, data, intrq->reqsize);
	intr_qtd->data = data;
	intr_qtd->next = NULL;

	intrq->endp->toggle ^= 1;
}

static void ehci_destroy_intr_queue(endpoint_t *const, void *const);

static void *ehci_create_intr_queue(
		endpoint_t *const ep,
		const int reqsize,
		int reqcount,
		const int reqtiming)
{
	int i;

	if ((reqsize > (4 * 4096 + 1)) || /* the maximum for arbitrary aligned
					     data in five 4096 byte pages */
			(reqtiming > 1024))
		return NULL;
	if (reqcount < 2) /* we need at least 2:
			     one for processing, one for the hc to advance to */
		reqcount = 2;

	int hubaddr = 0, hubport = 0;
	if (ep->dev->speed < 2) {
		/* we need a split transaction */
		if (closest_usb2_hub(ep->dev, &hubaddr, &hubport))
			return NULL;
	}

	intr_queue_t *const intrq =
		(intr_queue_t *)memalign(32, sizeof(intr_queue_t));
	u8 *data = (u8 *)malloc(reqsize * reqcount);
	if (!intrq || !data)
		fatal("Not enough memory to create USB interrupt queue.\n");
	intrq->data = data;
	intrq->endp = ep;
	intrq->reqsize = reqsize;

	/* create #reqcount transfer descriptors (qTDs) */
	intrq->head = (intr_qtd_t *)memalign(32, sizeof(intr_qtd_t));
	intr_qtd_t *cur_td = intrq->head;
	for (i = 0; i < reqcount; ++i) {
		fill_intr_queue_td(intrq, cur_td, data);
		data += reqsize;
		if (i < reqcount - 1) {
			/* create one more qTD */
			intr_qtd_t *const next_td =
				(intr_qtd_t *)memalign(32, sizeof(intr_qtd_t));
			cur_td->td.next_qtd = virt_to_phys(&next_td->td);
			cur_td->next = next_td;
			cur_td = next_td;
		}
	}
	intrq->tail = cur_td;

	/* initialize QH */
	const int endp = ep->endpoint & 0xf;
	memset(&intrq->qh, 0, sizeof(intrq->qh));
	intrq->qh.horiz_link_ptr = PS_TERMINATE;
	intrq->qh.epchar = ep->dev->address |
		(endp << QH_EP_SHIFT) |
		(ep->dev->speed << QH_EPS_SHIFT) |
		(1 << QH_DTC_SHIFT) |
		(0 << QH_RECLAIM_HEAD_SHIFT) |
		(ep->maxpacketsize << QH_MPS_SHIFT) |
		(0 << QH_NAK_CNT_SHIFT);
	intrq->qh.epcaps = (1 << QH_PIPE_MULTIPLIER_SHIFT) |
		(hubport << QH_PORT_NUMBER_SHIFT) |
		(hubaddr << QH_HUB_ADDRESS_SHIFT) |
		(0xfe << QH_UFRAME_CMASK_SHIFT) |
		1 /* uFrame S-mask */;
	intrq->qh.td.next_qtd = virt_to_phys(&intrq->head->td);

	/* insert QH into periodic schedule */
	int nothing_placed = 1;
	u32 *const ps = (u32 *)phys_to_virt(EHCI_INST(ep->dev->controller)
						->operation->periodiclistbase);
	for (i = 0; i < 1024; i += reqtiming) {
		/* advance to the next free position */
		while ((i < 1024) && !(ps[i] & PS_TERMINATE)) ++i;
		if (i < 1024) {
			ps[i] =	virt_to_phys(&intrq->qh) | PS_TYPE_QH;
			nothing_placed = 0;
		}
	}
	if (nothing_placed) {
		printf("Error: Failed to place ehci interrupt queue head "
				"into periodic schedule: no space left\n");
		ehci_destroy_intr_queue(ep, intrq);
		return NULL;
	}

	return intrq;
}

static void ehci_destroy_intr_queue(endpoint_t *const ep, void *const queue)
{
	intr_queue_t *const intrq = (intr_queue_t *)queue;

	/* remove QH from periodic schedule */
	int i;
	u32 *const ps = (u32 *)phys_to_virt(EHCI_INST(
			ep->dev->controller)->operation->periodiclistbase);
	for (i = 0; i < 1024; ++i) {
		if ((ps[i] & PS_PTR_MASK) == virt_to_phys(&intrq->qh))
			ps[i] = PS_TERMINATE;
	}

	/* wait 1ms for frame to end */
	mdelay(1);

	while (intrq->head) {
		/* disable qTD and destroy list */
		intrq->head->td.next_qtd = QTD_TERMINATE;
		intrq->head->td.token &= ~QTD_ACTIVE;

		/* save and advance head ptr */
		intr_qtd_t *const to_free = intrq->head;
		intrq->head = intrq->head->next;

		/* free current interrupt qTD */
		free(to_free);
	}
	free(intrq->data);
	free(intrq);
}

static u8 *ehci_poll_intr_queue(void *const queue)
{
	intr_queue_t *const intrq = (intr_queue_t *)queue;

	u8 *ret = NULL;

	/* process if head qTD is inactive AND QH has been moved forward */
	if (!(intrq->head->td.token & QTD_ACTIVE) &&
			(intrq->qh.current_td_ptr !=
			 virt_to_phys(&intrq->head->td))) {
		if (!(intrq->head->td.token & QTD_STATUS_MASK))
			ret = intrq->head->data;
		else
			debug("ehci_poll_intr_queue: transfer failed, "
				"status == 0x%02x\n",
				intrq->head->td.token & QTD_STATUS_MASK);

		/* save and advance our head ptr */
		intr_qtd_t *const new_td = intrq->head;
		intrq->head = intrq->head->next;

		/* reuse executed qTD */
		fill_intr_queue_td(intrq, new_td, new_td->data);

		/* at last insert reused qTD at the
		 * end and advance our tail ptr */
		intrq->tail->td.next_qtd = virt_to_phys(&new_td->td);
		intrq->tail->next = new_td;
		intrq->tail = intrq->tail->next;
	}
	return ret;
}

hci_t *
ehci_init (pcidev_t addr)
{
	int i;
	hci_t *controller = new_controller ();

	if (!controller)
		fatal("Could not create USB controller instance.\n");

	controller->instance = malloc (sizeof (ehci_t));
	if(!controller->instance)
		fatal("Not enough memory creating USB controller instance.\n");

#define PCI_COMMAND 4
#define PCI_COMMAND_IO 1
#define PCI_COMMAND_MEMORY 2
#define PCI_COMMAND_MASTER 4

	u32 pci_command = pci_read_config32(addr, PCI_COMMAND);
	pci_command = (pci_command | PCI_COMMAND_MEMORY) & ~PCI_COMMAND_IO ;
	pci_write_config32(addr, PCI_COMMAND, pci_command);

	controller->start = ehci_start;
	controller->stop = ehci_stop;
	controller->reset = ehci_reset;
	controller->shutdown = ehci_shutdown;
	controller->bulk = ehci_bulk;
	controller->control = ehci_control;
	controller->create_intr_queue = ehci_create_intr_queue;
	controller->destroy_intr_queue = ehci_destroy_intr_queue;
	controller->poll_intr_queue = ehci_poll_intr_queue;
	controller->bus_address = addr;
	for (i = 0; i < 128; i++) {
		controller->devices[i] = 0;
	}
	init_device_entry (controller, 0);

	EHCI_INST(controller)->capabilities = phys_to_virt(pci_read_config32(addr, USBBASE));
	EHCI_INST(controller)->operation = (hc_op_t *)(phys_to_virt(pci_read_config32(addr, USBBASE)) + EHCI_INST(controller)->capabilities->caplength);

	/* default value for frame length adjust */
	pci_write_config8(addr, FLADJ, FLADJ_framelength(60000));

	/* Enable operation of controller */
	controller->start(controller);

	/* take over all ports. USB1 should be blind now */
	EHCI_INST(controller)->operation->configflag = 1;

	/* Initialize periodic frame list */
	/* 1024 32-bit pointers, 4kb aligned */
	u32 *const periodic_list = (u32 *)memalign(4096, 1024 * sizeof(u32));
	if (!periodic_list)
		fatal("Not enough memory creating EHCI periodic frame list.\n");
	for (i = 0; i < 1024; ++i)
		periodic_list[i] = PS_TERMINATE;

	/* Make sure periodic schedule is disabled */
	ehci_set_periodic_schedule(EHCI_INST(controller), 0);
	/* Set periodic frame list pointer */
	EHCI_INST(controller)->operation->periodiclistbase =
		virt_to_phys(periodic_list);
	/* Enable use of periodic schedule */
	ehci_set_periodic_schedule(EHCI_INST(controller), 1);

	/* TODO lots of stuff missing */

	controller->devices[0]->controller = controller;
	controller->devices[0]->init = ehci_rh_init;
	controller->devices[0]->init (controller->devices[0]);

	return controller;
}
