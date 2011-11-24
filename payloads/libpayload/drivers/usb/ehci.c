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

static void ehci_shutdown (hci_t *controller)
{
	EHCI_INST(controller)->operation->configflag = 0;
}

enum { EHCI_OUT=0, EHCI_IN=1, EHCI_SETUP=2 };

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
		while ((cur->token & QTD_ACTIVE) && !(cur->token & QTD_HALTED)) udelay(60);
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

static int ehci_bulk (endpoint_t *ep, int size, u8 *data, int finalize)
{
	int result = 0;
	int endp = ep->endpoint & 0xf;
	int pid = (ep->direction==IN)?EHCI_IN:EHCI_OUT;

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
	qh->epcaps = 3 << QH_PIPE_MULTIPLIER_SHIFT;

	qh->td.next_qtd = virt_to_phys(head);
	qh->td.token |= (ep->toggle?QTD_TOGGLE_DATA1:0);
	head->token |= (ep->toggle?QTD_TOGGLE_DATA1:0);

	/* hook up QH */
	EHCI_INST(ep->dev->controller)->operation->asynclistaddr = virt_to_phys(qh);

	/* start async schedule */
	EHCI_INST(ep->dev->controller)->operation->usbcmd |= HC_OP_ASYNC_SCHED_EN;
	while (!(EHCI_INST(ep->dev->controller)->operation->usbsts & HC_OP_ASYNC_SCHED_STAT)) ; /* wait */

	/* wait for result */
	result = wait_for_tds(head);

	/* disable async schedule */
	EHCI_INST(ep->dev->controller)->operation->usbcmd &= ~HC_OP_ASYNC_SCHED_EN;
	while (EHCI_INST(ep->dev->controller)->operation->usbsts & HC_OP_ASYNC_SCHED_STAT) ; /* wait */

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
		(0 << QH_NON_HS_CTRL_EP_SHIFT) | /* no non-HS device support yet */
		(0 << QH_NAK_CNT_SHIFT);
	qh->epcaps = 3 << QH_PIPE_MULTIPLIER_SHIFT;
	qh->td.next_qtd = virt_to_phys(head);

	/* hook up QH */
	EHCI_INST(dev->controller)->operation->asynclistaddr = virt_to_phys(qh);

	/* start async schedule */
	EHCI_INST(dev->controller)->operation->usbcmd |= HC_OP_ASYNC_SCHED_EN;
	while (!(EHCI_INST(dev->controller)->operation->usbsts & HC_OP_ASYNC_SCHED_STAT)) ; /* wait */

	result = wait_for_tds(head);

	/* disable async schedule */
	EHCI_INST(dev->controller)->operation->usbcmd &= ~HC_OP_ASYNC_SCHED_EN;
	while (EHCI_INST(dev->controller)->operation->usbsts & HC_OP_ASYNC_SCHED_STAT) ; /* wait */

	free_qh_and_tds(qh, head);
	return result;
}

static void* ehci_create_intr_queue (endpoint_t *ep, int reqsize, int reqcount, int reqtiming)
{
	return NULL;
}

static void ehci_destroy_intr_queue (endpoint_t *ep, void *queue)
{
}

static u8* ehci_poll_intr_queue (void *queue)
{
	return NULL;
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

	/* TODO lots of stuff missing */

	controller->devices[0]->controller = controller;
	controller->devices[0]->init = ehci_rh_init;
	controller->devices[0]->init (controller->devices[0]);

	return controller;
}
