/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Rockchip Electronics
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

#include <libpayload.h>
#include <arch/cache.h>

#include "dwc2.h"
#include "dwc2_private.h"

static void dummy(hci_t *controller)
{
}

static void dwc2_reinit(hci_t *controller)
{
	dwc2_reg_t *reg = DWC2_REG(controller);
	gusbcfg_t gusbcfg = { .d32 = 0 };
	grstctl_t grstctl = { .d32 = 0 };
	gintsts_t gintsts = { .d32 = 0 };
	gahbcfg_t gahbcfg = { .d32 = 0 };
	grxfsiz_t grxfsiz = { .d32 = 0 };
	ghwcfg3_t hwcfg3 = { .d32 = 0 };
	hcintmsk_t hcintmsk = { .d32 = 0 };
	gnptxfsiz_t gnptxfsiz = { .d32 = 0 };

	const int timeout = 10000;
	int i, fifo_blocks, tx_blocks;

	/* Wait for AHB idle */
	for (i = 0; i < timeout; i++) {
		udelay(1);
		grstctl.d32 = readl(&reg->core.grstctl);
		if (grstctl.ahbidle)
			break;
	}
	if (i == timeout)
		fatal("DWC2 Init error AHB Idle\n");

	/* Restart the Phy Clock */
	writel(0x0, &reg->pcgr.pcgcctl);
	/* Core soft reset */
	grstctl.csftrst = 1;
	writel(grstctl.d32, &reg->core.grstctl);
	for (i = 0; i < timeout; i++) {
		udelay(1);
		grstctl.d32 = readl(&reg->core.grstctl);
		if (!grstctl.csftrst)
			break;
	}
	if (i == timeout)
		fatal("DWC2 Init error reset fail\n");

	/* Set 16bit PHY if & Force host mode */
	gusbcfg.d32 = readl(&reg->core.gusbcfg);
	gusbcfg.phyif = 1;
	gusbcfg.forcehstmode = 1;
	gusbcfg.forcedevmode = 0;
	writel(gusbcfg.d32, &reg->core.gusbcfg);
	/* Wait for force host mode effect, it may takes 100ms */
	for (i = 0; i < timeout; i++) {
		udelay(10);
		gintsts.d32 = readl(&reg->core.gintsts);
		if (gintsts.curmod)
			break;
	}
	if (i == timeout)
		fatal("DWC2 Init error force host mode fail\n");

	/*
	 * Config FIFO
	 * The non-periodic tx fifo and rx fifo share one continuous
	 * piece of IP-internal SRAM.
	 */

	/*
	 * Read total data FIFO depth from HWCFG3
	 * this value is in terms of 32-bit words
	 */
	hwcfg3.d32 = readl(&reg->core.ghwcfg3);
	/*
	 * Reserve 2 spaces for the status entries of received packets
	 * and 2 spaces for bulk and control OUT endpoints. Calculate how
	 * many blocks can be alloted, assume largest packet size is 512.
	 */
	fifo_blocks = (hwcfg3.dfifodepth - 4) / (512 / 4);
	tx_blocks = fifo_blocks / 2;

	grxfsiz.rxfdep = (fifo_blocks - tx_blocks) * (512 / 4) + 4;
	writel(grxfsiz.d32, &reg->core.grxfsiz);
	gnptxfsiz.nptxfstaddr = grxfsiz.rxfdep;
	gnptxfsiz.nptxfdep = tx_blocks * (512 / 4);
	writel(gnptxfsiz.d32, &reg->core.gnptxfsiz);

	/* Init host channels */
	hcintmsk.xfercomp = 1;
	hcintmsk.xacterr = 1;
	hcintmsk.stall = 1;
	hcintmsk.chhltd = 1;
	hcintmsk.bblerr = 1;
	for (i = 0; i < MAX_EPS_CHANNELS; i++)
		writel(hcintmsk.d32, &reg->host.hchn[i].hcintmaskn);

	/* Unmask interrupt and configure DMA mode */
	gahbcfg.glblintrmsk = 1;
	gahbcfg.hbstlen = DMA_BURST_INCR8;
	gahbcfg.dmaen = 1;
	writel(gahbcfg.d32, &reg->core.gahbcfg);

	DWC2_INST(controller)->hprt0 = &reg->host.hprt;

	usb_debug("DWC2 init finished!\n");
}

