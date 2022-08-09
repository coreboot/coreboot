/*
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
#include <udc/chipidea.h>
#include "chipidea_priv.h"

#ifdef DEBUG
#define debug(x...) printf(x)
#else
#define debug(x...) do {} while (0)
#endif

#define min(a, b) (((a) < (b)) ? (a) : (b))

static struct qh *get_qh(struct chipidea_pdata *p, int endpoint, int in_dir)
{
	assert(in_dir <= 1);
	return &p->qhlist[2 * endpoint + in_dir];
}

static unsigned int ep_to_bits(int ep, int in_dir)
{
	return ep + (in_dir ? 16 : 0);
}

static void clear_setup_ep(struct chipidea_pdata *p, int endpoint)
{
	writel(1 << endpoint, &p->opreg->epsetupstat);
}

static void clear_ep(struct chipidea_pdata *p, int endpoint, int in_dir)
{
	writel(1 << ep_to_bits(endpoint, in_dir), &p->opreg->epcomplete);
}

static int chipidea_hw_init(struct usbdev_ctrl *this, void *_opreg,
	const device_descriptor_t *dd)
{
	struct chipidea_opreg *opreg = _opreg;
	struct chipidea_pdata *p = CI_PDATA(this);

	p->opreg = phys_to_virt(opreg);
	p->qhlist = dma_memalign(4096, sizeof(struct qh) * CI_QHELEMENTS);
	memcpy(&this->device_descriptor, dd, sizeof(*dd));

	if (p->qhlist == NULL)
		die("failed to allocate memory for USB device mode");

	memset(p->qhlist, 0, sizeof(struct qh) * CI_QHELEMENTS);

	SLIST_INIT(&this->configs);

	int i;
	for (i = 0; i < 16; i++) {
		SIMPLEQ_INIT(&p->job_queue[i][0]);
		SIMPLEQ_INIT(&p->job_queue[i][1]);
	}

	for (i = 0; i < CI_QHELEMENTS; i++) {
		p->qhlist[i].config = QH_MPS(512) | QH_NO_AUTO_ZLT | QH_IOS;
		p->qhlist[i].td.next = TD_TERMINATE;
	}
	/* EP0 in/out are hardwired for SETUP */
	p->qhlist[0].config = QH_MPS(64) | QH_NO_AUTO_ZLT | QH_IOS;
	p->qhlist[1].config = QH_MPS(64) | QH_NO_AUTO_ZLT | QH_IOS;

	do {
		debug("waiting for USB phy clk valid: %x\n",
			readl(&p->opreg->susp_ctrl));
		mdelay(1);
	} while ((readl(&p->opreg->susp_ctrl) & (1 << 7)) == 0);

	writel(USBCMD_8MICRO | USBCMD_RST, &p->opreg->usbcmd);
	mdelay(1);

	/* enable device mode */
	writel(2, &p->opreg->usbmode);

	dcache_clean_by_mva(p->qhlist, sizeof(struct qh) * CI_QHELEMENTS);

	writel(virt_to_phys(p->qhlist), &p->opreg->epbase);
	writel(0xffffffff, &p->opreg->epflush);

	/* enable EP0 */
	writel((1 << 23) | (1 << 22) | (1 << 7) | (1 << 6),
		&p->opreg->epctrl[0]);

	/* clear status register */
	writel(readl(&p->opreg->usbsts), &p->opreg->usbsts);

	debug("taking controller out of reset\n");
	writel(USBCMD_8MICRO | USBCMD_RUN, &p->opreg->usbcmd);

	this->stall(this, 0, 0, 0);
	this->stall(this, 0, 1, 0);

	return 1;
}

static void chipidea_halt_ep(struct usbdev_ctrl *this, int ep, int in_dir)
{
	struct chipidea_pdata *p = CI_PDATA(this);
	writel(1 << ep_to_bits(ep, in_dir), &p->opreg->epflush);
	while (readl(&p->opreg->epflush))
		;
	clrbits32(&p->opreg->epctrl[ep], 1 << (7 + (in_dir ? 16 : 0)));

	while (!SIMPLEQ_EMPTY(&p->job_queue[ep][in_dir])) {
		struct job *job = SIMPLEQ_FIRST(&p->job_queue[ep][in_dir]);
		if (job->autofree)
			free(job->data);

		SIMPLEQ_REMOVE_HEAD(&p->job_queue[ep][in_dir], queue);
	}
}

