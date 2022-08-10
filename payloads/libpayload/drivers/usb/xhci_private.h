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

#ifndef __XHCI_PRIVATE_H
#define __XHCI_PRIVATE_H

//#define USB_DEBUG
#include <usb/usb.h>
#include <arch/barrier.h>
#include <kconfig.h>

//#define XHCI_DUMPS
#define xhci_debug(fmt, args...) usb_debug("%s: " fmt, __func__, ## args)
#ifdef XHCI_SPEW_DEBUG
# define xhci_spew(fmt, args...) xhci_debug(fmt, ##args)
#else
# define xhci_spew(fmt, args...) do {} while (0)
#endif

#define MASK(startbit, lenbit) (((1<<(lenbit))-1)<<(startbit))

/* Make these high enough to not collide with negative XHCI CCs */
#define TIMEOUT			-65
#define CONTROLLER_ERROR	-66
#define COMMUNICATION_ERROR	-67
#define OUT_OF_MEMORY		-68
#define DRIVER_ERROR		-69

#define CC_SUCCESS			 1
#define CC_TRB_ERROR			 5
#define CC_STALL_ERROR			 6
#define CC_RESOURCE_ERROR		 7
#define CC_BANDWIDTH_ERROR		 8
#define CC_NO_SLOTS_AVAILABLE		 9
#define CC_SHORT_PACKET			13
#define CC_EVENT_RING_FULL_ERROR	21
#define CC_COMMAND_RING_STOPPED		24
#define CC_COMMAND_ABORTED		25
#define CC_STOPPED			26
#define CC_STOPPED_LENGTH_INVALID	27

enum {
	TRB_NORMAL = 1,
	TRB_SETUP_STAGE = 2, TRB_DATA_STAGE = 3, TRB_STATUS_STAGE = 4,
	TRB_LINK = 6, TRB_EVENT_DATA = 7,
	TRB_CMD_ENABLE_SLOT = 9, TRB_CMD_DISABLE_SLOT = 10, TRB_CMD_ADDRESS_DEV = 11,
	TRB_CMD_CONFIGURE_EP = 12, TRB_CMD_EVAL_CTX = 13, TRB_CMD_RESET_EP = 14,
	TRB_CMD_STOP_EP = 15, TRB_CMD_SET_TR_DQ = 16, TRB_CMD_NOOP = 23,
	TRB_EV_TRANSFER = 32, TRB_EV_CMD_CMPL = 33, TRB_EV_PORTSC = 34, TRB_EV_HOST = 37,
};
enum { TRB_TRT_NO_DATA = 0, TRB_TRT_OUT_DATA = 2, TRB_TRT_IN_DATA = 3 };
enum { TRB_DIR_OUT = 0, TRB_DIR_IN = 1 };

