/*
 *
 * Copyright (C) 2015 Rockchip Electronics
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <libpayload.h>
#include <assert.h>
#include <limits.h>

#include <udc/dwc2_udc.h>
#include "dwc2_priv.h"

static int get_mps(dwc2_ep_t *ep)
{
	dwc2_ep_reg_t *ep_reg = ep->ep_regs;
	depctl_t depctl;
	uint16_t mps = 0;

	depctl.d32 = readl(&ep_reg->depctl);
	if (ep->ep_num == 0) {
		switch (depctl.mps) {
		case D0EPCTL_MPS_64:
			mps = 64;
			break;
		case D0EPCTL_MPS_32:
			mps = 32;
			break;
		case D0EPCTL_MPS_16:
			mps = 16;
			break;
		case D0EPCTL_MPS_8:
			mps = 8;
			break;
		default:
			usb_debug("get mps error\n");
		}
	} else {
		mps = depctl.mps;
	}

	return mps;
}

static void dwc2_process_ep(dwc2_ep_t *ep, int len, void *buf)
{
	depctl_t depctl;
	depsiz_t depsiz;
	uint16_t pkt_cnt;
	uint16_t mps;
	int max_transfer_size;
	dwc2_ep_reg_t *ep_reg = ep->ep_regs;

	if (ep->ep_num == 0)
		max_transfer_size = EP0_MAXLEN;
	else
		max_transfer_size = EP_MAXLEN;
	assert(len <= max_transfer_size);

	mps = get_mps(ep);

	pkt_cnt = ALIGN_UP(len, mps) / mps;
	if (pkt_cnt == 0)
		pkt_cnt = 1;

	depsiz.pktcnt = pkt_cnt;
	depsiz.xfersize = len;
	writel(depsiz.d32, &ep_reg->deptsiz);

	writel((uint32_t)buf, &ep_reg->depdma);

	depctl.d32 = readl(&ep_reg->depctl);

	if (ep->ep_num != 0) {
		if (depctl.dpid == 0)
			depctl.setd0pid = 1;
		else
			depctl.setd1pid = 1;
	}
	depctl.cnak = 1;
	depctl.epena = 1;
	writel(depctl.d32, &ep_reg->depctl);

}

static void dwc2_write_ep(dwc2_ep_t *ep, int len, void *buf)
{
	dwc2_process_ep(ep, len, buf);
}

static void dwc2_read_ep(dwc2_ep_t *ep, int len, void *buf)
{
	dwc2_process_ep(ep, len, buf);
}

static void dwc2_connect(struct usbdev_ctrl *this, int connect)
{
	/* Turn on the USB connection by enabling the pullup resistor */
	dwc2_pdata_t *p = DWC2_PDATA(this);
	dctl_t dctl;

	usb_debug("DwcUdcConnect\n");

	dctl.d32 = readl(&p->regs->device.dctl);

	if (connect)
		/* Connect */
		dctl.sftdiscon = 0;
	else
		/* Disconnect */
		dctl.sftdiscon = 1;

	writel(dctl.d32, &p->regs->device.dctl);
}

static void dwc2_bus_reset(struct usbdev_ctrl *this)
{
	dwc2_pdata_t *p = DWC2_PDATA(this);
	dcfg_t dcfg;
	dctl_t dctl;

	if (this->initialized)
		this->initialized = 0;

	/* Reset device addr */
	dcfg.d32 = readl(&p->regs->device.dcfg);
	dcfg.devaddr = 0;
	writel(dcfg.d32, &p->regs->device.dcfg);

	dctl.d32 = readl(&p->regs->device.dctl);
	dctl.rmtwkupsig = 0;
	writel(dctl.d32, &p->regs->device.dctl);
}

static void dwc2_enum_done(struct usbdev_ctrl *this)
{
	dwc2_pdata_t *p = DWC2_PDATA(this);

	dctl_t dctl;
	dsts_t dsts;

	usb_debug("dwc2_enum_done\n");

	dsts.d32 = readl(&p->regs->device.dsts);

	switch (dsts.enumspd) {
	case 0:
		this->ep_mps[0][0] = 64;
		this->ep_mps[0][1] = 64;
		usb_debug("HighSpeed Enum Done\n");
		break;
	default:
		usb_debug("EnumSpeed Error\n");
		return;
	}

	/* Clear global IN Nak */
	dctl.d32 = readl(&p->regs->device.dctl);
	dctl.cgnpinnak = 1;
	writel(dctl.d32, &p->regs->device.dctl);
}

