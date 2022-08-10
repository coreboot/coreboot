/*
 *
 * Copyright (C) 2010 Patrick Georgi
 * Copyright (C) 2013 secunet Security Networks AG
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

//#define XHCI_SPEW_DEBUG

#include <inttypes.h>
#include <arch/virtual.h>
#include "xhci_private.h"
#include "xhci.h"

static void xhci_start(hci_t *controller);
static void xhci_stop(hci_t *controller);
static void xhci_reset(hci_t *controller);
static void xhci_reinit(hci_t *controller);
static void xhci_shutdown(hci_t *controller);
static int xhci_bulk(endpoint_t *ep, int size, u8 *data, int finalize);
static int xhci_control(usbdev_t *dev, direction_t dir, int drlen, void *devreq,
			 int dalen, u8 *data);
static void* xhci_create_intr_queue(endpoint_t *ep, int reqsize, int reqcount, int reqtiming);
static void xhci_destroy_intr_queue(endpoint_t *ep, void *queue);
static u8* xhci_poll_intr_queue(void *queue);

/*
 * Some structures must not cross page boundaries. To get this,
 * we align them by their size (or the next greater power of 2).
 */
void *
xhci_align(const size_t min_align, const size_t size)
{
	size_t align;
	if (!(size & (size - 1)))
		align = size; /* It's a power of 2 */
	else
		align = 1 << ((sizeof(unsigned) << 3) - __builtin_clz(size));
	if (align < min_align)
		align = min_align;
	xhci_spew("Aligning %zu to %zu\n", size, align);
	return dma_memalign(align, size);
}

void
xhci_clear_trb(trb_t *const trb, const int pcs)
{
	trb->ptr_low	= 0;
	trb->ptr_high	= 0;
	trb->status	= 0;
	trb->control	= !pcs;
}

void
xhci_init_cycle_ring(transfer_ring_t *const tr, const size_t ring_size)
{
	memset((void *)tr->ring, 0, ring_size * sizeof(*tr->ring));
	TRB_SET(TT, &tr->ring[ring_size - 1], TRB_LINK);
	TRB_SET(TC, &tr->ring[ring_size - 1], 1);
	/* only one segment that points to itself */
	tr->ring[ring_size - 1].ptr_low = virt_to_phys(tr->ring);

	tr->pcs = 1;
	tr->cur = tr->ring;
}

/* On Panther Point: switch ports shared with EHCI to xHCI */
#if CONFIG(LP_USB_PCI)
static void
xhci_switch_ppt_ports(pcidev_t addr)
{
	if (pci_read_config32(addr, 0x00) == 0x1e318086) {
		u32 reg32 = pci_read_config32(addr, 0xdc) & 0xf;
		xhci_debug("Ports capable of SuperSpeed: 0x%"PRIx32"\n", reg32);

		/* For now, do not enable SuperSpeed on any ports */
		//pci_write_config32(addr, 0xd8, reg32);
		pci_write_config32(addr, 0xd8, 0x00000000);
		reg32 = pci_read_config32(addr, 0xd8) & 0xf;
		xhci_debug("Configured for SuperSpeed:   0x%"PRIx32"\n", reg32);

		reg32 = pci_read_config32(addr, 0xd4) & 0xf;
		xhci_debug("Trying to switch over:       0x%"PRIx32"\n", reg32);

		pci_write_config32(addr, 0xd0, reg32);
		reg32 = pci_read_config32(addr, 0xd0) & 0xf;
		xhci_debug("Actually switched over:      0x%"PRIx32"\n", reg32);
	}
}
#endif

#if CONFIG(LP_USB_PCI)
/* On Panther Point: switch all ports back to EHCI */
static void
xhci_switchback_ppt_ports(pcidev_t addr)
{
	if (pci_read_config32(addr, 0x00) == 0x1e318086) {
		u32 reg32 = pci_read_config32(addr, 0xd0) & 0xf;
		xhci_debug("Switching ports back:   0x%"PRIx32"\n", reg32);
		pci_write_config32(addr, 0xd0, 0x00000000);
		reg32 = pci_read_config32(addr, 0xd0) & 0xf;
		xhci_debug("Still switched to xHCI: 0x%"PRIx32"\n", reg32);
	}
}
#endif

static long
xhci_handshake(volatile u32 *const reg, u32 mask, u32 wait_for, long timeout_us)
{
	if (timeout_us <= 0)
		return 0;
	while ((*reg & mask) != wait_for && timeout_us != 0) {
		--timeout_us;
		udelay(1);
	}
	return timeout_us;
}