static void chipidea_start_ep(struct usbdev_ctrl *this,
	int ep, int in_dir, int ep_type, int mps)
{
	struct chipidea_pdata *p = CI_PDATA(this);
	struct qh *qh = get_qh(p, ep, in_dir);
	qh->config = (mps << 16) | QH_NO_AUTO_ZLT | QH_IOS;
	dcache_clean_by_mva(qh, sizeof(*qh));
	in_dir = in_dir ? 1 : 0;
	debug("enabling %d-%d (type %d)\n", ep, in_dir, ep_type);
	/* enable endpoint, reset data toggle */
	setbits32(&p->opreg->epctrl[ep],
		((1 << 7) | (1 << 6) | (ep_type << 2)) << (in_dir*16));
	p->ep_busy[ep][in_dir] = 0;
	this->ep_mps[ep][in_dir] = mps;
}

static void advance_endpoint(struct chipidea_pdata *p, int endpoint, int in_dir)
{
	if (p->ep_busy[endpoint][in_dir])
		return;
	if (SIMPLEQ_EMPTY(&p->job_queue[endpoint][in_dir]))
		return;

	struct job *job = SIMPLEQ_FIRST(&p->job_queue[endpoint][in_dir]);
	struct qh *qh = get_qh(p, endpoint, in_dir);

	uint32_t start = (uint32_t)(uintptr_t)job->data;
	uint32_t offset = (start & 0xfff);
	/* unlike with typical EHCI controllers,
	 * a full TD transfers either 0x5000 bytes if
	 * page aligned or 0x4000 bytes if not.
	 */
	int maxsize = 0x5000;
	if (offset > 0)
		maxsize = 0x4000;
	uint32_t td_count = (job->length + maxsize - 1) / maxsize;

	/* special case for zero length packets */
	if (td_count == 0)
		td_count = 1;

	if (job->zlp)
		td_count++;

	struct td *tds = dma_memalign(32, sizeof(struct td) * td_count);
	memset(tds, 0, sizeof(struct td) * td_count);

	int i;
	int remaining = job->length;
	for (i = 0; i < td_count; i++) {
		int datacount = min(maxsize, remaining);

		debug("td %d, %d bytes\n", i, datacount);
		tds[i].next = (uint32_t)virt_to_phys(&tds[i+1]);
		tds[i].info = TD_INFO_LEN(datacount) | TD_INFO_ACTIVE;
		tds[i].page0 = start;
		tds[i].page1 = (start & 0xfffff000) + 0x1000;
		tds[i].page2 = (start & 0xfffff000) + 0x2000;
		tds[i].page3 = (start & 0xfffff000) + 0x3000;
		tds[i].page4 = (start & 0xfffff000) + 0x4000;
		remaining -= datacount;
		start = start + datacount;
	}
	tds[td_count - 1].next = TD_TERMINATE;
	tds[td_count - 1].info |= TD_INFO_IOC;

	qh->td.next = (uint32_t)virt_to_phys(tds);
	qh->td.info = 0;

	job->tds = tds;
	job->td_count = td_count;
	dcache_clean_by_mva(tds, sizeof(struct td) * td_count);
	dcache_clean_by_mva(qh, sizeof(*qh));
	if (!dma_coherent(job->data))
		dcache_clean_by_mva(job->data, job->length);

	debug("priming EP %d-%d with %zx bytes starting at %x (%p)\n", endpoint,
		in_dir, job->length, tds[0].page0, job->data);
	writel(1 << ep_to_bits(endpoint, in_dir), &p->opreg->epprime);
	while (readl(&p->opreg->epprime))
		;
	p->ep_busy[endpoint][in_dir] = 1;
}

static void handle_endpoint(struct usbdev_ctrl *this, int endpoint, int in_dir)
{
	struct chipidea_pdata *p = CI_PDATA(this);
	struct job *job = SIMPLEQ_FIRST(&p->job_queue[endpoint][in_dir]);
	SIMPLEQ_REMOVE_HEAD(&p->job_queue[endpoint][in_dir], queue);

	if (in_dir && !dma_coherent(job->data))
		dcache_invalidate_by_mva(job->data, job->length);

	int length = job->length;

	int i = 0;
	do {
		int active;
		do {
			dcache_invalidate_by_mva(&job->tds[i],
				sizeof(struct td));
			active = job->tds[i].info & TD_INFO_ACTIVE;
			debug("%d-%d: info %08x, page0 %x, next %x\n",
				endpoint, in_dir, job->tds[i].info,
				job->tds[i].page0, job->tds[i].next);
		} while (active);
		/*
		 * The controller writes back the length field in info
		 * with the number of bytes it did _not_ process.
		 * Hence, take the originally scheduled length and
		 * subtract whatever lengths we still find - that gives
		 * us the data that the controller did transfer.
		 */
		int remaining = job->tds[i].info >> 16;
		length -= remaining;
	} while (job->tds[i++].next != TD_TERMINATE);
	debug("%d-%d: scheduled %zd, now %d bytes\n", endpoint, in_dir,
		job->length, length);

	if (this->current_config &&
	    this->current_config->interfaces[0].handle_packet)
		this->current_config->interfaces[0].handle_packet(this,
			endpoint, in_dir, job->data, length);

	free(job->tds);
	if (job->autofree)
		free(job->data);
	free(job);
	p->ep_busy[endpoint][in_dir] = 0;

	advance_endpoint(p, endpoint, in_dir);
}