static void dwc2_tx_fifo_flush(struct usbdev_ctrl *this, unsigned int idx)
{
	dwc2_pdata_t *p = DWC2_PDATA(this);
	grstctl_t grstctl;
	int timeout = 100;

	grstctl.d32 = readl(&p->regs->core.grstctl);
	grstctl.txfflsh = 1;
	grstctl.txfnum = idx;
	writel(grstctl.d32, &p->regs->core.grstctl);

	/* wait until the fifo is flushed */
	do {
		udelay(1);
		grstctl.d32 = readl(&p->regs->core.grstctl);

		if (--timeout < 0) {
			usb_debug("timeout flushing Tx fifo %x\n", idx);
			break;
		}
	} while (grstctl.txfflsh);
}

static void dwc2_rx_fifo_flush(struct usbdev_ctrl *this,  unsigned int idx)
{
	dwc2_pdata_t *p = DWC2_PDATA(this);
	grstctl_t grstctl;
	int timeout = 100;

	grstctl.d32 = readl(&p->regs->core.grstctl);
	grstctl.rxfflsh = 1;
	writel(grstctl.d32, &p->regs->core.grstctl);

	/* wait until the fifo is flushed */
	do {
		udelay(1);
		grstctl.d32 = readl(&p->regs->core.grstctl);

		if (--timeout < 0) {
			usb_debug("timeout flushing Rx fifo %x\n", idx);
			break;
		}
	} while (grstctl.rxfflsh);
}

static void dwc2_disable_ep(dwc2_ep_reg_t *ep_reg)
{
	depctl_t depctl;
	depint_t depint;

	/* Disable the required IN/OUT endpoint */
	depctl.d32 = readl(&ep_reg->depctl);

	/* Already disabled */
	if (depctl.epena == 0)
		return;
	depctl.epdis = 1;
	depctl.snak = 1;
	writel(depctl.d32, &ep_reg->depctl);

	/* Wait for the DEPINTn.EPDisabled interrupt */
	do {
		depint.d32 = readl(&ep_reg->depint);
	} while (!depint.epdisbld);

	/* Clear DEPINTn.EPDisabled */
	writel(depint.d32, &ep_reg->depint);

	depctl.d32 = readl(&ep_reg->depctl);
	depctl.epena = 0;
	depctl.epdis = 0;
	writel(depctl.d32, &ep_reg->depctl);
}

static void dwc2_halt_ep(struct usbdev_ctrl *this, int ep, int in_dir)
{
	dwc2_pdata_t *p = DWC2_PDATA(this);
	dwc2_ep_reg_t *ep_reg = p->eps[ep][in_dir].ep_regs;
	depctl_t depctl;
	dctl_t dctl;
	gintsts_t gintsts;

	usb_debug("dwc2_halt_ep ep %d-%d\n", ep, in_dir);
	depctl.d32 = readl(&ep_reg->depctl);
	/* Already disabled */
	if (!depctl.epena)
		return;
	/* First step: disable EP */
	if (in_dir) {
		/* Only support Non-Periodic IN Endpoints */
		dctl.d32 = readl(&p->regs->device.dctl);
		dctl.sgnpinnak = 1;
		writel(dctl.d32, &p->regs->device.dctl);

		/* Wait for the GINTSTS.Global IN NP NAK Effective interrupt */
		do {
			gintsts.d32 = readl(&p->regs->core.gintsts);
		} while (!gintsts.ginnakeff);

		/* Clear GINTSTS.Global IN NP NAK Effective interrupt */
		writel(gintsts.d32, &p->regs->core.gintsts);
		dwc2_disable_ep(ep_reg);

		/* Flush Tx Fifo */
		dwc2_tx_fifo_flush(this, p->eps[ep][in_dir].txfifo);

	} else {
		/* Enable Global OUT NAK mode */
		dctl.d32 = readl(&p->regs->device.dctl);
		dctl.sgoutnak = 1;
		writel(dctl.d32, &p->regs->device.dctl);

		/* Wait for the GINTSTS.GOUTNakEff interrupt */
		do {
			gintsts.d32 = readl(&p->regs->core.gintsts);
		} while (!gintsts.goutnakeff);

		/* Clear GINTSTS.GOUTNakEff */
		writel(gintsts.d32, &p->regs->core.gintsts);

		dwc2_disable_ep(ep_reg);

		dctl.d32 = readl(&p->regs->device.dctl);
		dctl.cgoutnak = 1;
		dctl.sgoutnak = 0;
		writel(dctl.d32, &p->regs->device.dctl);
	}

	/* Second step: clear job queue */
	while (!SIMPLEQ_EMPTY(&p->eps[ep][in_dir].job_queue)) {
		struct job *job = SIMPLEQ_FIRST(&p->eps[ep][in_dir].job_queue);

		if (job->autofree)
			free(job->data);

		SIMPLEQ_REMOVE_HEAD(&p->eps[ep][in_dir].job_queue, queue);
	}
}

