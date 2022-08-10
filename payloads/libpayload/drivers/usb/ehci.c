/*
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

//#define USB_DEBUG

#include <inttypes.h>
#include <libpayload.h>
#include <arch/barrier.h>
#include <arch/cache.h>
#include "ehci.h"
#include "ehci_private.h"

static void dump_td(u32 addr)
{
	qtd_t *td = phys_to_virt(addr);
	usb_debug("+---------------------------------------------------+\n");
	if (((td->token & (3UL << 8)) >> 8) == 2)
		usb_debug("|..[SETUP]..........................................|\n");
	else if (((td->token & (3UL << 8)) >> 8) == 1)
		usb_debug("|..[IN].............................................|\n");
	else if (((td->token & (3UL << 8)) >> 8) == 0)
		usb_debug("|..[OUT]............................................|\n");
	else
		usb_debug("|..[]...............................................|\n");
	usb_debug("|:|============ EHCI TD at [0x%08"PRIx32"] ==========|:|\n", addr);
	usb_debug("|:| ERRORS = [%"PRId32"] | TOKEN = [0x%08"PRIx32"] |         |:|\n",
		3 - ((td->token & QTD_CERR_MASK) >> QTD_CERR_SHIFT), td->token);
	usb_debug("|:+-----------------------------------------------+:|\n");
	usb_debug("|:| Next qTD        [0x%08"PRIx32"]                  |:|\n", td->next_qtd);
	usb_debug("|:+-----------------------------------------------+:|\n");
	usb_debug("|:| Alt. Next qTD   [0x%08"PRIx32"]                  |:|\n", td->alt_next_qtd);
	usb_debug("|:+-----------------------------------------------+:|\n");
	usb_debug("|:|       | Bytes to Transfer            |[%05"PRId32"] |:|\n", (td->token & QTD_TOTAL_LEN_MASK) >> 16);
	usb_debug("|:|       | PID CODE:                    |    [%ld] |:|\n", (td->token & (3UL << 8)) >> 8);
	usb_debug("|:|       | Interrupt On Complete (IOC)  |    [%ld] |:|\n", (td->token & (1UL << 15)) >> 15);
	usb_debug("|:|       | Status Active                |    [%ld] |:|\n", (td->token & (1UL << 7)) >> 7);
	usb_debug("|:|       | Status Halted                |    [%ld] |:|\n", (td->token & (1UL << 6)) >> 6);
	usb_debug("|:| TOKEN | Status Data Buffer Error     |    [%ld] |:|\n", (td->token & (1UL << 5)) >> 5);
	usb_debug("|:|       | Status Babble detected       |    [%ld] |:|\n", (td->token & (1UL << 4)) >> 4);
	usb_debug("|:|       | Status Transaction Error     |    [%ld] |:|\n", (td->token & (1UL << 3)) >> 3);
	usb_debug("|:|       | Status Missed Micro Frame    |    [%ld] |:|\n", (td->token & (1UL << 2)) >> 2);
	usb_debug("|:|       | Split Transaction State      |    [%ld] |:|\n", (td->token & (1UL << 1)) >> 1);
	usb_debug("|:|       | Ping State                   |    [%ld] |:|\n", td->token & 1UL);
	usb_debug("|:|-----------------------------------------------|:|\n");
	usb_debug("|...................................................|\n");
	usb_debug("+---------------------------------------------------+\n");
}

#if 0 && defined(USB_DEBUG)
static void dump_qh(ehci_qh_t *cur)
{
	qtd_t *tmp_qtd = NULL;
	usb_debug("+===================================================+\n");
	usb_debug("| ############# EHCI QH at [0x%08lx] ########### |\n", virt_to_phys(cur));
	usb_debug("+---------------------------------------------------+\n");
	usb_debug("| Horizontal Link Pointer        [0x%08lx]       |\n", cur->horiz_link_ptr);
	usb_debug("+------------------[ 0x%08lx ]-------------------+\n", cur->epchar);
	usb_debug("|        | Maximum Packet Length           | [%04ld] |\n", ((cur->epchar & (0x7ffUL << 16)) >> 16));
	usb_debug("|        | Device Address                  |    [%ld] |\n", cur->epchar & 0x7F);
	usb_debug("|        | Inactivate on Next Transaction  |    [%ld] |\n", ((cur->epchar & (1UL << 7)) >> 7));
	usb_debug("|        | Endpoint Number                 |    [%ld] |\n", ((cur->epchar & (0xFUL << 8)) >> 8));
	usb_debug("| EPCHAR | Endpoint Speed                  |    [%ld] |\n", ((cur->epchar & (3UL << 12)) >> 12));
	usb_debug("|        | Data Toggle Control             |    [%ld] |\n", ((cur->epchar & (1UL << 14)) >> 14));
	usb_debug("|        | Head of Reclamation List Flag   |    [%ld] |\n", ((cur->epchar & (1UL << 15)) >> 15));
	usb_debug("|        | Control Endpoint Flag           |    [%ld] |\n", ((cur->epchar & (1UL << 27)) >> 27));
	usb_debug("|        | Nak Count Reload                |    [%ld] |\n", ((cur->epchar & (0xFUL << 28)) >> 28));
	if (((cur->epchar & (1UL << QH_NON_HS_CTRL_EP_SHIFT)) >> QH_NON_HS_CTRL_EP_SHIFT) == 1) { /* Split transaction */
		usb_debug("+--------+---------[ 0x%08lx ]----------+--------+\n", cur->epcaps);
		usb_debug("|        | Hub Port                        |    [%ld] |\n", ((cur->epcaps & (0x7FUL << 23)) >> 23)); /* [29:23] */
		usb_debug("|        | Hub Address                     |    [%ld] |\n", ((cur->epcaps & (0x7FUL << 16)) >> 16)); /* [22:16] */
	}
	usb_debug("+---------------------------------------------------+\n");
	usb_debug("| Current QTD                   [0x%08lx]        |\n", cur->current_td_ptr);

	if (!((cur->horiz_link_ptr == 0) && (cur->epchar == 0))) {
		/* Dump overlay QTD for this QH */
		usb_debug("+---------------------------------------------------+\n");
		usb_debug("|::::::::::::::::::: QTD OVERLAY :::::::::::::::::::|\n");
		dump_td(virt_to_phys((void *)&(cur->td)));
		/* Dump all TD tree for this QH */
		tmp_qtd = (qtd_t *)phys_to_virt((cur->td.next_qtd & ~0x1FUL));
		if (tmp_qtd != NULL)
			usb_debug("|:::::::::::::::::: EHCI QTD CHAIN :::::::::::::::::|\n");
		while (tmp_qtd != NULL)
		{
			dump_td(virt_to_phys(tmp_qtd));
			tmp_qtd = (qtd_t *)phys_to_virt((tmp_qtd->next_qtd & ~0x1FUL));
		}
		usb_debug("|:::::::::::::::: EOF EHCI QTD CHAIN :::::::::::::::|\n");
		usb_debug("+---------------------------------------------------+\n");
	} else {
		usb_debug("+---------------------------------------------------+\n");
	}
}
#endif