static void start_setup(struct usbdev_ctrl *this, int ep)
{
	dev_req_t dr;
	struct chipidea_pdata *p = CI_PDATA(this);
	struct qh *qh = get_qh(p, ep, 0);

	dcache_invalidate_by_mva(qh, sizeof(*qh));
	memcpy(&dr, qh->setup_data, sizeof(qh->setup_data));
	clear_setup_ep(p, ep);

#ifdef DEBUG
	hexdump((unsigned long)&dr, sizeof(dr));
#endif

	udc_handle_setup(this, ep, &dr);
}

static void chipidea_enqueue_packet(struct usbdev_ctrl *this, int endpoint,
	int in_dir, void *data, int len, int zlp, int autofree)
{
	struct chipidea_pdata *p = CI_PDATA(this);
	struct job *job = malloc(sizeof(*job));

	job->data = data;
	job->length = len;
	job->zlp = zlp;
	job->autofree = autofree;

	debug("adding job of %d bytes to EP %d-%d\n", len, endpoint, in_dir);
	SIMPLEQ_INSERT_TAIL(&p->job_queue[endpoint][in_dir], job, queue);

	if ((endpoint == 0) || (this->initialized))
		advance_endpoint(p, endpoint, in_dir);
}

static int chipidea_poll(struct usbdev_ctrl *this)
{
	struct chipidea_pdata *p = CI_PDATA(this);
	uint32_t sts = readl(&p->opreg->usbsts);
	writel(sts, &p->opreg->usbsts); /* clear */

	/* new information if the bus is high speed or not */
	if (sts & USBSTS_PCI) {
		debug("USB speed negotiation: ");
		if ((readl(&p->opreg->devlc) & DEVLC_HOSTSPEED_MASK)
		   == DEVLC_HOSTSPEED(2)) {
			debug("high speed\n");
			// TODO: implement
		} else {
			debug("full speed\n");
			// TODO: implement
		}
	}

	/* reset requested. stop all activities */
	if (sts & USBSTS_URI) {
		int i;
		debug("USB reset requested\n");
		if (this->initialized) {
			writel(readl(&p->opreg->epstat), &p->opreg->epstat);
			writel(readl(&p->opreg->epsetupstat),
				&p->opreg->epsetupstat);
			writel(0xffffffff, &p->opreg->epflush);
			for (i = 1; i < 16; i++)
				writel(0, &p->opreg->epctrl[i]);
			this->initialized = 0;
		}
		writel((1 << 22) | (1 << 6), &p->opreg->epctrl[0]);
		p->qhlist[0].config = QH_MPS(64) | QH_NO_AUTO_ZLT | QH_IOS;
		p->qhlist[1].config = QH_MPS(64) | QH_NO_AUTO_ZLT | QH_IOS;
		dcache_clean_by_mva(p->qhlist, 2 * sizeof(struct qh));
	}

	if (sts & (USBSTS_UEI | USBSTS_UI)) {
		uint32_t bitmap;
		int ep;

		/* This slightly deviates from the recommendation in the
		 * data sheets, but the strict ordering is to simplify
		 * handling control transfers, which are initialized in
		 * the third step with a SETUP packet, then proceed in
		 * the next poll loop with in transfers (either data or
		 * status phase), then optionally out transfers (status
		 * phase).
		 */

		/* in transfers */
		bitmap = (readl(&p->opreg->epcomplete) >> 16) & 0xffff;
		ep = 0;
		while (bitmap) {
			if (bitmap & 1) {
				debug("incoming packet on EP %d (in)\n", ep);
				handle_endpoint(this, ep, 1);
				clear_ep(p, ep & 0xf, 1);
			}
			bitmap >>= 1;
			ep++;
		}

		/* out transfers */
		bitmap = readl(&p->opreg->epcomplete) & 0xffff;
		ep = 0;
		while (bitmap) {
			if (bitmap & 1) {
				debug("incoming packet on EP %d (out)\n", ep);
				handle_endpoint(this, ep, 0);
				clear_ep(p, ep, 0);
			}
			bitmap >>= 1;
			ep++;
		}

		/* setup transfers */
		bitmap = readl(&p->opreg->epsetupstat);
		ep = 0;
		while (bitmap) {
			if (bitmap & 1) {
				debug("incoming packet on EP %d (setup)\n", ep);
				start_setup(this, ep);
			}
			bitmap >>= 1;
			ep++;
		}
	}

	return 1;
}