static int find_tx_fifo(struct usbdev_ctrl *this, uint32_t mps)
{
	dwc2_pdata_t *p = DWC2_PDATA(this);
	uint32_t fifo_index = 0;
	uint32_t fifo_size = UINT_MAX;
	gtxfsiz_t gtxfsiz;
	int i, val;

	for (i = 1; i < MAX_EPS_CHANNELS - 1; i++) {
		if (p->fifo_map & (1 << i))
			continue;
		gtxfsiz.d32 = readl(&p->regs->core.dptxfsiz_dieptxf[i]);
		val = gtxfsiz.txfdep * 4;

		if (val < mps)
			continue;
		/* Search for smallest acceptable fifo */
		if (val < fifo_size) {
			fifo_size = val;
			fifo_index = i;
		}
	}

	if (!fifo_index)
		fatal("find_tx_fifo no suitable fifo found\n");

	p->fifo_map |= 1 << fifo_index;

	return fifo_index;

}

static void dwc2_start_ep0(struct usbdev_ctrl *this)
{
	dwc2_pdata_t *p = DWC2_PDATA(this);
	depctl_t depctl = { .d32 = 0 };
	depint_t depint = { .d32 = 0xff };

	usb_debug("dwc2_start_ep0\n");

	/* Enable endpoint, reset data toggle */
	depctl.mps = 0;
	depctl.usbactep = 1;
	depctl.snak = 1;
	depctl.epdis = 1;

	writel(depctl.d32, &p->regs->device.inep[0].depctl);
	writel(depint.d32, &p->regs->device.inep[0].depint);
	writel(depctl.d32, &p->regs->device.outep[0].depctl);
	writel(depint.d32, &p->regs->device.outep[0].depint);

	p->eps[0][0].busy = 0;
	p->eps[0][1].busy = 0;
	this->ep_mps[0][0] = 64;
	this->ep_mps[0][1] = 64;
}

static void dwc2_start_ep(struct usbdev_ctrl *this,
			int ep, int in_dir, int ep_type, int mps)
{
	dwc2_pdata_t *p = DWC2_PDATA(this);
	dwc2_ep_reg_t *ep_reg = p->eps[ep][in_dir].ep_regs;
	depctl_t depctl = { .d32 = 0 };

	assert((ep < 16) && (ep > 0));
	usb_debug("dwc2_start_ep %d-%d (type %d)\n", ep, in_dir, ep_type);

	in_dir = in_dir ? 1 : 0;

	/* Enable endpoint, reset data toggle */
	depctl.setd0pid = 1;
	depctl.mps = mps & 0x3ff;
	depctl.usbactep = 1;

	/* ep type 0:ctrl 1:isoc 2:bulk 3:intr */
	depctl.eptype = ep_type;
	depctl.snak = 1;
	if (in_dir) {
		/* Allocate Tx FIFO */
		p->eps[ep][in_dir].txfifo = find_tx_fifo(this, mps);
	}
	writel(depctl.d32, &ep_reg->depctl);

	p->eps[ep][in_dir].busy = 0;
	this->ep_mps[ep][in_dir] = mps;
}

