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
#include "xhci.h"
#include "xhci_private.h"

static void xhci_start (hci_t *controller);
static void xhci_stop (hci_t *controller);
static void xhci_reset (hci_t *controller);
static void xhci_shutdown (hci_t *controller);
static int xhci_bulk (endpoint_t *ep, int size, u8 *data, int finalize);
static int xhci_control (usbdev_t *dev, direction_t dir, int drlen, void *devreq,
			 int dalen, u8 *data);
static void* xhci_create_intr_queue (endpoint_t *ep, int reqsize, int reqcount, int reqtiming);
static void xhci_destroy_intr_queue (endpoint_t *ep, void *queue);
static u8* xhci_poll_intr_queue (void *queue);

static void
xhci_reset (hci_t *controller)
{
}

hci_t *
xhci_init (pcidev_t addr)
{
	int i;

	hci_t *controller = new_controller ();

	if (!controller)
		usb_fatal("Could not create USB controller instance.\n");

	controller->instance = malloc (sizeof (xhci_t));
	if(!controller->instance)
		usb_fatal("Not enough memory creating USB controller instance.\n");

	controller->start = xhci_start;
	controller->stop = xhci_stop;
	controller->reset = xhci_reset;
	controller->shutdown = xhci_shutdown;
	controller->bulk = xhci_bulk;
	controller->control = xhci_control;
	controller->create_intr_queue = xhci_create_intr_queue;
	controller->destroy_intr_queue = xhci_destroy_intr_queue;
	controller->poll_intr_queue = xhci_poll_intr_queue;
	for (i = 0; i < 128; i++) {
		controller->devices[i] = 0;
	}
	init_device_entry (controller, 0);
	XHCI_INST (controller)->roothub = controller->devices[0];

	controller->bus_address = addr;
	controller->reg_base = (u32)phys_to_virt(pci_read_config32 (controller->bus_address, 0x10) & ~0xf);
	//controller->reg_base = pci_read_config32 (controller->bus_address, 0x14) & ~0xf;
	if (pci_read_config32 (controller->bus_address, 0x14) > 0) {
		usb_fatal("We don't do 64bit addressing.\n");
	}
	debug("regbase: %lx\n", controller->reg_base);

	XHCI_INST (controller)->capreg = (void*)controller->reg_base;
	XHCI_INST (controller)->opreg = (void*)(controller->reg_base + XHCI_INST (controller)->capreg->caplength);
	XHCI_INST (controller)->hcrreg = (void*)(controller->reg_base + XHCI_INST (controller)->capreg->rtsoff);
	XHCI_INST (controller)->dbreg = (void*)(controller->reg_base + XHCI_INST (controller)->capreg->dboff);
	debug("caplen: %lx\nrtsoff: %lx\ndboff: %lx\n", XHCI_INST (controller)->capreg->caplength, XHCI_INST (controller)->capreg->rtsoff, XHCI_INST (controller)->capreg->dboff);
	debug("caplength: %x\n", XHCI_INST (controller)->capreg->caplength);
	debug("hciversion: %x.%x\n", XHCI_INST (controller)->capreg->hciver_hi, XHCI_INST (controller)->capreg->hciver_lo);
	if ((XHCI_INST (controller)->capreg->hciversion < 0x96) || (XHCI_INST (controller)->capreg->hciversion > 0x100)) {
		usb_fatal("Unsupported xHCI version\n");
	}
	debug("maxslots: %x\n", XHCI_INST (controller)->capreg->MaxSlots);
	debug("maxports: %x\n", XHCI_INST (controller)->capreg->MaxPorts);
	int pagesize = XHCI_INST (controller)->opreg->pagesize << 12;
	debug("pagesize: %x\n", pagesize);

	XHCI_INST (controller)->dcbaa = memalign(64, (XHCI_INST (controller)->capreg->MaxSlots+1)*sizeof(devctxp_t));
	memset((void*)XHCI_INST (controller)->dcbaa, 0, (XHCI_INST (controller)->capreg->MaxSlots+1)*sizeof(devctxp_t));

	debug("max scratchpad bufs: %x\n", XHCI_INST (controller)->capreg->Max_Scratchpad_Bufs);
	if (XHCI_INST (controller)->capreg->Max_Scratchpad_Bufs > 0) {
		XHCI_INST (controller)->dcbaa->ptr = memalign(64, XHCI_INST (controller)->capreg->Max_Scratchpad_Bufs * 8);
	}

	XHCI_INST (controller)->opreg->dcbaap_lo = virt_to_phys(XHCI_INST (controller)->dcbaa);
	XHCI_INST (controller)->opreg->dcbaap_hi = 0;

	printf("waiting for controller to be ready - ");
	while ((XHCI_INST (controller)->opreg->usbsts & USBSTS_CNR) != 0) mdelay(1);
	printf("ok.\n");

	debug("ERST Max: %lx -> %lx entries\n", XHCI_INST (controller)->capreg->ERST_Max, 1<<(XHCI_INST (controller)->capreg->ERST_Max));

	// enable all available slots
	XHCI_INST (controller)->opreg->config = XHCI_INST (controller)->capreg->MaxSlots & CONFIG_MASK_MaxSlotsEn;

	XHCI_INST (controller)->cmd_ring = memalign(64, 16*sizeof(trb_t)); /* TODO: make sure not to cross 64k page boundary */
	memset((void*)XHCI_INST (controller)->cmd_ring, 0, 16*sizeof(trb_t));

	XHCI_INST (controller)->ev_ring = memalign(64, 16*sizeof(trb_t)); /* TODO: make sure not to cross 64k page boundary */
	memset((void*)XHCI_INST (controller)->ev_ring, 0, 16*sizeof(trb_t));

	XHCI_INST (controller)->ev_ring_table = memalign(64, sizeof(erst_entry_t));
	memset((void*)XHCI_INST (controller)->ev_ring_table, 0, sizeof(erst_entry_t));
	XHCI_INST (controller)->ev_ring_table[0].seg_base_lo = virt_to_phys(XHCI_INST (controller)->ev_ring);
	XHCI_INST (controller)->ev_ring_table[0].seg_base_hi = 0;
	XHCI_INST (controller)->ev_ring_table[0].seg_size = 16;

	// init command ring
	XHCI_INST (controller)->opreg->crcr_lo = virt_to_phys(XHCI_INST (controller)->cmd_ring) | CRCR_RCS;
	XHCI_INST (controller)->opreg->crcr_hi = 0;
	XHCI_INST (controller)->cmd_ccs = 1;
	XHCI_INST (controller)->ev_ccs = 1;

	// init primary interrupter
	XHCI_INST (controller)->hcrreg->intrrs[0].erstsz = 1;
	XHCI_INST (controller)->hcrreg->intrrs[0].erdp_lo = virt_to_phys(XHCI_INST (controller)->ev_ring);
	XHCI_INST (controller)->hcrreg->intrrs[0].erdp_hi = 0;
	XHCI_INST (controller)->hcrreg->intrrs[0].erstba_lo = virt_to_phys(XHCI_INST (controller)->ev_ring_table);
	XHCI_INST (controller)->hcrreg->intrrs[0].erstba_hi = 0;

	XHCI_INST (controller)->opreg->usbcmd |= USBCMD_RS; /* start USB controller */
	XHCI_INST (controller)->dbreg[0] = 0; // and tell controller to consume commands

	/* TODO: TEST */
	// setup noop command
	trb_t *cmd = &XHCI_INST (controller)->cmd_ring[0];
	((u32*)cmd)[3] = 1-XHCI_INST (controller)->cmd_ccs; // disable command descriptor
	((u32*)cmd)[0] = 0;
	((u32*)cmd)[1] = 0;
	((u32*)cmd)[2] = 0;
	cmd->cmd_No_Op.TRB_Type = TRB_CMD_NOOP;

	// ring the HC doorbell
	debug("Posting command at %lx\n", virt_to_phys(cmd));
	cmd->cmd_No_Op.C = XHCI_INST (controller)->cmd_ccs; // enable command
	XHCI_INST (controller)->dbreg[0] = 0; // and tell controller to consume commands

	// wait for result in event ring
	trb_t *ev = &XHCI_INST (controller)->ev_ring[0];
	trb_t *ev1 = &XHCI_INST (controller)->ev_ring[1];
	while (ev->event_cmd_cmpl.C != XHCI_INST (controller)->ev_ccs) {
		debug("CRCR: %lx, USBSTS: %lx\n",  XHCI_INST (controller)->opreg->crcr_lo, XHCI_INST (controller)->opreg->usbsts);
		debug("ev0.C %x, ev1.C %x\n", ev->event_cmd_cmpl.C, ev1->event_cmd_cmpl.C);
		mdelay(100);
	}
	debug("command ring is %srunning\n", (XHCI_INST (controller)->opreg->crcr_lo & CRCR_CRR)?"":"not ");
	switch (ev->event_cmd_cmpl.TRB_Type) {
		case TRB_EV_CMD_CMPL:
			debug("Completed command TRB at %lx. Code: %d\n",
				ev->event_cmd_cmpl.Cmd_TRB_Pointer_lo, ev->event_cmd_cmpl.Completion_Code);
			break;
		case TRB_EV_PORTSC:
			debug("Port Status Change Event. Completion Code: %d\n Port: %d. Ignoring.\n",
				ev->event_cmd_cmpl.Completion_Code, ev->event_portsc.Port);
			// we ignore the event as we look for the PORTSC registers instead, at a time when it suits _us_
			break;
		default:
			debug("Unknown event: %d, Completion Code: %d\n", ev->event_cmd_cmpl.TRB_Type, ev->event_cmd_cmpl.Completion_Code);
			break;
	}
	debug("CRCR: %lx, USBSTS: %lx\n",  XHCI_INST (controller)->opreg->crcr_lo, XHCI_INST (controller)->opreg->usbsts);
	debug("ev0.C %x, ev1.C %x, ev1.CC %d\n", ev->event_cmd_cmpl.C, ev1->event_cmd_cmpl.C, ev1->event_cmd_cmpl.Completion_Code);

	controller->devices[0]->controller = controller;
	controller->devices[0]->init = xhci_rh_init;
	controller->devices[0]->init (controller->devices[0]);

	xhci_reset (controller);
	return controller;
}