static void chipidea_force_shutdown(struct usbdev_ctrl *this)
{
	struct chipidea_pdata *p = CI_PDATA(this);
	writel(0xffffffff, &p->opreg->epflush);
	writel(USBCMD_8MICRO | USBCMD_RST, &p->opreg->usbcmd);
	writel(0, &p->opreg->usbmode);
	writel(USBCMD_8MICRO, &p->opreg->usbcmd);
	free(p->qhlist);
	free(p);
	free(this);
}

static void chipidea_shutdown(struct usbdev_ctrl *this)
{
	struct chipidea_pdata *p = CI_PDATA(this);
	int i, j;
	int is_empty = 0;
	while (!is_empty) {
		is_empty = 1;
		this->poll(this);
		for (i = 0; i < 16; i++)
			for (j = 0; j < 2; j++)
				if (!SIMPLEQ_EMPTY(&p->job_queue[i][j]))
					is_empty = 0;
	}
	chipidea_force_shutdown(this);
}

static void chipidea_set_address(struct usbdev_ctrl *this, int address)
{
	struct chipidea_pdata *p = CI_PDATA(this);
	writel((address << 25) | (1 << 24), &p->opreg->usbadr);
}

static void chipidea_stall(struct usbdev_ctrl *this,
	uint8_t ep, int in_dir, int set)
{
	struct chipidea_pdata *p = CI_PDATA(this);
	assert(ep < 16);
	uint32_t *ctrl = &p->opreg->epctrl[ep];
	in_dir = in_dir ? 1 : 0;
	if (set) {
		if (in_dir)
			setbits32(ctrl, 1 << 16);
		else
			setbits32(ctrl, 1 << 0);
	} else {
		/* reset STALL bit, reset data toggle */
		if (in_dir) {
			setbits32(ctrl, 1 << 22);
			clrbits32(ctrl, 1 << 16);
		} else {
			setbits32(ctrl, 1 << 6);
			clrbits32(ctrl, 1 << 0);
		}
	}
	this->ep_halted[ep][in_dir] = set;
}

static void *chipidea_malloc(size_t size)
{
	return dma_malloc(size);
}

static void chipidea_free(void *ptr)
{
	free(ptr);
}

struct usbdev_ctrl *chipidea_init(device_descriptor_t *dd)
{
	struct usbdev_ctrl *ctrl = calloc(1, sizeof(*ctrl));
	if (ctrl == NULL)
		return NULL;
	ctrl->pdata = calloc(1, sizeof(struct chipidea_pdata));
	if (ctrl->pdata == NULL) {
		free(ctrl);
		return NULL;
	}

	ctrl->poll = chipidea_poll;
	ctrl->add_gadget = udc_add_gadget;
	ctrl->add_strings = udc_add_strings;
	ctrl->enqueue_packet = chipidea_enqueue_packet;
	ctrl->force_shutdown = chipidea_force_shutdown;
	ctrl->shutdown = chipidea_shutdown;
	ctrl->set_address = chipidea_set_address;
	ctrl->stall = chipidea_stall;
	ctrl->halt_ep = chipidea_halt_ep;
	ctrl->start_ep = chipidea_start_ep;
	ctrl->alloc_data = chipidea_malloc;
	ctrl->free_data = chipidea_free;
	ctrl->initialized = 0;

	int i;
	ctrl->ep_mps[0][0] = 64;
	ctrl->ep_mps[0][1] = 64;
	for (i = 1; i < 16; i++) {
		ctrl->ep_mps[i][0] = 512;
		ctrl->ep_mps[i][1] = 512;
	}

	if (!chipidea_hw_init(ctrl, (void *)0x7d000000, dd)) {
		free(ctrl->pdata);
		free(ctrl);
		return NULL;
	}
	return ctrl;
}