static void continue_ep_transfer(dwc2_pdata_t *p,
				 int endpoint, int in_dir)
{
	int max_transfer_size = (endpoint == 0) ? EP0_MAXLEN : EP_MAXLEN;
	int mps;
	uint32_t remind_length;
	void *data_buf;

	if (SIMPLEQ_EMPTY(&p->eps[endpoint][in_dir].job_queue))
		return;

	struct job *job = SIMPLEQ_FIRST(&p->eps[endpoint][in_dir].job_queue);

	remind_length = job->length - job->xfered_length;

	job->xfer_length = (remind_length > max_transfer_size) ?
			    max_transfer_size : remind_length;
	data_buf = job->data + job->xfered_length;

	if ((((uint32_t)data_buf & 3) != 0) && (job->xfer_length > 0))
		usb_debug("Un-aligned buffer address\n");

	if (in_dir) {
		dwc2_write_ep(&p->eps[endpoint][in_dir],
			    job->xfer_length, data_buf);
	} else {
		mps = get_mps(&p->eps[endpoint][in_dir]);
		job->xfer_length = ALIGN_UP(job->xfer_length, mps);
		dwc2_read_ep(&p->eps[endpoint][0], job->xfer_length, data_buf);
	}
}

static void start_ep_transfer(dwc2_pdata_t *p,
			      int endpoint, int in_dir)
{
	int max_transfer_size = (endpoint == 0) ? EP0_MAXLEN : EP_MAXLEN;
	int mps;

	if (p->eps[endpoint][in_dir].busy) {
		usb_debug("ep %d-%d busy\n", endpoint, in_dir);
		return;
	}

	if (SIMPLEQ_EMPTY(&p->eps[endpoint][in_dir].job_queue)) {
		usb_debug("ep %d-%d empty\n", endpoint, in_dir);
		return;
	}

	struct job *job = SIMPLEQ_FIRST(&p->eps[endpoint][in_dir].job_queue);

	job->xfer_length = (job->length > max_transfer_size) ?
			    max_transfer_size : job->length;

	if (in_dir) {
		dwc2_write_ep(&p->eps[endpoint][1], job->xfer_length, job->data);
	} else {
		mps = get_mps(&p->eps[endpoint][0]);
		job->xfer_length = ALIGN_UP(job->xfer_length, mps);
		/* BUG */
		if ((endpoint == 0) && (job->length == 0))
			job->data = p->setup_buf;
		dwc2_read_ep(&p->eps[endpoint][0], job->xfer_length, job->data);
	}

	usb_debug("start EP %d-%d with %zx bytes starting at %p\n", endpoint,
	      in_dir, job->length, job->data);

	p->eps[endpoint][in_dir].busy = 1;
}

static void dwc2_enqueue_packet(struct usbdev_ctrl *this, int endpoint,
	int in_dir, void *data, int len, int zlp, int autofree)
{
	dwc2_pdata_t *p = DWC2_PDATA(this);
	struct job *job = xzalloc(sizeof(*job));

	job->data = data;
	job->length = len;
	job->zlp = zlp;
	job->autofree = autofree;

	usb_debug("adding job %d bytes to EP %d-%d\n", len, endpoint, in_dir);
	SIMPLEQ_INSERT_TAIL(&p->eps[endpoint][in_dir].job_queue, job, queue);

	if ((endpoint == 0) || (this->initialized))
		start_ep_transfer(p, endpoint, in_dir);
}

static void complete_ep_transfer(struct usbdev_ctrl *this, int endpoint,
				 int in_dir, int xfer_result)
{
	dwc2_pdata_t *p = DWC2_PDATA(this);
	struct job *job = SIMPLEQ_FIRST(&p->eps[endpoint][in_dir].job_queue);
	int mps = this->ep_mps[endpoint][in_dir];

	if (in_dir) {
		job->xfered_length += job->xfer_length - xfer_result;
		if (job->xfered_length < job->length ||
		    (job->xfered_length == job->length &&
		     job->xfered_length % mps == 0 && job->xfer_length)) {
			continue_ep_transfer(p, endpoint, in_dir);
			return;
		}
	} else {
		job->xfered_length += job->xfer_length - xfer_result;
	}
	SIMPLEQ_REMOVE_HEAD(&p->eps[endpoint][in_dir].job_queue, queue);

	usb_debug("%d-%d: scheduled %zd, now %zd bytes\n", endpoint, in_dir,
	      job->length, job->xfered_length);

	if (this->current_config &&
	    this->current_config->interfaces[0].handle_packet)
		this->current_config->interfaces[0].handle_packet(this,
			endpoint, in_dir, job->data, job->xfered_length);

	if (job->autofree)
		free(job->data);
	free(job);

	p->eps[endpoint][in_dir].busy = 0;

	if (endpoint == 0 && job->xfered_length == 0)
		dwc2_enqueue_packet(this, 0, 0, p->setup_buf, 8, 0, 0);
	else
		start_ep_transfer(p, endpoint, in_dir);
}