static int
xhci_wait_ready(xhci_t *const xhci)
{
	xhci_debug("Waiting for controller to be ready... ");
	if (!xhci_handshake(&xhci->opreg->usbsts, USBSTS_CNR, 0, 100000L)) {
		usb_debug("timeout!\n");
		return -1;
	}
	usb_debug("ok.\n");
	return 0;
}

hci_t *
xhci_init(unsigned long physical_bar)
{
	int i;

	/* First, allocate and initialize static controller structures */

	hci_t *const controller = new_controller();
	controller->type		= XHCI;
	controller->start		= xhci_start;
	controller->stop		= xhci_stop;
	controller->reset		= xhci_reset;
	controller->init		= xhci_reinit;
	controller->shutdown		= xhci_shutdown;
	controller->bulk		= xhci_bulk;
	controller->control		= xhci_control;
	controller->set_address		= xhci_set_address;
	controller->finish_device_config = xhci_finish_device_config;
	controller->destroy_device	= xhci_destroy_dev;
	controller->create_intr_queue	= xhci_create_intr_queue;
	controller->destroy_intr_queue	= xhci_destroy_intr_queue;
	controller->poll_intr_queue	= xhci_poll_intr_queue;
	controller->pcidev		= 0;

	controller->reg_base = (uintptr_t)physical_bar;
	controller->instance = xzalloc(sizeof(xhci_t));
	xhci_t *const xhci = (xhci_t *)controller->instance;

	init_device_entry(controller, 0);
	xhci->roothub = controller->devices[0];
	xhci->cr.ring = xhci_align(64, COMMAND_RING_SIZE * sizeof(trb_t));
	xhci->er.ring = xhci_align(64, EVENT_RING_SIZE * sizeof(trb_t));
	xhci->ev_ring_table = xhci_align(64, sizeof(erst_entry_t));
	if (!xhci->roothub || !xhci->cr.ring ||
			!xhci->er.ring || !xhci->ev_ring_table) {
		xhci_debug("Out of memory\n");
		goto _free_xhci;
	}

	xhci->capreg = phys_to_virt(physical_bar);
	xhci->opreg = phys_to_virt(physical_bar) + CAP_GET(CAPLEN, xhci->capreg);
	xhci->hcrreg = phys_to_virt(physical_bar) + xhci->capreg->rtsoff;
	xhci->dbreg = phys_to_virt(physical_bar) + xhci->capreg->dboff;

	xhci_debug("regbase: 0x%"PRIxPTR"\n", physical_bar);
	xhci_debug("caplen:  0x%"PRIx32"\n", CAP_GET(CAPLEN, xhci->capreg));
	xhci_debug("rtsoff:  0x%"PRIx32"\n", xhci->capreg->rtsoff);
	xhci_debug("dboff:   0x%"PRIx32"\n", xhci->capreg->dboff);

	xhci_debug("hciversion: %"PRIx8".%"PRIx8"\n",
		   CAP_GET(CAPVER_HI, xhci->capreg), CAP_GET(CAPVER_LO, xhci->capreg));
	if ((CAP_GET(CAPVER, xhci->capreg) < 0x96) ||
	    (CAP_GET(CAPVER, xhci->capreg) > 0x120)) {
		xhci_debug("Unsupported xHCI version\n");
		goto _free_xhci;
	}

	xhci_debug("context size: %dB\n", CTXSIZE(xhci));
	xhci_debug("maxslots: 0x%02"PRIx32"\n", CAP_GET(MAXSLOTS, xhci->capreg));
	xhci_debug("maxports: 0x%02"PRIx32"\n", CAP_GET(MAXPORTS, xhci->capreg));
	const unsigned pagesize = xhci->opreg->pagesize << 12;
	xhci_debug("pagesize: 0x%04x\n", pagesize);

	/*
	 * We haven't touched the hardware yet. So we allocate all dynamic
	 * structures at first and can still chicken out easily if we run out
	 * of memory.
	 */
	xhci->max_slots_en = CAP_GET(MAXSLOTS, xhci->capreg) &
		CONFIG_LP_MASK_MaxSlotsEn;
	xhci->dcbaa = xhci_align(64, (xhci->max_slots_en + 1) * sizeof(u64));
	xhci->dev = malloc((xhci->max_slots_en + 1) * sizeof(*xhci->dev));
	if (!xhci->dcbaa || !xhci->dev) {
		xhci_debug("Out of memory\n");
		goto _free_xhci;
	}
	memset(xhci->dcbaa, 0x00, (xhci->max_slots_en + 1) * sizeof(u64));
	memset(xhci->dev, 0x00, (xhci->max_slots_en + 1) * sizeof(*xhci->dev));

	/*
	 * Let dcbaa[0] point to another array of pointers, sp_ptrs.
	 * The pointers therein point to scratchpad buffers (pages).
	 */
	const size_t max_sp_bufs =
		CAP_GET(MAX_SCRATCH_BUFS_HI, xhci->capreg) << 5 |
		CAP_GET(MAX_SCRATCH_BUFS_LO, xhci->capreg);
	xhci_debug("max scratchpad bufs: 0x%zx\n", max_sp_bufs);
	if (max_sp_bufs) {
		const size_t sp_ptrs_size = max_sp_bufs * sizeof(u64);
		xhci->sp_ptrs = xhci_align(64, sp_ptrs_size);
		if (!xhci->sp_ptrs) {
			xhci_debug("Out of memory\n");
			goto _free_xhci_structs;
		}
		memset(xhci->sp_ptrs, 0x00, sp_ptrs_size);
		for (i = 0; i < max_sp_bufs; ++i) {
			/* Could use mmap() here if we had it.
			   Maybe there is another way. */
			void *const page = memalign(pagesize, pagesize);
			if (!page) {
				xhci_debug("Out of memory\n");
				goto _free_xhci_structs;
			}
			xhci->sp_ptrs[i] = virt_to_phys(page);
		}
		xhci->dcbaa[0] = virt_to_phys(xhci->sp_ptrs);
	}

	if (dma_initialized()) {
		xhci->dma_buffer = dma_memalign(64 * 1024, DMA_SIZE);
		if (!xhci->dma_buffer) {
			xhci_debug("Not enough memory for DMA bounce buffer\n");
			goto _free_xhci_structs;
		}
	}

	/* Now start working on the hardware */
	if (xhci_wait_ready(xhci))
		goto _free_xhci_structs;

	/* TODO: Check if BIOS claims ownership (and hand over) */

	xhci_reset(controller);
	xhci_reinit(controller);

	xhci->roothub->controller = controller;
	xhci->roothub->init = xhci_rh_init;
	xhci->roothub->init(xhci->roothub);

	return controller;

_free_xhci_structs:
	free(xhci->dma_buffer);
	if (xhci->sp_ptrs) {
		for (i = 0; i < max_sp_bufs; ++i) {
			if (xhci->sp_ptrs[i])
				free(phys_to_virt(xhci->sp_ptrs[i]));
		}
	}
	free(xhci->sp_ptrs);
	free(xhci->dcbaa);
_free_xhci:
	free((void *)xhci->ev_ring_table);
	free((void *)xhci->er.ring);
	free((void *)xhci->cr.ring);
	free(xhci->roothub);
	free(xhci->dev);
	free(xhci);
/* _free_controller: */
	detach_controller(controller);
	free(controller);
	return NULL;
}