static void ehci_start(hci_t *controller)
{
	EHCI_INST(controller)->operation->usbcmd |= HC_OP_RS;
}

static void ehci_stop(hci_t *controller)
{
	EHCI_INST(controller)->operation->usbcmd &= ~HC_OP_RS;
}

static void ehci_reset(hci_t *controller)
{
	short count = 0;
	ehci_stop(controller);
	/* wait 10 ms just to be sure */
	mdelay(10);
	if (EHCI_INST(controller)->operation->usbsts & HC_OP_HC_HALTED) {
		EHCI_INST(controller)->operation->usbcmd = HC_OP_HC_RESET;
		/* wait 100 ms */
		for (count = 0; count < 10; count++) {
			mdelay(10);
			if (!(EHCI_INST(controller)->operation->usbcmd & HC_OP_HC_RESET)) {
				return;
			}
		}
	}
	usb_debug("ehci_reset(): reset failed!\n");
}

static void ehci_reinit(hci_t *controller)
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
	int timeout = 100000; /* time out after 100ms */
	while (((ehcic->operation->usbsts & HC_OP_PERIODIC_SCHED_STAT) != enable)
			&& timeout--)
		udelay(1);
	if (timeout < 0) {
		usb_debug("ehci periodic schedule status change timed out.\n");
		return 1;
	}
	return 0;
}