#define TRB_PORT_FIELD		ptr_low
#define TRB_PORT_START		24
#define TRB_PORT_LEN		8
#define TRB_TL_FIELD		status		/* TL - Transfer Length */
#define TRB_TL_START		0
#define TRB_TL_LEN		17
#define TRB_EVTL_FIELD		status		/* EVTL - (Event TRB) Transfer Length */
#define TRB_EVTL_START		0
#define TRB_EVTL_LEN		24
#define TRB_TDS_FIELD		status		/* TDS - TD Size */
#define TRB_TDS_START		17
#define TRB_TDS_LEN		5
#define TRB_CC_FIELD		status		/* CC - Completion Code */
#define TRB_CC_START		24
#define TRB_CC_LEN		8
#define TRB_C_FIELD		control		/* C - Cycle Bit */
#define TRB_C_START		0
#define TRB_C_LEN		1
#define TRB_TC_FIELD		control		/* TC - Toggle Cycle */
#define TRB_TC_START		1
#define TRB_TC_LEN		1
#define TRB_ENT_FIELD		control		/* ENT - Evaluate Next TRB */
#define TRB_ENT_START		1
#define TRB_ENT_LEN		1
#define TRB_ISP_FIELD		control		/* ISP - Interrupt-on Short Packet */
#define TRB_ISP_START		2
#define TRB_ISP_LEN		1
#define TRB_CH_FIELD		control		/* CH - Chain Bit */
#define TRB_CH_START		4
#define TRB_CH_LEN		1
#define TRB_IOC_FIELD		control		/* IOC - Interrupt On Completion */
#define TRB_IOC_START		5
#define TRB_IOC_LEN		1
#define TRB_IDT_FIELD		control		/* IDT - Immediate Data */
#define TRB_IDT_START		6
#define TRB_IDT_LEN		1
#define TRB_DC_FIELD		control		/* DC - Deconfigure */
#define TRB_DC_START		9
#define TRB_DC_LEN		1
#define TRB_TT_FIELD		control		/* TT - TRB Type */
#define TRB_TT_START		10
#define TRB_TT_LEN		6
#define TRB_TRT_FIELD		control		/* TRT - Transfer Type */
#define TRB_TRT_START		16
#define TRB_TRT_LEN		2
#define TRB_DIR_FIELD		control		/* DIR - Direction */
#define TRB_DIR_START		16
#define TRB_DIR_LEN		1
#define TRB_EP_FIELD		control		/* EP - Endpoint ID */
#define TRB_EP_START		16
#define TRB_EP_LEN		5
#define TRB_ID_FIELD		control		/* ID - Slot ID */
#define TRB_ID_START		24
#define TRB_ID_LEN		8
#define TRB_MASK(tok)		MASK(TRB_##tok##_START, TRB_##tok##_LEN)
#define TRB_GET(tok, trb)	(((trb)->TRB_##tok##_FIELD & TRB_MASK(tok)) \
				 >> TRB_##tok##_START)