#if CONFIG(LP_USB_PCI)
hci_t *
xhci_pci_init(pcidev_t addr)
{
	u32 reg_addr;
	hci_t *controller;

	reg_addr = pci_read_config32(addr, PCI_BASE_ADDRESS_0) &
		   PCI_BASE_ADDRESS_MEM_MASK;
	if (pci_read_config32(addr, PCI_BASE_ADDRESS_1) > 0)
		fatal("We don't do 64bit addressing.\n");

	controller = xhci_init((unsigned long)reg_addr);
	if (controller) {
		xhci_t *xhci = controller->instance;
		controller->pcidev = addr;

		xhci_switch_ppt_ports(addr);

		/* Set up any quirks for controller root hub */
		xhci->roothub->quirks = pci_quirk_check(addr);
	}

	return controller;
}
#endif

static void
xhci_reset(hci_t *const controller)
{
	xhci_t *const xhci = XHCI_INST(controller);

	xhci_stop(controller);

	xhci->opreg->usbcmd |= USBCMD_HCRST;

	/* Existing Intel xHCI controllers require a delay of 1 ms,
	 * after setting the CMD_RESET bit, and before accessing any
	 * HC registers. This allows the HC to complete the
	 * reset operation and be ready for HC register access.
	 * Without this delay, the subsequent HC register access,
	 * may result in a system hang very rarely.
	 */
	if (CONFIG(LP_ARCH_X86))
		mdelay(1);

	xhci_debug("Resetting controller... ");
	if (!xhci_handshake(&xhci->opreg->usbcmd, USBCMD_HCRST, 0, 1000000L))
		usb_debug("timeout!\n");
	else
		usb_debug("ok.\n");
}