static void ehci_shutdown(hci_t *controller)
{
	detach_controller(controller);

	/* Make sure periodic schedule is disabled */
	ehci_set_periodic_schedule(EHCI_INST(controller), 0);

	/* Give all ports back to companion controller */
	EHCI_INST(controller)->operation->configflag = 0;

	/* Free all dynamic allocations */
	free(EHCI_INST(controller)->dma_buffer);
	free(phys_to_virt(EHCI_INST(controller)->operation->periodiclistbase));
	free((void *)EHCI_INST(controller)->dummy_qh);
	free(EHCI_INST(controller));
	free(controller);
}

enum { EHCI_OUT = 0, EHCI_IN = 1, EHCI_SETUP = 2 };

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
			/* we have a contiguous mapping between virtual and physical memory */
			page += 4096;

			td->bufptrs[page_no++] = page;
			if (datalen <= 4096) {
				total_len += datalen;
				break;
			}
			datalen -= 4096;
			total_len += 4096;

			/* end TD at a packet boundary if transfer not complete */
			if (page_no == 5)
				total_len &= ~511;
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
		free((void *)cur);
		cur = next;
	}
	free((void *)qh);
}

#define EHCI_SLEEP_TIME_US	50

static int wait_for_tds(qtd_t *head)
{
	/* returns the amount of bytes *not* transmitted, or -1 for error */
	int result = 0;
	qtd_t *cur = head;
	while (1) {
		if (0) dump_td(virt_to_phys(cur));

		/* wait for results */
		int timeout = USB_MAX_PROCESSING_TIME_US / EHCI_SLEEP_TIME_US;
		while ((cur->token & QTD_ACTIVE) && !(cur->token & QTD_HALTED)
				&& timeout--)
			udelay(EHCI_SLEEP_TIME_US);
		if (timeout < 0) {
			usb_debug("Error: ehci: queue transfer "
				"processing timed out.\n");
			return -1;
		}
		if (cur->token & QTD_HALTED) {
			usb_debug("ERROR with packet\n");
			dump_td(virt_to_phys(cur));
			usb_debug("-----------------\n");
			return -1;
		}
		result += (cur->token & QTD_TOTAL_LEN_MASK)
				>> QTD_TOTAL_LEN_SHIFT;
		if (cur->next_qtd & 1) {
			break;
		}
		if (0)
			dump_td(virt_to_phys(cur));
		/* helps debugging the TD chain */
		if (0)
			usb_debug("\nmoving from %p to %p\n", cur, phys_to_virt(cur->next_qtd));
		cur = phys_to_virt(cur->next_qtd);
	}
	return result;
}

static int ehci_set_async_schedule(ehci_t *ehcic, int enable)
{

	/* Memory barrier to ensure that all memory accesses before we set the
	 * async schedule are complete. It was observed especially in the case of
	 * arm64, that netboot and USB stuff resulted in lots of errors possibly
	 * due to CPU reordering. Hence, enforcing strict CPU ordering.
	 */
	mb();

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
		usb_debug("ehci async schedule status change timed out.\n");
		return 1;
	}
	return 0;
}