static void dwc2_shutdown(hci_t *controller)
{
	detach_controller(controller);
	free(DWC2_INST(controller)->dma_buffer);
	free(DWC2_INST(controller));
	free(controller);
}

/*
 * This function returns the actual transfer length when the transfer succeeded
 * or an error code if the transfer failed
 */
static int
wait_for_complete(endpoint_t *ep, uint32_t ch_num)
{
	hcint_t hcint;
	hcchar_t hcchar;
	hctsiz_t hctsiz;
	dwc2_reg_t *reg = DWC2_REG(ep->dev->controller);
	int timeout = 600000; /* time out after 600000 * 5us == 3s */

	/*
	 * TODO: We should take care of up to three times of transfer error
	 * retry here, according to the USB 2.0 spec 4.5.2
	 */
	do {
		udelay(5);
		hcint.d32 = readl(&reg->host.hchn[ch_num].hcintn);
		hctsiz.d32 = readl(&reg->host.hchn[ch_num].hctsizn);

		if (hcint.chhltd) {
			writel(hcint.d32, &reg->host.hchn[ch_num].hcintn);

			if (hcint.xfercomp)
				return hctsiz.xfersize;
			else if (hcint.xacterr)
				return -HCSTAT_XFERERR;
			else if (hcint.bblerr)
				return -HCSTAT_BABBLE;
			else if (hcint.stall)
				return -HCSTAT_STALL;
			else
				return -HCSTAT_UNKNOW;
		}
	} while (timeout--);

	/* Release the channel on timeout */
	hcchar.d32 = readl(&reg->host.hchn[ch_num].hccharn);
	if (hcchar.chen) {
		/*
		 * Programming the HCCHARn register with the chdis and
		 * chena bits set to 1 at the same time to disable the
		 * channel and the core will generate a channel halted
		 * interrupt.
		 */
		 hcchar.chdis = 1;
		 writel(hcchar.d32, &reg->host.hchn[ch_num].hccharn);
		 do {
			hcchar.d32 = readl(&reg->host.hchn[ch_num].hccharn);
		 } while (hcchar.chen);

	}

	/* Clear all pending interrupt flags */
	hcint.d32 = ~0;
	writel(hcint.d32, &reg->host.hchn[ch_num].hcintn);

	return -HCSTAT_TIMEOUT;
}

static int
dwc2_transfer(endpoint_t *ep, int size, int pid, ep_dir_t dir,
			  uint32_t ch_num, u8 *data_buf)
{
	uint32_t do_copy;
	int ret;
	uint32_t packet_cnt;
	uint32_t packet_size;
	uint32_t transferred = 0;
	uint32_t inpkt_length;
	hctsiz_t hctsiz = { .d32 = 0 };
	hcchar_t hcchar = { .d32 = 0 };
	void *aligned_buf;
	dwc2_reg_t *reg = DWC2_REG(ep->dev->controller);

	packet_size = ep->maxpacketsize;
	packet_cnt = ALIGN_UP(size, packet_size) / packet_size;
	inpkt_length = packet_cnt * packet_size;
	/* At least 1 packet should be programed */
	packet_cnt = (packet_cnt == 0) ? 1 : packet_cnt;

	/*
	 * For an IN, this field is the buffer size that the application has
	 * reserved for the transfer. The application should program this field
	 * as integer multiple of the maximum packet size for IN transactions.
	 */
	hctsiz.xfersize = (dir == EPDIR_OUT) ? size : inpkt_length;
	hctsiz.pktcnt = packet_cnt;
	hctsiz.pid = pid;

	hcchar.mps = packet_size;
	hcchar.epnum = ep->endpoint & 0xf;
	hcchar.epdir = dir;
	hcchar.eptype = ep->type;
	hcchar.multicnt = 1;
	hcchar.devaddr = ep->dev->address;
	hcchar.chdis = 0;
	hcchar.chen = 1;

	if (size > DMA_SIZE) {
		usb_debug("Transfer too large: %d\n", size);
		return -1;
	}

	/*
	 * Check the buffer address which should be 4-byte aligned and DMA
	 * coherent
	 */
	do_copy = !dma_coherent(data_buf) || ((uintptr_t)data_buf & 0x3);
	aligned_buf = do_copy ? DWC2_INST(ep->dev->controller)->dma_buffer :
		      data_buf;

	if (do_copy && (dir == EPDIR_OUT))
		memcpy(aligned_buf, data_buf, size);

	writel(hctsiz.d32, &reg->host.hchn[ch_num].hctsizn);
	writel((uint32_t)virt_to_bus(aligned_buf),
		&reg->host.hchn[ch_num].hcdman);
	writel(hcchar.d32, &reg->host.hchn[ch_num].hccharn);

	ret = wait_for_complete(ep, ch_num);

	if (ret >= 0) {
		/* Calculate actual transferred length */
		transferred = (dir == EPDIR_IN) ? inpkt_length - ret : ret;

		if (do_copy && (dir == EPDIR_IN))
			memcpy(data_buf, aligned_buf, transferred);
	}

	/* Save data toggle */
	hctsiz.d32 = readl(&reg->host.hchn[ch_num].hctsizn);
	ep->toggle = hctsiz.pid;

	if (ret < 0) {
		usb_debug("%s Transfer stop code: %x\n", __func__, ret);
		return ret;
	}
	return transferred;
}