static void
xhci_reinit(hci_t *controller)
{
	xhci_t *const xhci = XHCI_INST(controller);

	if (xhci_wait_ready(xhci))
		return;

	/* Enable all available slots */
	xhci->opreg->config = xhci->max_slots_en;

	/* Set DCBAA */
	xhci->opreg->dcbaap_lo = virt_to_phys(xhci->dcbaa);
	xhci->opreg->dcbaap_hi = 0;

	/* Initialize command ring */
	xhci_init_cycle_ring(&xhci->cr, COMMAND_RING_SIZE);
	xhci_debug("command ring @%p (0x%08"PRIxPTR")\n",
		   xhci->cr.ring, virt_to_phys(xhci->cr.ring));
	xhci->opreg->crcr_lo = virt_to_phys(xhci->cr.ring) | CRCR_RCS;
	xhci->opreg->crcr_hi = 0;

	/* Make sure interrupts are disabled */
	xhci->opreg->usbcmd &= ~USBCMD_INTE;

	/* Initialize event ring */
	xhci_reset_event_ring(&xhci->er);
	xhci_debug("event ring @%p (0x%08"PRIxPTR")\n",
		   xhci->er.ring, virt_to_phys(xhci->er.ring));
	xhci_debug("ERST Max: 0x%"PRIx32" ->  0x%x entries\n",
		   CAP_GET(ERST_MAX, xhci->capreg),
		   1 << CAP_GET(ERST_MAX, xhci->capreg));
	memset((void*)xhci->ev_ring_table, 0x00, sizeof(erst_entry_t));
	xhci->ev_ring_table[0].seg_base_lo = virt_to_phys(xhci->er.ring);
	xhci->ev_ring_table[0].seg_base_hi = 0;
	xhci->ev_ring_table[0].seg_size = EVENT_RING_SIZE;

	/* pass event ring table to hardware */
	wmb();
	/* Initialize primary interrupter */
	xhci->hcrreg->intrrs[0].erstsz = 1;
	xhci_update_event_dq(xhci);
	/* erstba has to be written at last */
	xhci->hcrreg->intrrs[0].erstba_lo = virt_to_phys(xhci->ev_ring_table);
	xhci->hcrreg->intrrs[0].erstba_hi = 0;

	xhci_start(controller);

#ifdef USB_DEBUG
	int i;
	for (i = 0; i < 32; ++i) {
		xhci_debug("NOOP run #%d\n", i);
		trb_t *const cmd = xhci_next_command_trb(xhci);
		TRB_SET(TT, cmd, TRB_CMD_NOOP);

		xhci_post_command(xhci);

		/* Wait for result in event ring */
		int cc = xhci_wait_for_command_done(xhci, cmd, 1);

		xhci_debug("Command ring is %srunning: cc: %d\n",
			   (xhci->opreg->crcr_lo & CRCR_CRR) ? "" : "not ", cc);
		if (cc != CC_SUCCESS)
			xhci_debug("noop command failed.\n");
	}
#endif
}

static void
xhci_shutdown(hci_t *const controller)
{
	int i;

	if (controller == 0)
		return;

	detach_controller(controller);

	xhci_t *const xhci = XHCI_INST(controller);
	xhci_stop(controller);

#if CONFIG(LP_USB_PCI)
        if (controller->pcidev)
		xhci_switchback_ppt_ports(controller->pcidev);
#endif

	if (xhci->sp_ptrs) {
		const size_t max_sp_bufs =
			CAP_GET(MAX_SCRATCH_BUFS_HI, xhci->capreg) << 5 |
			CAP_GET(MAX_SCRATCH_BUFS_LO, xhci->capreg);
		for (i = 0; i < max_sp_bufs; ++i) {
			if (xhci->sp_ptrs[i])
				free(phys_to_virt(xhci->sp_ptrs[i]));
		}
	}
	free(xhci->sp_ptrs);
	free(xhci->dma_buffer);
	free(xhci->dcbaa);
	free(xhci->dev);
	free((void *)xhci->ev_ring_table);
	free((void *)xhci->er.ring);
	free((void *)xhci->cr.ring);
	free(xhci);
	free(controller);
}

static void
xhci_start(hci_t *controller)
{
	xhci_t *const xhci = XHCI_INST(controller);

	xhci->opreg->usbcmd |= USBCMD_RS;
	if (!xhci_handshake(&xhci->opreg->usbsts, USBSTS_HCH, 0, 1000000L))
		xhci_debug("Controller didn't start within 1s\n");
}

static void
xhci_stop(hci_t *controller)
{
	xhci_t *const xhci = XHCI_INST(controller);

	xhci->opreg->usbcmd &= ~USBCMD_RS;
	if (!xhci_handshake(&xhci->opreg->usbsts,
			    USBSTS_HCH, USBSTS_HCH, 1000000L))
		xhci_debug("Controller didn't halt within 1s\n");
}