static void dwc2_outep_intr(struct usbdev_ctrl *this, dwc2_ep_t *ep)
{
	dwc2_pdata_t *p = DWC2_PDATA(this);
	depint_t depint;
	depsiz_t depsiz;

	depint.d32 = readl(&ep->ep_regs->depint) &
		     readl(&p->regs->device.doepmsk);

	/* Don't process XferCompl interrupt if it is a setup packet */
	if ((ep->ep_num == 0) && (depint.setup || depint.stuppktrcvd))
		depint.xfercompl = 0;

	/* Transfer completed */
	if (depint.xfercompl) {
		usb_debug("DOEPINT_XFERCOMPL\n");
		writel(DXEPINT_XFERCOMPL, &ep->ep_regs->depint);
		depsiz.d32 = readl(&ep->ep_regs->deptsiz);

		if (ep->ep_num == 0)
			depsiz.xfersize &= 0x7f;

		complete_ep_transfer(this, ep->ep_num, 0, depsiz.xfersize);
	}
	/* Endpoint disable */
	if (depint.epdisbld) {
		usb_debug("DEPINT_EPDISBLD\n");
		writel(DXEPINT_EPDISBLD, &ep->ep_regs->depint);
	}
	/* AHB Error */
	if (depint.ahberr) {
		usb_debug("DEPINT_AHBERR\n");
		writel(DXEPINT_AHBERR, &ep->ep_regs->depint);
	}

	/* Handle Setup Phase Done (Control Ep) */
	if (depint.setup) {
		usb_debug("DEPINT_SETUP\n");
		writel(DXEPINT_SETUP, &ep->ep_regs->depint);
#ifdef USB_DEBUG
		hexdump(p->setup_buf, sizeof(dev_req_t));
#endif
		SIMPLEQ_REMOVE_HEAD(&p->eps[0][0].job_queue, queue);
		p->eps[0][0].busy = 0;

		udc_handle_setup(this, ep->ep_num, (dev_req_t *)p->setup_buf);
	}
}

static void dwc2_inep_intr(struct usbdev_ctrl *this, dwc2_ep_t *ep)
{
	dwc2_pdata_t *p = DWC2_PDATA(this);
	depint_t depint;
	depsiz_t depsiz;

	depint.d32 = readl(&ep->ep_regs->depint) &
		     readl(&p->regs->device.doepmsk);

	/* Don't process XferCompl interrupt if it is a setup packet */
	if ((ep->ep_num == 0) && (depint.setup)) {
		usb_debug("IN ep timeout\n");
		writel(DXEPINT_TIMEOUT, &ep->ep_regs->depint);
	}

	/* Transfer completed */
	if (depint.xfercompl) {
		usb_debug("DIEPINT_XFERCOMPL\n");
		writel(DXEPINT_XFERCOMPL, &ep->ep_regs->depint);
		depsiz.d32 = readl(&ep->ep_regs->deptsiz);

		if (ep->ep_num == 0)
			depsiz.xfersize &= 0x7f;

		complete_ep_transfer(this, ep->ep_num, 1, depsiz.xfersize);
	}
	/* Endpoint disable */
	if (depint.epdisbld) {
		usb_debug("DEPINT_EPDISBLD\n");
		writel(DXEPINT_EPDISBLD, &ep->ep_regs->depint);
	}
	/* AHB Error */
	if (depint.ahberr) {
		usb_debug("DEPINT_AHBERR\n");
		writel(DXEPINT_AHBERR, &ep->ep_regs->depint);
	}
}