static int ehci_process_async_schedule(
		ehci_t *ehcic, ehci_qh_t *qhead, qtd_t *head)
{
	int result;

	/* make sure async schedule is disabled */
	if (ehci_set_async_schedule(ehcic, 0)) return -1;

	/* hook up QH */
	ehcic->operation->asynclistaddr = virt_to_phys(qhead);

	/* start async schedule */
	if (ehci_set_async_schedule(ehcic, 1)) return -1;

	/* wait for result */
	result = wait_for_tds(head);

	/* disable async schedule */
	ehci_set_async_schedule(ehcic, 0);

	return result;
}

static int ehci_bulk(endpoint_t *ep, int size, u8 *src, int finalize)
{
	int result = 0;
	u8 *end = src + size;
	int remaining = size;
	int endp = ep->endpoint & 0xf;
	int pid = (ep->direction == IN)?EHCI_IN:EHCI_OUT;

	int hubaddr = 0, hubport = 0;
	if (ep->dev->speed < 2) {
		/* we need a split transaction */
		if (closest_usb2_hub(ep->dev, &hubaddr, &hubport))
			return -1;
	}

	if (!dma_coherent(src)) {
		end = EHCI_INST(ep->dev->controller)->dma_buffer + size;
		if (size > DMA_SIZE) {
			usb_debug("EHCI bulk transfer too large for DMA buffer: %d\n", size);
			return -1;
		}
		if (pid == EHCI_OUT)
			memcpy(end - size, src, size);
	}

	ehci_qh_t *qh = dma_memalign(64, sizeof(ehci_qh_t));
	qtd_t *head = dma_memalign(64, sizeof(qtd_t));
	qtd_t *cur = head;
	if (!qh || !head)
		goto oom;
	while (1) {
		memset((void *)cur, 0, sizeof(qtd_t));
		cur->token = QTD_ACTIVE |
			(pid << QTD_PID_SHIFT) |
			(0 << QTD_CERR_SHIFT);
		remaining -= fill_td(cur, end - remaining, remaining);

		cur->alt_next_qtd = QTD_TERMINATE;
		if (remaining <= 0) {
			cur->next_qtd = virt_to_phys(0) | QTD_TERMINATE;
			break;
		} else {
			qtd_t *next = dma_memalign(64, sizeof(qtd_t));
			if (!next)
				goto oom;
			cur->next_qtd = virt_to_phys(next);
			cur = next;
		}
	}

	/* create QH */
	memset((void *)qh, 0, sizeof(ehci_qh_t));
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
	if (result >= 0) {
		result = size - result;
		if (pid == EHCI_IN && end != src + size)
			memcpy(src, end - size, result);
	}

	ep->toggle = (cur->token & QTD_TOGGLE_MASK) >> QTD_TOGGLE_SHIFT;

	free_qh_and_tds(qh, head);

	return result;

oom:
	usb_debug("Not enough DMA memory for EHCI control structures!\n");
	free_qh_and_tds(qh, head);
	return -1;
}