static int
xhci_reset_endpoint(usbdev_t *const dev, endpoint_t *const ep)
{
	xhci_t *const xhci = XHCI_INST(dev->controller);
	const int slot_id = dev->address;
	const int ep_id = ep ? xhci_ep_id(ep) : 1;
	epctx_t *const epctx = xhci->dev[slot_id].ctx.ep[ep_id];

	xhci_debug("Resetting ID %d EP %d (ep state: %d)\n",
		   slot_id, ep_id, EC_GET(STATE, epctx));

	/* Run Reset Endpoint Command if the EP is in Halted state */
	if (EC_GET(STATE, epctx) == 2) {
		const int cc = xhci_cmd_reset_endpoint(xhci, slot_id, ep_id);
		if (cc != CC_SUCCESS) {
			xhci_debug("Reset Endpoint Command failed: %d\n", cc);
			return 1;
		}
	}

	/* Clear TT buffer for bulk and control endpoints behind a TT */
	const int hub = dev->hub;
	if (hub && dev->speed < HIGH_SPEED &&
			dev->controller->devices[hub]->speed == HIGH_SPEED)
		/* TODO */;

	/* Reset transfer ring if the endpoint is in the right state */
	const unsigned ep_state = EC_GET(STATE, epctx);
	if (ep_state == 3 || ep_state == 4) {
		transfer_ring_t *const tr =
				xhci->dev[slot_id].transfer_rings[ep_id];
		const int cc = xhci_cmd_set_tr_dq(xhci, slot_id, ep_id,
						  tr->ring, 1);
		if (cc != CC_SUCCESS) {
			xhci_debug("Set TR Dequeue Command failed: %d\n", cc);
			return 1;
		}
		xhci_init_cycle_ring(tr, TRANSFER_RING_SIZE);
	}

	xhci_debug("Finished resetting ID %d EP %d (ep state: %d)\n",
		   slot_id, ep_id, EC_GET(STATE, epctx));

	return 0;
}

static void
xhci_enqueue_trb(transfer_ring_t *const tr)
{
	const int chain = TRB_GET(CH, tr->cur);
	TRB_SET(C, tr->cur, tr->pcs);
	++tr->cur;

	while (TRB_GET(TT, tr->cur) == TRB_LINK) {
		xhci_spew("Handling LINK pointer\n");
		const int tc = TRB_GET(TC, tr->cur);
		TRB_SET(CH, tr->cur, chain);
		wmb();
		TRB_SET(C, tr->cur, tr->pcs);
		tr->cur = phys_to_virt(tr->cur->ptr_low);
		if (tc)
			tr->pcs ^= 1;
	}
}

static void
xhci_ring_doorbell(endpoint_t *const ep)
{
	/* Ensure all TRB changes are written to memory. */
	wmb();
	XHCI_INST(ep->dev->controller)->dbreg[ep->dev->address] =
		xhci_ep_id(ep);
}

static void
xhci_enqueue_td(transfer_ring_t *const tr, const int ep, const size_t mps,
		const int dalen, void *const data, const int dir)
{
	trb_t *trb = NULL;				/* cur TRB */
	u8 *cur_start = data;				/* cur data pointer */
	size_t length = dalen;				/* remaining bytes */
	size_t packets = (length + mps - 1) / mps;	/* remaining packets */
	size_t residue = 0;				/* residue from last TRB */
	size_t trb_count = 0;				/* TRBs added so far */

	while (length || !trb_count /* enqueue at least one */) {
		const size_t cur_end = ((size_t)cur_start + 0x10000) & ~0xffff;
		size_t cur_length = cur_end - (size_t)cur_start;
		if (length < cur_length) {
			cur_length = length;
			packets = 0;
			length = 0;
		} else if (!CONFIG(LP_USB_XHCI_MTK_QUIRK)) {
			packets -= (residue + cur_length) / mps;
			residue = (residue + cur_length) % mps;
			length -= cur_length;
		}

		trb = tr->cur;
		xhci_clear_trb(trb, tr->pcs);
		trb->ptr_low = virt_to_phys(cur_start);
		TRB_SET(TL, trb, cur_length);
		TRB_SET(TDS, trb, MIN(TRB_MAX_TD_SIZE, packets));
		TRB_SET(CH, trb, 1);

		if (length && CONFIG(LP_USB_XHCI_MTK_QUIRK)) {
			/*
			 * For MTK's xHCI controller, TDS defines a number of
			 * packets that remain to be transferred for a TD after
			 * processing all Max packets in all previous TRBs, that
			 * means don't include the current TRB's.
			 */
			packets -= (residue + cur_length) / mps;
			residue = (residue + cur_length) % mps;
			length -= cur_length;
		}

		/* Check for first, data stage TRB */
		if (!trb_count && ep == 1) {
			TRB_SET(DIR, trb, dir);
			TRB_SET(TT, trb, TRB_DATA_STAGE);
		} else {
			TRB_SET(TT, trb, TRB_NORMAL);
		}
		/*
		 * This is a workaround for Synopsys DWC3. If the ENT flag is
		 * not set for the Normal and Data Stage TRBs. We get Event TRB
		 * with length 0x20d from the controller when we enqueue a TRB
		 * for the IN endpoint with length 0x200.
		 */
		if (!length)
			TRB_SET(ENT, trb, 1);

		xhci_enqueue_trb(tr);

		cur_start += cur_length;
		++trb_count;
	}

	trb = tr->cur;
	xhci_clear_trb(trb, tr->pcs);
	trb->ptr_low = virt_to_phys(trb);	/* for easier debugging only */
	TRB_SET(TT, trb, TRB_EVENT_DATA);
	TRB_SET(IOC, trb, 1);

	xhci_enqueue_trb(tr);
}