static int
dwc2_bulk(endpoint_t *ep, int size, u8 *src, int finalize)
{
	ep_dir_t data_dir;

	if (ep->direction == IN)
		data_dir = EPDIR_IN;
	else if (ep->direction == OUT)
		data_dir = EPDIR_OUT;
	else
		return -1;

	return dwc2_transfer(ep, size, ep->toggle, data_dir, 0, src);
}

static int
dwc2_control(usbdev_t *dev, direction_t dir, int drlen, void *setup,
		   int dalen, u8 *src)
{
	int ret = 0;

	ep_dir_t data_dir;

	if (dir == IN)
		data_dir = EPDIR_IN;
	else if (dir == OUT)
		data_dir = EPDIR_OUT;
	else
		return -1;

	/* Setup Phase */
	if (dwc2_transfer(&dev->endpoints[0], drlen, PID_SETUP, EPDIR_OUT, 0,
	    setup) < 0)
		return -1;
	/* Data Phase */
	if (dalen > 0) {
		ret = dwc2_transfer(&dev->endpoints[0], dalen, PID_DATA1,
				    data_dir, 0, src);
		if (ret < 0)
			return -1;
	}
	/* Status Phase */
	if (dwc2_transfer(&dev->endpoints[0], 0, PID_DATA1, !data_dir, 0,
	    NULL) < 0)
		return -1;

	return ret;
}

hci_t *dwc2_init(void *bar)
{
	hci_t *controller = new_controller();
	controller->instance = xzalloc(sizeof(dwc_ctrl_t));

	DWC2_INST(controller)->dma_buffer = dma_malloc(DMA_SIZE);
	if (!DWC2_INST(controller)->dma_buffer) {
		usb_debug("Not enough DMA memory for DWC2 bounce buffer\n");
		goto free_dwc2;
	}

	controller->type = DWC2;
	controller->start = dummy;
	controller->stop = dummy;
	controller->reset = dummy;
	controller->init = dwc2_reinit;
	controller->shutdown = dwc2_shutdown;
	controller->bulk = dwc2_bulk;
	controller->control = dwc2_control;
	controller->set_address = generic_set_address;
	controller->finish_device_config = NULL;
	controller->destroy_device = NULL;
	controller->create_intr_queue = NULL;
	controller->destroy_intr_queue = NULL;
	controller->poll_intr_queue = NULL;
	controller->reg_base = (uintptr_t)bar;
	init_device_entry(controller, 0);

	/* Init controller */
	controller->init(controller);

	/* Setup up root hub */
	controller->devices[0]->controller = controller;
	controller->devices[0]->init = dwc2_rh_init;
	controller->devices[0]->init(controller->devices[0]);
	return controller;

free_dwc2:
	detach_controller(controller);
	free(DWC2_INST(controller));
	free(controller);
	return NULL;
}