/* FIXME: Handle control transfers as 3 QHs, so the 2nd stage can be >0x4000 bytes */
static int ehci_control(usbdev_t *dev, direction_t dir, int drlen, void *setup,
			 int dalen, u8 *src)
{
	u8 *data = src;
	u8 *devreq = setup;
	int endp = 0; // this is control. always 0 (for now)
	int toggle = 0;
	int mlen = dev->endpoints[0].maxpacketsize;
	int result = 0;

	int hubaddr = 0, hubport = 0, non_hs_ctrl_ep = 0;
	if (dev->speed < 2) {
		/* we need a split transaction */
		if (closest_usb2_hub(dev, &hubaddr, &hubport))
			return -1;
		non_hs_ctrl_ep = 1;
	}

	if (!dma_coherent(setup)) {
		devreq = EHCI_INST(dev->controller)->dma_buffer;
		memcpy(devreq, setup, drlen);
	}
	if (dalen > 0 && !dma_coherent(src)) {
		data = EHCI_INST(dev->controller)->dma_buffer + drlen;
		if (drlen + dalen > DMA_SIZE) {
			usb_debug("EHCI control transfer too large for DMA buffer: %d\n", drlen + dalen);
			return -1;
		}
		if (dir == OUT)
			memcpy(data, src, dalen);
	}

	/* create qTDs */
	qtd_t *head = dma_memalign(64, sizeof(qtd_t));
	ehci_qh_t *qh = dma_memalign(64, sizeof(ehci_qh_t));
	qtd_t *cur = head;
	if (!qh || !head)
		goto oom;
	memset((void *)cur, 0, sizeof(qtd_t));
	cur->token = QTD_ACTIVE |
		(toggle?QTD_TOGGLE_DATA1:0) |
		(EHCI_SETUP << QTD_PID_SHIFT) |
		(3 << QTD_CERR_SHIFT);
	if (fill_td(cur, devreq, drlen) != drlen) {
		usb_debug("ERROR: couldn't send the entire device request\n");
	}
	qtd_t *next = dma_memalign(64, sizeof(qtd_t));
	cur->next_qtd = virt_to_phys(next);
	cur->alt_next_qtd = QTD_TERMINATE;
	if (!next)
		goto oom;

	/* FIXME: We're limited to 16-20K (depending on alignment) for payload for now.
	 * Figure out, how toggle can be set sensibly in this scenario */
	if (dalen > 0) {
		toggle ^= 1;
		cur = next;
		memset((void *)cur, 0, sizeof(qtd_t));
		cur->token = QTD_ACTIVE |
			(toggle?QTD_TOGGLE_DATA1:0) |
			(((dir == OUT)?EHCI_OUT:EHCI_IN) << QTD_PID_SHIFT) |
			(3 << QTD_CERR_SHIFT);
		if (fill_td(cur, data, dalen) != dalen) {
			usb_debug("ERROR: couldn't send the entire control payload\n");
		}
		next = dma_memalign(64, sizeof(qtd_t));
		if (!next)
			goto oom;
		cur->next_qtd = virt_to_phys(next);
		cur->alt_next_qtd = QTD_TERMINATE;
	}

	toggle = 1;
	cur = next;
	memset((void *)cur, 0, sizeof(qtd_t));
	cur->token = QTD_ACTIVE |
		(toggle?QTD_TOGGLE_DATA1:QTD_TOGGLE_DATA0) |
		((dir == OUT)?EHCI_IN:EHCI_OUT) << QTD_PID_SHIFT |
		(0 << QTD_CERR_SHIFT);
	fill_td(cur, NULL, 0);
	cur->next_qtd = virt_to_phys(0) | QTD_TERMINATE;
	cur->alt_next_qtd = QTD_TERMINATE;

	/* create QH */
	memset((void *)qh, 0, sizeof(ehci_qh_t));
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
	if (result >= 0) {
		result = dalen - result;
		if (dir == IN && data != src)
			memcpy(src, data, result);
	}

	free_qh_and_tds(qh, head);
	return result;

oom:
	usb_debug("Not enough DMA memory for EHCI control structures!\n");
	free_qh_and_tds(qh, head);
	return -1;
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
	intr_qtd_t		*spare;
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

	intr_queue_t *const intrq = (intr_queue_t *)dma_memalign(64,
		sizeof(intr_queue_t));
	/*
	 * reqcount data chunks
	 * plus one more spare, which we'll leave out of queue
	 */
	u8 *data = (u8 *)dma_malloc(reqsize * (reqcount + 1));
	if (!intrq || !data)
		fatal("Not enough memory to create USB interrupt queue.\n");
	intrq->data = data;
	intrq->endp = ep;
	intrq->reqsize = reqsize;

	/* create #reqcount transfer descriptors (qTDs) */
	intrq->head = (intr_qtd_t *)dma_memalign(64, sizeof(intr_qtd_t));
	if (!intrq->head)
		fatal("Not enough DMA memory to create #reqcount TD.\n");
	intr_qtd_t *cur_td = intrq->head;
	for (i = 0; i < reqcount; ++i) {
		fill_intr_queue_td(intrq, cur_td, data);
		data += reqsize;
		if (i < reqcount - 1) {
			/* create one more qTD */
			intr_qtd_t *const next_td =
				(intr_qtd_t *)dma_memalign(64, sizeof(intr_qtd_t));
			if (!next_td)
				fatal("Not enough DMA memory to create TD.\n");
			cur_td->td.next_qtd = virt_to_phys(&next_td->td);
			cur_td->next = next_td;
			cur_td = next_td;
		}
	}
	intrq->tail = cur_td;

	/* create spare qTD */
	intrq->spare = (intr_qtd_t *)dma_memalign(64, sizeof(intr_qtd_t));
	if (!intrq->spare)
		fatal("Not enough DMA memory to create spare qTD.\n");
	intrq->spare->data = data;

	/* initialize QH */
	const int endp = ep->endpoint & 0xf;
	memset((void *)&intrq->qh, 0, sizeof(intrq->qh));
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
	const u32 dummy_ptr = virt_to_phys(EHCI_INST(
				ep->dev->controller)->dummy_qh) | PS_TYPE_QH;
	for (i = 0; i < 1024; i += reqtiming) {
		/* advance to the next free position */
		while ((i < 1024) && (ps[i] != dummy_ptr)) ++i;
		if (i < 1024) {
			ps[i] =	virt_to_phys(&intrq->qh) | PS_TYPE_QH;
			nothing_placed = 0;
		}
	}
	if (nothing_placed) {
		usb_debug("Error: Failed to place ehci interrupt queue head "
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
	const u32 dummy_ptr = virt_to_phys(EHCI_INST(
				ep->dev->controller)->dummy_qh) | PS_TYPE_QH;
	for (i = 0; i < 1024; ++i) {
		if ((ps[i] & PS_PTR_MASK) == virt_to_phys(&intrq->qh))
			ps[i] = dummy_ptr;
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
	free(intrq->spare);
	free(intrq->data);
	free(intrq);
}

static u8 *ehci_poll_intr_queue(void *const queue)
{
	intr_queue_t *const intrq = (intr_queue_t *)queue;

	u8 *ret = NULL;

	/* process if head qTD is inactive AND QH has been moved forward */
	if (!(intrq->head->td.token & QTD_ACTIVE)) {
		if (!(intrq->head->td.token & QTD_STATUS_MASK))
			ret = intrq->head->data;
		else
			usb_debug("ehci_poll_intr_queue: transfer failed, "
				"status == 0x%02x\n",
				intrq->head->td.token & QTD_STATUS_MASK);

		/* insert spare qTD at the end and advance our tail ptr */
		fill_intr_queue_td(intrq, intrq->spare, intrq->spare->data);
		intrq->tail->td.next_qtd = virt_to_phys(&intrq->spare->td);
		intrq->tail->next = intrq->spare;
		intrq->tail = intrq->tail->next;

		/* reuse executed qTD as spare one and advance our head ptr */
		intrq->spare = intrq->head;
		intrq->head = intrq->head->next;
	}
	/* reset queue if we fully processed it after underrun */
	else if ((intrq->qh.td.next_qtd & QTD_TERMINATE) &&
			/* to prevent race conditions:
			   not our head and not active */
			(intrq->qh.current_td_ptr !=
			 virt_to_phys(&intrq->head->td)) &&
			!(intrq->qh.td.token & QTD_ACTIVE)) {
		usb_debug("resetting underrun ehci interrupt queue.\n");
		intrq->qh.current_td_ptr = 0;
		memset((void *)&intrq->qh.td, 0, sizeof(intrq->qh.td));
		intrq->qh.td.next_qtd = virt_to_phys(&intrq->head->td);
	}
	return ret;
}

hci_t *
ehci_init(unsigned long physical_bar)
{
	int i;
	hci_t *controller = new_controller();
	controller->instance = xzalloc(sizeof(ehci_t));
	controller->reg_base = (uintptr_t)physical_bar;
	controller->type = EHCI;
	controller->start = ehci_start;
	controller->stop = ehci_stop;
	controller->reset = ehci_reset;
	controller->init = ehci_reinit;
	controller->shutdown = ehci_shutdown;
	controller->bulk = ehci_bulk;
	controller->control = ehci_control;
	controller->set_address = generic_set_address;
	controller->finish_device_config = NULL;
	controller->destroy_device = NULL;
	controller->create_intr_queue = ehci_create_intr_queue;
	controller->destroy_intr_queue = ehci_destroy_intr_queue;
	controller->poll_intr_queue = ehci_poll_intr_queue;
	init_device_entry(controller, 0);

	EHCI_INST(controller)->capabilities = phys_to_virt(physical_bar);
	EHCI_INST(controller)->operation = (hc_op_t *)(phys_to_virt(physical_bar) + EHCI_INST(controller)->capabilities->caplength);

	/* Set the high address word (aka segment) if controller is 64-bit */
	if (EHCI_INST(controller)->capabilities->hccparams & 1)
		EHCI_INST(controller)->operation->ctrldssegment = 0;

	/* Enable operation of controller */
	controller->start(controller);

	/* take over all ports. USB1 should be blind now */
	EHCI_INST(controller)->operation->configflag = 1;

	/* Initialize periodic frame list */
	/* 1024 32-bit pointers, 4kb aligned */
	u32 *const periodic_list = (u32 *)dma_memalign(4096, 1024 * sizeof(u32));
	if (!periodic_list)
		fatal("Not enough memory creating EHCI periodic frame list.\n");

	if (dma_initialized()) {
		EHCI_INST(controller)->dma_buffer = dma_memalign(4096, DMA_SIZE);
		if (!EHCI_INST(controller)->dma_buffer)
			fatal("Not enough DMA memory for EHCI bounce buffer.\n");
	}

	/*
	 * Insert dummy QH in periodic frame list
	 * This helps with broken host controllers
	 * and doesn't violate the standard.
	 */
	EHCI_INST(controller)->dummy_qh = (ehci_qh_t *)dma_memalign(64, sizeof(ehci_qh_t));
	if (!EHCI_INST(controller)->dummy_qh)
		fatal("Not enough DMA memory for EHCI dummy TD.\n");
	memset((void *)EHCI_INST(controller)->dummy_qh, 0,
		sizeof(*EHCI_INST(controller)->dummy_qh));
	EHCI_INST(controller)->dummy_qh->horiz_link_ptr = QH_TERMINATE;
	EHCI_INST(controller)->dummy_qh->td.next_qtd = QH_TERMINATE;
	EHCI_INST(controller)->dummy_qh->td.alt_next_qtd = QH_TERMINATE;
	for (i = 0; i < 1024; ++i)
		periodic_list[i] =
			virt_to_phys(EHCI_INST(controller)->dummy_qh)
				| PS_TYPE_QH;

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
	controller->devices[0]->init(controller->devices[0]);

	return controller;
}

#if CONFIG(LP_USB_PCI)
hci_t *
ehci_pci_init(pcidev_t addr)
{
	hci_t *controller;
	u32 reg_base;

	u16 pci_command = pci_read_config16(addr, PCI_COMMAND);
	pci_command = (pci_command | PCI_COMMAND_MEMORY) & ~PCI_COMMAND_IO;
	pci_write_config16(addr, PCI_COMMAND, pci_command);

	reg_base = pci_read_config32(addr, USBBASE);

	/* default value for frame length adjust */
	pci_write_config8(addr, FLADJ, FLADJ_framelength(60000));

	controller = ehci_init((unsigned long)reg_base);

	if (controller)
		controller->pcidev = addr;

	return controller;
}
#endif