static void
xhci_shutdown (hci_t *controller)
{
	if (controller == 0)
		return;
	detach_controller (controller);
	XHCI_INST (controller)->roothub->destroy (XHCI_INST (controller)->
						  roothub);
	/* TODO: stop hardware, kill data structures */
	free (XHCI_INST (controller));
	free (controller);
}

static void
xhci_start (hci_t *controller)
{
}

static void
xhci_stop (hci_t *controller)
{
}

static int
xhci_control (usbdev_t *dev, direction_t dir, int drlen, void *devreq, int dalen,
	      unsigned char *data)
{
	return 1;
}

/* finalize == 1: if data is of packet aligned size, add a zero length packet */
static int
xhci_bulk (endpoint_t *ep, int size, u8 *data, int finalize)
{
	int maxpsize = ep->maxpacketsize;
	if (maxpsize == 0)
		usb_fatal ("MaxPacketSize == 0!!!");
	return 1;
}

/* create and hook-up an intr queue into device schedule */
static void*
xhci_create_intr_queue (endpoint_t *ep, int reqsize, int reqcount, int reqtiming)
{
	return NULL;
}

/* remove queue from device schedule, dropping all data that came in */
static void
xhci_destroy_intr_queue (endpoint_t *ep, void *q_)
{
	//free(q);
}

/* read one intr-packet from queue, if available. extend the queue for new input.
   return NULL if nothing new available.
   Recommended use: while (data=poll_intr_queue(q)) process(data);
 */
static u8*
xhci_poll_intr_queue (void *q_)
{
	return NULL;
}