#define TRB_SET(tok, trb, to)	(trb)->TRB_##tok##_FIELD = \
				(((trb)->TRB_##tok##_FIELD & ~TRB_MASK(tok)) | \
				 (((to) << TRB_##tok##_START) & TRB_MASK(tok)))
#define TRB_DUMP(tok, trb)	usb_debug(" "#tok"\t0x%04"PRIx32"\n", TRB_GET(tok, trb))

#define TRB_CYCLE		(1 << 0)
typedef volatile struct trb {
	u32 ptr_low;
	u32 ptr_high;
	u32 status;
	u32 control;
} trb_t;

#define TRB_MAX_TD_SIZE	0x1F			/* bits 21:17 of TD Size in TRB */

#define EVENT_RING_SIZE 64
typedef struct {
	trb_t *ring;
	trb_t *cur;
	trb_t *last;
	u8 ccs;
	u8 adv;
} event_ring_t;

/* Never raise this above 256 to prevent transfer event length overflow! */
#define TRANSFER_RING_SIZE 32
typedef struct {
	trb_t *ring;
	trb_t *cur;
	u8 pcs;
} __packed transfer_ring_t;

#define COMMAND_RING_SIZE 4
typedef transfer_ring_t command_ring_t;

#define SC_ROUTE_FIELD		f1		/* ROUTE - Route String */
#define SC_ROUTE_START		0
#define SC_ROUTE_LEN		20
#define SC_SPEED1_FIELD		f1		/* SPEED - Port speed plus one (compared to usb_speed enum) */
#define SC_SPEED1_START		20
#define SC_SPEED1_LEN		4
#define SC_MTT_FIELD		f1		/* MTT - Multi Transaction Translator */
#define SC_MTT_START		25
#define SC_MTT_LEN		1
#define SC_HUB_FIELD		f1		/* HUB - Is this a hub? */
#define SC_HUB_START		26
#define SC_HUB_LEN		1
#define SC_CTXENT_FIELD		f1		/* CTXENT - Context Entries (number of following ep contexts) */
#define SC_CTXENT_START		27
#define SC_CTXENT_LEN		5
#define SC_RHPORT_FIELD		f2		/* RHPORT - Root Hub Port Number */
#define SC_RHPORT_START		16
#define SC_RHPORT_LEN		8
#define SC_NPORTS_FIELD		f2		/* NPORTS - Number of Ports */
#define SC_NPORTS_START		24
#define SC_NPORTS_LEN		8
#define SC_TTID_FIELD		f3		/* TTID - TT Hub Slot ID */
#define SC_TTID_START		0
#define SC_TTID_LEN		8
#define SC_TTPORT_FIELD		f3		/* TTPORT - TT Port Number */
#define SC_TTPORT_START		8
#define SC_TTPORT_LEN		8
#define SC_TTT_FIELD		f3		/* TTT - TT Think Time */
#define SC_TTT_START		16
#define SC_TTT_LEN		2
#define SC_UADDR_FIELD		f4		/* UADDR - USB Device Address */
#define SC_UADDR_START		0
#define SC_UADDR_LEN		8
#define SC_STATE_FIELD		f4		/* STATE - Slot State */
#define SC_STATE_START		27
#define SC_STATE_LEN		5
#define SC_MASK(tok)		MASK(SC_##tok##_START, SC_##tok##_LEN)
#define SC_GET(tok, sc)		(((sc)->SC_##tok##_FIELD & SC_MASK(tok)) \
				 >> SC_##tok##_START)
#define SC_SET(tok, sc, to)	(sc)->SC_##tok##_FIELD = \
				(((sc)->SC_##tok##_FIELD & ~SC_MASK(tok)) | \
				 (((to) << SC_##tok##_START) & SC_MASK(tok)))
#define SC_DUMP(tok, sc)	usb_debug(" "#tok"\t0x%04"PRIx32"\n", SC_GET(tok, sc))
typedef volatile struct slotctx {
	u32 f1;
	u32 f2;
	u32 f3;
	u32 f4;
	u32 rsvd[4];
} slotctx_t;

#define EC_STATE_FIELD		f1		/* STATE - Endpoint State */
#define EC_STATE_START		0
#define EC_STATE_LEN		3
#define EC_INTVAL_FIELD		f1		/* INTVAL - Interval */
#define EC_INTVAL_START		16
#define EC_INTVAL_LEN		8
#define EC_CERR_FIELD		f2		/* CERR - Error Count */
#define EC_CERR_START		1
#define EC_CERR_LEN		2
#define EC_TYPE_FIELD		f2		/* TYPE - EP Type */
#define EC_TYPE_START		3
#define EC_TYPE_LEN		3
#define EC_MBS_FIELD		f2		/* MBS - Max Burst Size */
#define EC_MBS_START		8
#define EC_MBS_LEN		8
#define EC_MPS_FIELD		f2		/* MPS - Max Packet Size */
#define EC_MPS_START		16
#define EC_MPS_LEN		16
#define EC_DCS_FIELD		tr_dq_low	/* DCS - Dequeue Cycle State */
#define EC_DCS_START		0
#define EC_DCS_LEN		1
#define EC_AVRTRB_FIELD		f5		/* AVRTRB - Average TRB Length */
#define EC_AVRTRB_START		0
#define EC_AVRTRB_LEN		16
#define EC_MXESIT_FIELD		f5		/* MXESIT - Max ESIT Payload */
#define EC_MXESIT_START		16
#define EC_MXESIT_LEN		16
#define EC_BPKTS_FIELD		rsvd[0]		/* BPKTS - packets tx in scheduled uframe */
#define EC_BPKTS_START		0
#define EC_BPKTS_LEN		6
#define EC_BBM_FIELD		rsvd[0]		/* BBM - burst mode for scheduling */
#define EC_BBM_START		11
#define EC_BBM_LEN		1

#define EC_MASK(tok)		MASK(EC_##tok##_START, EC_##tok##_LEN)
#define EC_GET(tok, ec)		(((ec)->EC_##tok##_FIELD & EC_MASK(tok)) \
				 >> EC_##tok##_START)
#define EC_SET(tok, ec, to)	(ec)->EC_##tok##_FIELD = \
				(((ec)->EC_##tok##_FIELD & ~EC_MASK(tok)) | \
				 (((to) << EC_##tok##_START) & EC_MASK(tok)))
#define EC_DUMP(tok, ec)	usb_debug(" "#tok"\t0x%04"PRIx32"\n", EC_GET(tok, ec))
enum { EP_ISOC_OUT = 1, EP_BULK_OUT = 2, EP_INTR_OUT = 3,
	EP_CONTROL = 4, EP_ISOC_IN = 5, EP_BULK_IN = 6, EP_INTR_IN = 7 };
typedef volatile struct epctx {
	u32 f1;
	u32 f2;
	u32 tr_dq_low;
	u32 tr_dq_high;
	u32 f5;
	u32 rsvd[3];
} epctx_t;

#define NUM_EPS 32

typedef union devctx {
	/* set of pointers, so we can dynamically adjust Slot/EP context size */
	struct {
		union {
			slotctx_t *slot;
			void *raw;	/* Pointer to the whole dev context. */
		};
		epctx_t *ep0;
		epctx_t *eps1_30[NUM_EPS - 2];
	};
	epctx_t *ep[NUM_EPS];	/* At index 0 it's actually the slotctx,
					we have it like that so we can use
					the ep_id directly as index. */
} devctx_t;

typedef struct inputctx {
	union {		    /* The drop flags are located at the start of the */
		u32 *drop;  /* structure, so a pointer to them is equivalent */
		void *raw;  /* to a pointer to the whole (raw) input context. */
	};
	u32 *add;
	devctx_t dev;
} inputctx_t;

typedef struct intrq {
	size_t size;	/* Size of each transfer */
	size_t count;	/* The number of TRBs to fill at once */
	trb_t *next;	/* The next TRB expected to be processed by the controller */
	trb_t *ready;	/* The last TRB in the transfer ring processed by the controller */
	endpoint_t *ep;
} intrq_t;

typedef struct devinfo {
	devctx_t ctx;
	transfer_ring_t *transfer_rings[NUM_EPS];
	intrq_t *interrupt_queues[NUM_EPS];
} devinfo_t;

typedef struct erst_entry {
	u32 seg_base_lo;
	u32 seg_base_hi;
	u32 seg_size;
	u32 rsvd;
} erst_entry_t;

#define CAP_CAPLEN_FIELD		hciparams
#define CAP_CAPLEN_START		0
#define CAP_CAPLEN_LEN			8
#define CAP_CAPVER_FIELD		hciparams
#define CAP_CAPVER_START		16
#define CAP_CAPVER_LEN			16
#define CAP_CAPVER_HI_FIELD		hciparams
#define CAP_CAPVER_HI_START		24
#define CAP_CAPVER_HI_LEN		8
#define CAP_CAPVER_LO_FIELD		hciparams
#define CAP_CAPVER_LO_START		16
#define CAP_CAPVER_LO_LEN		8
#define CAP_MAXSLOTS_FIELD		hcsparams1
#define CAP_MAXSLOTS_START		0
#define CAP_MAXSLOTS_LEN		7
#define CAP_MAXINTRS_FIELD		hcsparams1
#define CAP_MAXINTRS_START		7
#define CAP_MAXINTRS_LEN		11
#define CAP_MAXPORTS_FIELD		hcsparams1
#define CAP_MAXPORTS_START		24
#define CAP_MAXPORTS_LEN		8
#define CAP_IST_FIELD			hcsparams2
#define CAP_IST_START			0
#define CAP_IST_LEN			4
#define CAP_ERST_MAX_FIELD		hcsparams2
#define CAP_ERST_MAX_START		4
#define CAP_ERST_MAX_LEN		4
#define CAP_MAX_SCRATCH_BUFS_HI_FIELD	hcsparams2
#define CAP_MAX_SCRATCH_BUFS_HI_START	21
#define CAP_MAX_SCRATCH_BUFS_HI_LEN	5
#define CAP_MAX_SCRATCH_BUFS_LO_FIELD	hcsparams2
#define CAP_MAX_SCRATCH_BUFS_LO_START	27
#define CAP_MAX_SCRATCH_BUFS_LO_LEN	5
#define CAP_U1_LATENCY_FIELD		hcsparams3
#define CAP_U1_LATENCY_START		0
#define CAP_U1_LATENCY_LEN		8
#define CAP_U2_LATENCY_FIELD		hcsparams3
#define CAP_U2_LATENCY_START		16
#define CAP_U2_LATENCY_LEN		16
#define CAP_CSZ_FIELD			hccparams
#define CAP_CSZ_START			2
#define CAP_CSZ_LEN			1

#define CAP_MASK(tok)		MASK(CAP_##tok##_START, CAP_##tok##_LEN)
#define CAP_GET(tok, cap)	((read32(&(cap)->CAP_##tok##_FIELD) & CAP_MASK(tok)) \
				 >> CAP_##tok##_START)

#define CTXSIZE(xhci) (CAP_GET(CSZ, (xhci)->capreg) ? 64 : 32)

typedef struct xhci {
	struct capreg {
		u32 hciparams;
		u32 hcsparams1;
		u32 hcsparams2;
		u32 hcsparams3;
		u32 hccparams;
		u32 dboff;
		u32 rtsoff;
	} __packed * capreg;

	/* opreg is R/W is most places, so volatile access is necessary.
	   volatile means that the compiler seeks byte writes if possible,
	   making bitfields unusable for MMIO register blocks. Yay C :-( */
	volatile struct opreg {
		u32 usbcmd; /* 0x00 */
#define USBCMD_RS (1 << 0)
#define USBCMD_HCRST (1 << 1)
#define USBCMD_INTE (1 << 2)
		u32 usbsts; /* 0x04 */
#define USBSTS_HCH (1 << 0)
#define USBSTS_HSE (1 << 2)
#define USBSTS_EINT (1 << 3)
#define USBSTS_PCD (1 << 4)
#define USBSTS_CNR (1 << 11)
#define USBSTS_PRSRV_MASK ((1 << 1) | 0xffffe000)
		u32 pagesize; /* 0x08 */
		u8 res1[0x13-0x0c+1]; /* 0x0C */
		u32 dnctrl; /* 0x14 */
		u32 crcr_lo; /* 0x18 */
		u32 crcr_hi; /* 0x1C */
#define CRCR_RCS (1 << 0)
#define CRCR_CS (1 << 1)
#define CRCR_CA (1 << 2)
#define CRCR_CRR (1 << 3)
		u8 res2[0x2f-0x20+1]; /* 0x20 */
		u32 dcbaap_lo; /* 0x30 */
		u32 dcbaap_hi; /* 0x34 */
		u32 config; /* 0x38 */
#define CONFIG_LP_MASK_MaxSlotsEn 0xff
		u8 res3[0x3ff-0x3c+1]; /* 0x3C */
		struct {
			u32 portsc; /* 0x400 + 4 * port */
#define PORTSC_CCS (1 << 0)
#define PORTSC_PED (1 << 1)
	// BIT 2 rsvdZ
#define PORTSC_OCA (1 << 3)
#define PORTSC_PR (1 << 4)
#define PORTSC_PLS (1 << 5)
#define PORTSC_PLS_MASK MASK(5, 4)
#define PORTSC_PP (1 << 9)
#define PORTSC_PORT_SPEED_START 10
#define PORTSC_PORT_SPEED (1 << PORTSC_PORT_SPEED_START)
#define PORTSC_PORT_SPEED_MASK MASK(PORTSC_PORT_SPEED_START, 4)
#define PORTSC_PIC (1 << 14)
#define PORTSC_PIC_MASK MASK(14, 2)
#define PORTSC_LWS (1 << 16)
#define PORTSC_CSC (1 << 17)
#define PORTSC_PEC (1 << 18)
#define PORTSC_WRC (1 << 19)
#define PORTSC_OCC (1 << 20)
#define PORTSC_PRC (1 << 21)
#define PORTSC_PLC (1 << 22)
#define PORTSC_CEC (1 << 23)
#define PORTSC_CAS (1 << 24)
#define PORTSC_WCE (1 << 25)
#define PORTSC_WDE (1 << 26)
#define PORTSC_WOE (1 << 27)
	// BIT 29:28 rsvdZ
#define PORTSC_DR (1 << 30)
#define PORTSC_WPR (1 << 31)
#define PORTSC_RW_MASK (PORTSC_PR | PORTSC_PLS_MASK | PORTSC_PP | PORTSC_PIC_MASK | PORTSC_LWS | PORTSC_WCE | PORTSC_WDE | PORTSC_WOE)
			u32 portpmsc; /* 0x404 + 4 * port */
			u32 portli; /* 0x408 + 4 * port */
			u32 res; /* 0x40C + 4 * port */
		} __packed prs[];
	} __packed * opreg;

	/* R/W, volatile, MMIO -> no bitfields */
	volatile struct hcrreg {
		u32 mfindex;
		u8 res1[0x20-0x4];
		struct {
			u32 iman;
			u32 imod;
			u32 erstsz;
			u32 res;
			u32 erstba_lo;
			u32 erstba_hi;
			u32 erdp_lo;
			u32 erdp_hi;
		} __packed intrrs[]; // up to 1024, but maximum host specific, given in capreg->MaxIntrs
	} __packed * hcrreg;

	/* R/W, volatile, MMIO -> no bitfields */
	volatile u32 *dbreg;

	/* R/W, volatile, Memory -> bitfields allowed */
	u64 *dcbaa;	/* pointers to sp_ptrs and output (device) contexts */
	u64 *sp_ptrs;	/* pointers to scratchpad buffers */

	command_ring_t cr;
	event_ring_t er;
	volatile erst_entry_t *ev_ring_table;

	usbdev_t *roothub;

	u8 max_slots_en;
	devinfo_t *dev;	/* array of devinfos by slot_id */

#define DMA_SIZE (64 * 1024)
	void *dma_buffer;
} xhci_t;

#define XHCI_INST(controller) ((xhci_t*)((controller)->instance))

void *xhci_align(const size_t min_align, const size_t size);
void xhci_init_cycle_ring(transfer_ring_t *, const size_t ring_size);
usbdev_t *xhci_set_address(hci_t *, usb_speed speed, int hubport, int hubaddr);
int xhci_finish_device_config(usbdev_t *);
void xhci_destroy_dev(hci_t *, int slot_id);

void xhci_reset_event_ring(event_ring_t *);
void xhci_advance_event_ring(xhci_t *);
void xhci_update_event_dq(xhci_t *);
void xhci_handle_events(xhci_t *);
int xhci_wait_for_command_aborted(xhci_t *, const trb_t *);
int xhci_wait_for_command_done(xhci_t *, const trb_t *, int clear_event);
int xhci_wait_for_transfer(xhci_t *, const int slot_id, const int ep_id);

void xhci_clear_trb(trb_t *, int pcs);

trb_t *xhci_next_command_trb(xhci_t *);
void xhci_post_command(xhci_t *);
int xhci_cmd_enable_slot(xhci_t *, int *slot_id);
int xhci_cmd_disable_slot(xhci_t *, int slot_id);
int xhci_cmd_address_device(xhci_t *, int slot_id, inputctx_t *);
int xhci_cmd_configure_endpoint(xhci_t *, int slot_id, int config_id, inputctx_t *);
int xhci_cmd_evaluate_context(xhci_t *, int slot_id, inputctx_t *);
int xhci_cmd_reset_endpoint(xhci_t *, int slot_id, int ep);
int xhci_cmd_stop_endpoint(xhci_t *, int slot_id, int ep);
int xhci_cmd_set_tr_dq(xhci_t *, int slot_id, int ep, trb_t *, int dcs);

static inline int xhci_ep_id(const endpoint_t *const ep) {
	return ((ep->endpoint & 0x7f) * 2) + (ep->direction != OUT);
}

#ifdef XHCI_DUMPS
void xhci_dump_slotctx(const slotctx_t *);
void xhci_dump_epctx(const epctx_t *);
void xhci_dump_devctx(const devctx_t *, const u32 ctx_mask);
void xhci_dump_inputctx(const inputctx_t *);
void xhci_dump_transfer_trb(const trb_t *);
void xhci_dump_transfer_trbs(const trb_t *first, const trb_t *last);
#else
#define xhci_dump_slotctx(args...)		do {} while (0)
#define xhci_dump_epctx(args...)		do {} while (0)
#define xhci_dump_devctx(args...)		do {} while (0)
#define xhci_dump_inputctx(args...)		do {} while (0)
#define xhci_dump_transfer_trb(args...)		do {} while (0)
#define xhci_dump_transfer_trbs(args...)	do {} while (0)
#endif

#endif