static int
xhci_control(usbdev_t *const dev, const direction_t dir,
	     const int drlen, void *const devreq,
	     const int dalen, unsigned char *const src)
{
	unsigned char *data = src;
	xhci_t *const xhci = XHCI_INST(dev->controller);
	epctx_t *const epctx = xhci->dev[dev->address].ctx.ep0;
	transfer_ring_t *const tr = xhci->dev[dev->address].transfer_rings[1];

	const size_t off = (size_t)data & 0xffff;
	if ((off + dalen) > ((TRANSFER_RING_SIZE - 4) << 16)) {
		xhci_debug("Unsupported transfer size\n");
		return -1;
	}

	/* Reset endpoint if it's not running */
	const unsigned ep_state = EC_GET(STATE, epctx);
	if (ep_state > 1) {
		if (xhci_reset_endpoint(dev, NULL))
			return -1;
	}

	if (dalen && !dma_coherent(src)) {
		data = xhci->dma_buffer;
		if (dalen > DMA_SIZE) {
			xhci_debug("Control transfer too large: %d\n", dalen);
			return -1;
		}
		if (dir == OUT)
			memcpy(data, src, dalen);
	}

	/* Fill and enqueue setup TRB */
	trb_t *const setup = tr->cur;
	xhci_clear_trb(setup, tr->pcs);
	setup->ptr_low = ((u32 *)devreq)[0];
	setup->ptr_high = ((u32 *)devreq)[1];
	TRB_SET(TL, setup, 8);
	TRB_SET(TRT, setup, (dalen)
			? ((dir == OUT) ? TRB_TRT_OUT_DATA : TRB_TRT_IN_DATA)
			: TRB_TRT_NO_DATA);
	TRB_SET(TT, setup, TRB_SETUP_STAGE);
	TRB_SET(IDT, setup, 1);
	TRB_SET(IOC, setup, 1);
	xhci_enqueue_trb(tr);

	/* Fill and enqueue data TRBs (if any) */
	if (dalen) {
		const unsigned mps = EC_GET(MPS, epctx);
		const unsigned dt_dir = (dir == OUT) ? TRB_DIR_OUT : TRB_DIR_IN;
		xhci_enqueue_td(tr, 1, mps, dalen, data, dt_dir);
	}

	/* Fill status TRB */
	trb_t *const status = tr->cur;
	xhci_clear_trb(status, tr->pcs);
	TRB_SET(DIR, status, (dir == OUT) ? TRB_DIR_IN : TRB_DIR_OUT);
	TRB_SET(TT, status, TRB_STATUS_STAGE);
	TRB_SET(IOC, status, 1);
	xhci_enqueue_trb(tr);

	/* Ring doorbell for EP0 */
	xhci_ring_doorbell(&dev->endpoints[0]);

	/* Wait for transfer events */
	int i, transferred = 0;
	const int n_stages = 2 + !!dalen;
	for (i = 0; i < n_stages; ++i) {
		const int ret = xhci_wait_for_transfer(xhci, dev->address, 1);
		transferred += ret;
		if (ret < 0) {
			if (ret == TIMEOUT) {
				xhci_debug("Stopping ID %d EP 1\n",
					   dev->address);
				xhci_cmd_stop_endpoint(xhci, dev->address, 1);
			}
			xhci_debug("Stage %d/%d failed: %d\n"
				   "  trb ring:   @%p\n"
				   "  setup trb:  @%p\n"
				   "  status trb: @%p\n"
				   "  ep state:   %d -> %d\n"
				   "  usbsts:     0x%08"PRIx32"\n",
				   i, n_stages, ret,
				   tr->ring, setup, status,
				   ep_state, EC_GET(STATE, epctx),
				   xhci->opreg->usbsts);
			return ret;
		}
	}

	if (dir == IN && data != src)
		memcpy(src, data, transferred);
	return transferred;
}