static int dwc2_check_irq(struct usbdev_ctrl *this)
{
	dwc2_pdata_t *p = DWC2_PDATA(this);
	gintsts_t gintsts;
	uint32_t daint, daint_out, daint_in, ep;

	gintsts.d32 = readl(&p->regs->core.gintsts) &
		      readl(&p->regs->core.gintmsk);

	if (gintsts.d32 == 0)
		return 1;

	/* EP INTR */
	if (gintsts.oepint || gintsts.iepint) {

		daint = readl(&p->regs->device.daint) &
			readl(&p->regs->device.daintmsk);

		daint_out = daint >> DAINT_OUTEP_SHIFT;
		daint_in = daint & ~(daint_out << DAINT_OUTEP_SHIFT);

		for (ep = 0; ep < MAX_EPS_CHANNELS; ep++, daint_in >>= 1) {
			if (daint_in & 1)
				dwc2_inep_intr(this, &p->eps[ep][1]);
		}

		for (ep = 0; ep < MAX_EPS_CHANNELS; ep++, daint_out >>= 1) {
			if (daint_out & 1)
				dwc2_outep_intr(this, &p->eps[ep][0]);
		}
	}

	/* USB Bus Suspend */
	if (gintsts.usbsusp) {
		usb_debug("GINTSTS_ERLYSUSP\n");
		writel(GINTSTS_USBSUSP, &p->regs->core.gintsts);
	}
	/* USB Bus Reset */
	if (gintsts.usbrst) {
		usb_debug("GINTSTS_USBRST\n");
		dwc2_bus_reset(this);
		writel(GINTSTS_USBRST, &p->regs->core.gintsts);
	}
	/* Enumeration done */
	if (gintsts.enumdone) {
		usb_debug("GINTSTS_ENUMDONE\n");
		dwc2_enum_done(this);
		writel(GINTSTS_ENUMDONE, &p->regs->core.gintsts);
	}
	if (gintsts.sessreqint) {
		usb_debug("GINTSTS_SESSREQINT\n");
		writel(GINTSTS_SESSREQINT, &p->regs->core.gintsts);
	}
	if (gintsts.wkupint) {
		usb_debug("GINTSTS_WKUPINT\n");
		writel(GINTSTS_WKUPINT, &p->regs->core.gintsts);
	}

	return 1;
}

static void dwc2_force_shutdown(struct usbdev_ctrl *this)
{
	gusbcfg_t gusbcfg;
	dwc2_pdata_t *p = DWC2_PDATA(this);

	/* Disconnect */
	dwc2_connect(this, 0);

	/* Back to normal otg mode */
	gusbcfg.d32 = readl(&p->regs->core.gusbcfg);
	gusbcfg.forcehstmode = 0;
	gusbcfg.forcedevmode = 0;
	writel(gusbcfg.d32, &p->regs->core.gusbcfg);

	free(p);
	free(this);
}

static void dwc2_shutdown(struct usbdev_ctrl *this)
{
	dwc2_pdata_t *p = DWC2_PDATA(this);
	int i, j;
	int is_empty = 0;

	uint64_t shutdown_timer_us = timer_us(0);
	/* Wait up to 3 seconds for packets to be flushed out. */
	uint64_t shutdown_timeout_us = 3 * 1000 * 1000UL;

	while ((!is_empty) &&
	       (timer_us(shutdown_timer_us) < shutdown_timeout_us)) {
		is_empty = 1;
		this->poll(this);
		for (i = 0; i < 16; i++)
			for (j = 0; j < 2; j++) {
				/*
				 * EP0-OUT needs to always have an active packet
				 * for proper operation of control packet
				 * flow. Thus, ignore if only 1 packet is
				 * present in EP0-OUT.
				 */
				if ((i == 0) && (j == 0) &&
				    SIMPLEQ_SINGLETON(&p->eps[0][0].job_queue,
						      queue))
					continue;

				if (!SIMPLEQ_EMPTY(&p->eps[i][j].job_queue))
					is_empty = 0;
			}
	}

	if (timer_us(shutdown_timer_us) >= shutdown_timeout_us)
		usb_debug("Error: Failed to empty queues.. timeout\n");

	dwc2_force_shutdown(this);
}

static void dwc2_set_address(struct usbdev_ctrl *this, int address)
{
	dwc2_pdata_t *p = DWC2_PDATA(this);
	dcfg_t dcfg;

	dcfg.d32 = readl(&p->regs->device.dcfg);
	dcfg.devaddr = address;
	writel(dcfg.d32, &p->regs->device.dcfg);
}

static void dwc2_stall(struct usbdev_ctrl *this,
			     uint8_t ep, int in_dir, int set)
{
	dwc2_pdata_t *p = DWC2_PDATA(this);
	dwc2_ep_reg_t *ep_reg = p->eps[ep][in_dir].ep_regs;
	depctl_t depctl;

	usb_debug("dwc2_stall\n");
	depctl.d32 = readl(&ep_reg->depctl);

	in_dir = in_dir ? 1 : 0;

	if (set) {
		depctl.stall = 1;
		depctl.setd0pid = 1;
		writel(depctl.d32, &ep_reg->depctl);
	} else {
		/* STALL bit will be clear by core */
	}
	this->ep_halted[ep][in_dir] = set;
}

static void *dwc2_malloc(size_t size)
{
	return dma_memalign(4096, size);
}

static void dwc2_free(void *ptr)
{
	free(ptr);
}

static int dwc2_reinit_udc(struct usbdev_ctrl *this, void *_opreg,
			 const device_descriptor_t *dd)
{
	grstctl_t grstctl = { .d32 = 0 };
	gintmsk_t gintmsk = { .d32 = 0 };
	gahbcfg_t gahbcfg = { .d32 = 0 };
	gusbcfg_t gusbcfg = { .d32 = 0 };
	grxfsiz_t grxfsiz = { .d32 = 0 };
	dtxfsiz_t dtxfsiz0 = { .d32 = 0 };
	dtxfsiz_t dtxfsiz1 = { .d32 = 0 };
	dtxfsiz_t dtxfsiz2 = { .d32 = 0 };
	depint_t depint_msk = { .d32 = 0 };
	dcfg_t dcfg = { .d32 = 0 };
	dwc2_reg_t *regs = (dwc2_reg_t *)_opreg;
	dwc2_pdata_t *p = DWC2_PDATA(this);
	const int timeout = 10000;
	int i;

	p->regs = phys_to_virt(regs);
	p->fifo_map = 0;
	p->setup_buf = dma_memalign(4, 64);

	for (i = 0; i < MAX_EPS_CHANNELS; i++) {
		/* Init OUT EPs */
		p->eps[i][0].ep_num = i;
		p->eps[i][0].ep_regs = &regs->device.outep[i];
		SIMPLEQ_INIT(&p->eps[i][0].job_queue);

		/* Init IN EPs */
		p->eps[i][1].ep_num = i;
		p->eps[i][1].ep_regs = &regs->device.inep[i];
		SIMPLEQ_INIT(&p->eps[i][1].job_queue);
	}

	usb_debug("dwc2_hw_init\n");

	/* Wait for AHB idle */
	for (i = 0; i < timeout; i++) {
		udelay(1);
		grstctl.d32 = readl(&regs->core.grstctl);
		if (grstctl.ahbidle)
			break;
	}
	if (i == timeout) {
		usb_debug("DWC2 Init error AHB Idle\n");
		return 0;
	}

	/* Restart the Phy Clock */
	/* Core soft reset */
	grstctl.csftrst = 1;
	writel(grstctl.d32, &regs->core.grstctl);
	for (i = 0; i <= timeout; i++) {
		udelay(1);
		grstctl.d32 = readl(&regs->core.grstctl);
		if (!grstctl.csftrst)
			break;

	if (i == timeout) {
		usb_debug("DWC2 Init error reset fail\n");
		return 0;
		}
	}

	/* Restart the Phy Clock */
	writel(0x0, &regs->pcgr.pcgcctl);

	/* Set 16bit PHY if & Force host mode */
	gusbcfg.d32 = readl(&regs->core.gusbcfg);
	gusbcfg.phyif = 1;
	gusbcfg.forcehstmode = 0;
	gusbcfg.forcedevmode = 1;
	writel(gusbcfg.d32, &regs->core.gusbcfg);

	dcfg.d32 = readl(&regs->device.dcfg);
	/* reset device addr */
	dcfg.devaddr = 0;
	/* enable HS */
	dcfg.devspd = 0;
	writel(dcfg.d32, &regs->device.dcfg);

	dwc2_tx_fifo_flush(this, 0x10);
	dwc2_rx_fifo_flush(this, 0);

	grxfsiz.rxfdep = RX_FIFO_SIZE;
	writel(grxfsiz.d32, &regs->core.grxfsiz);

	dtxfsiz0.dtxfdep = DTX_FIFO_SIZE_0;
	dtxfsiz0.dtxfstaddr = DTX_FIFO_SIZE_0_OFFSET;
	writel(dtxfsiz0.d32, &regs->core.gnptxfsiz);

	dtxfsiz1.dtxfdep = DTX_FIFO_SIZE_1;
	dtxfsiz1.dtxfstaddr = DTX_FIFO_SIZE_1_OFFSET;
	writel(dtxfsiz1.d32, &regs->core.dptxfsiz_dieptxf[0]);

	dtxfsiz2.dtxfdep = DTX_FIFO_SIZE_2;
	dtxfsiz2.dtxfstaddr = DTX_FIFO_SIZE_2_OFFSET;
	writel(dtxfsiz2.d32, &regs->core.dptxfsiz_dieptxf[1]);

	/* Config Ep0 */
	dwc2_start_ep0(this);

	dwc2_enqueue_packet(this, 0, 0, p->setup_buf, 8, 0, 0);

	depint_msk.xfercompl = 1;
	depint_msk.epdisbld = 1;
	depint_msk.ahberr = 1;
	depint_msk.setup = 1;

	/* device IN interrupt mask */
	writel(depint_msk.d32, &regs->device.diepmsk);
	/* device OUT interrupt mask */
	writel(depint_msk.d32, &regs->device.doepmsk);

	/* Clear all pending interrupt */
	writel(0xffffffff, &regs->device.daint);

	/* Config core interface regs */
	writel(0xffffffff, &regs->core.gintsts);
	writel(0xffffffff, &regs->core.gotgint);

	/* Enable device endpoint interrupt */
	writel(0xffffffff, &regs->device.daintmsk);

	gintmsk.usbsusp = 1;
	gintmsk.usbrst = 1;
	gintmsk.enumdone = 1;
	gintmsk.sessreqint = 1;
	gintmsk.iepint = 1;
	gintmsk.oepint = 1;
	writel(gintmsk.d32, &regs->core.gintmsk);

	gahbcfg.d32 = readl(&regs->core.gahbcfg);
	gahbcfg.dmaen = 1;
	gahbcfg.glblintrmsk = 1;
	gahbcfg.hbstlen = DMA_BURST_INCR16;
	writel(gahbcfg.d32, &regs->core.gahbcfg);

	dwc2_connect(this, 1);

	return 1;

}