/* finalize == 1: if data is of packet aligned size, add a zero length packet */
static int
xhci_bulk(endpoint_t *const ep, const int size, u8 *const src,
	  const int finalize)
{
	/* finalize: Hopefully the xHCI controller always does this.
		     We have no control over the packets. */

	u8 *data = src;
	xhci_t *const xhci = XHCI_INST(ep->dev->controller);
	const int slot_id = ep->dev->address;
	const int ep_id = xhci_ep_id(ep);
	epctx_t *const epctx = xhci->dev[slot_id].ctx.ep[ep_id];
	transfer_ring_t *const tr = xhci->dev[slot_id].transfer_rings[ep_id];

	const size_t off = (size_t)data & 0xffff;
	if ((off + size) > ((TRANSFER_RING_SIZE - 2) << 16)) {
		xhci_debug("Unsupported transfer size\n");
		return -1;
	}

	if (!dma_coherent(src)) {
		data = xhci->dma_buffer;
		if (size > DMA_SIZE) {
			xhci_debug("Bulk transfer too large: %d\n", size);
			return -1;
		}
		if (ep->direction == OUT)
			memcpy(data, src, size);
	}

	/* Reset endpoint if it's not running */
	const unsigned ep_state = EC_GET(STATE, epctx);
	if (ep_state > 1) {
		if (xhci_reset_endpoint(ep->dev, ep))
			return -1;
	}

	/* Enqueue transfer and ring doorbell */
	const unsigned mps = EC_GET(MPS, epctx);
	const unsigned dir = (ep->direction == OUT) ? TRB_DIR_OUT : TRB_DIR_IN;
	xhci_enqueue_td(tr, ep_id, mps, size, data, dir);
	xhci_ring_doorbell(ep);

	/* Wait for transfer event */
	const int ret = xhci_wait_for_transfer(xhci, ep->dev->address, ep_id);
	if (ret < 0) {
		if (ret == TIMEOUT) {
			xhci_debug("Stopping ID %d EP %d\n",
				   ep->dev->address, ep_id);
			xhci_cmd_stop_endpoint(xhci, ep->dev->address, ep_id);
		}
		xhci_debug("Bulk transfer failed: %d\n"
			   "  ep state: %d -> %d\n"
			   "  usbsts:   0x%08"PRIx32"\n",
			   ret, ep_state,
			   EC_GET(STATE, epctx),
			   xhci->opreg->usbsts);
		return ret;
	}

	if (ep->direction == IN && data != src)
		memcpy(src, data, ret);
	return ret;
}

static trb_t *
xhci_next_trb(trb_t *cur, int *const pcs)
{
	++cur;
	while (TRB_GET(TT, cur) == TRB_LINK) {
		if (pcs && TRB_GET(TC, cur))
			*pcs ^= 1;
		cur = phys_to_virt(cur->ptr_low);
	}
	return cur;
}

/* create and hook-up an intr queue into device schedule */
static void *
xhci_create_intr_queue(endpoint_t *const ep,
		       const int reqsize, const int reqcount,
		       const int reqtiming)
{
	/* reqtiming: We ignore it and use the interval from the
		      endpoint descriptor configured earlier. */

	xhci_t *const xhci = XHCI_INST(ep->dev->controller);
	const int slot_id = ep->dev->address;
	const int ep_id = xhci_ep_id(ep);
	transfer_ring_t *const tr = xhci->dev[slot_id].transfer_rings[ep_id];

	if (reqcount > (TRANSFER_RING_SIZE - 2)) {
		xhci_debug("reqcount is too high, at most %d supported\n",
			   TRANSFER_RING_SIZE - 2);
		return NULL;
	}
	if (reqsize > 0x10000) {
		xhci_debug("reqsize is too large, at most 64KiB supported\n");
		return NULL;
	}
	if (xhci->dev[slot_id].interrupt_queues[ep_id]) {
		xhci_debug("Only one interrupt queue per endpoint supported\n");
		return NULL;
	}

	/* Allocate intrq structure and reqdata chunks */

	intrq_t *const intrq = malloc(sizeof(*intrq));
	if (!intrq) {
		xhci_debug("Out of memory\n");
		return NULL;
	}

	int i;
	int pcs = tr->pcs;
	trb_t *cur = tr->cur;
	for (i = 0; i < reqcount; ++i) {
		if (TRB_GET(C, cur) == pcs) {
			xhci_debug("Not enough empty TRBs\n");
			goto _free_return;
		}
		void *const reqdata = xhci_align(1, reqsize);
		if (!reqdata) {
			xhci_debug("Out of memory\n");
			goto _free_return;
		}
		xhci_clear_trb(cur, pcs);
		cur->ptr_low = virt_to_phys(reqdata);
		cur->ptr_high = 0;
		TRB_SET(TL,	cur, reqsize);
		TRB_SET(TT,	cur, TRB_NORMAL);
		TRB_SET(ISP,	cur, 1);
		TRB_SET(IOC,	cur, 1);

		cur = xhci_next_trb(cur, &pcs);
	}

	intrq->size	= reqsize;
	intrq->count	= reqcount;
	intrq->next	= tr->cur;
	intrq->ready	= NULL;
	intrq->ep	= ep;
	xhci->dev[slot_id].interrupt_queues[ep_id] = intrq;

	/* Now enqueue all the prepared TRBs but the last
	   and ring the doorbell. */
	for (i = 0; i < (reqcount - 1); ++i)
		xhci_enqueue_trb(tr);
	xhci_ring_doorbell(ep);

	return intrq;

_free_return:
	cur = tr->cur;
	for (--i; i >= 0; --i) {
		free(phys_to_virt(cur->ptr_low));
		cur = xhci_next_trb(cur, NULL);
	}
	free(intrq);
	return NULL;
}