struct usbdev_ctrl *dwc2_udc_init(device_descriptor_t *dd)
{
	struct usbdev_ctrl *ctrl = calloc(1, sizeof(*ctrl));
	int i;

	usb_debug("dwc2_udc_init\n");
	if (ctrl == NULL)
		return NULL;

	ctrl->pdata = calloc(1, sizeof(dwc2_pdata_t));
	if (ctrl->pdata == NULL) {
		free(ctrl);
		return NULL;
	}
	memcpy(&ctrl->device_descriptor, dd, sizeof(*dd));
	SLIST_INIT(&ctrl->configs);

	ctrl->poll = dwc2_check_irq;
	ctrl->add_gadget = udc_add_gadget;
	ctrl->add_strings = udc_add_strings;
	ctrl->enqueue_packet = dwc2_enqueue_packet;
	ctrl->force_shutdown = dwc2_force_shutdown;
	ctrl->shutdown = dwc2_shutdown;
	ctrl->set_address = dwc2_set_address;
	ctrl->stall = dwc2_stall;
	ctrl->halt_ep = dwc2_halt_ep;
	ctrl->start_ep = dwc2_start_ep;
	ctrl->alloc_data = dwc2_malloc;
	ctrl->free_data = dwc2_free;
	ctrl->initialized = 0;

	ctrl->ep_mps[0][0] = 64;
	ctrl->ep_mps[0][1] = 64;
	for (i = 1; i < 16; i++) {
		ctrl->ep_mps[i][0] = 512;
		ctrl->ep_mps[i][1] = 512;
	}

	if (!dwc2_reinit_udc(ctrl, (void *)0xff580000, dd)) {
		free(ctrl->pdata);
		free(ctrl);
		return NULL;
	}

	return ctrl;
}