/* remove queue from device schedule, dropping all data that came in */
static void
xhci_destroy_intr_queue(endpoint_t *const ep, void *const q)
{
	xhci_t *const xhci = XHCI_INST(ep->dev->controller);
	const int slot_id = ep->dev->address;
	const int ep_id = xhci_ep_id(ep);
	transfer_ring_t *const tr = xhci->dev[slot_id].transfer_rings[ep_id];

	intrq_t *const intrq = (intrq_t *)q;

	/* Make sure the endpoint is stopped */
	if (EC_GET(STATE, xhci->dev[slot_id].ctx.ep[ep_id]) == 1) {
		const int cc = xhci_cmd_stop_endpoint(xhci, slot_id, ep_id);
		if (cc != CC_SUCCESS)
			xhci_debug("Warning: Failed to stop endpoint\n");
	}

	/* Process all remaining transfer events */
	xhci_handle_events(xhci);

	/* Free all pending transfers and the interrupt queue structure */
	int i;
	for (i = 0; i < intrq->count; ++i) {
		free(phys_to_virt(intrq->next->ptr_low));
		intrq->next = xhci_next_trb(intrq->next, NULL);
	}
	xhci->dev[slot_id].interrupt_queues[ep_id] = NULL;
	free((void *)intrq);

	/* Reset the controller's dequeue pointer and reinitialize the ring */
	xhci_cmd_set_tr_dq(xhci, slot_id, ep_id, tr->ring, 1);
	xhci_init_cycle_ring(tr, TRANSFER_RING_SIZE);
}

/* read one intr-packet from queue, if available. extend the queue for new input.
   return NULL if nothing new available.
   Recommended use: while (data=poll_intr_queue(q)) process(data);
 */
static u8 *
xhci_poll_intr_queue(void *const q)
{
	if (!q)
		return NULL;

	intrq_t *const intrq = (intrq_t *)q;
	endpoint_t *const ep = intrq->ep;
	xhci_t *const xhci = XHCI_INST(ep->dev->controller);

	/* TODO: Reset interrupt queue if it gets halted? */

	xhci_handle_events(xhci);

	u8 *reqdata = NULL;
	while (!reqdata && intrq->ready) {
		const int ep_id = xhci_ep_id(ep);
		transfer_ring_t *const tr =
			xhci->dev[ep->dev->address].transfer_rings[ep_id];

		/* Fetch the request's buffer */
		reqdata = phys_to_virt(intrq->next->ptr_low);

		/* Enqueue the last (spare) TRB and ring doorbell */
		xhci_enqueue_trb(tr);
		xhci_ring_doorbell(ep);

		/* Reuse the current buffer for the next spare TRB */
		xhci_clear_trb(tr->cur, tr->pcs);
		tr->cur->ptr_low = virt_to_phys(reqdata);
		tr->cur->ptr_high = 0;
		TRB_SET(TL,	tr->cur, intrq->size);
		TRB_SET(TT,	tr->cur, TRB_NORMAL);
		TRB_SET(ISP,	tr->cur, 1);
		TRB_SET(IOC,	tr->cur, 1);

		/* Check if anything was transferred */
		const size_t read = TRB_GET(TL, intrq->next);
		if (!read)
			reqdata = NULL;
		else if (read < intrq->size)
			/* At least zero it, poll interface is rather limited */
			memset(reqdata + read, 0x00, intrq->size - read);

		/* Advance the interrupt queue */
		if (intrq->ready == intrq->next)
			/* This was last TRB being ready */
			intrq->ready = NULL;
		intrq->next = xhci_next_trb(intrq->next, NULL);
	}

	return reqdata;
}
