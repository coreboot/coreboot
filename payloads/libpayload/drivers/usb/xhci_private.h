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

#ifndef __XHCI_PRIVATE_H
#define __XHCI_PRIVATE_H

#include <usb/usb.h>

#define MASK(startbit, lenbit) (((1<<(lenbit))-1)<<(startbit))

typedef volatile union trb {
	// transfer

	// events
#define TRB_EV_CMD_CMPL 33
	struct {
		u32 Cmd_TRB_Pointer_lo;
		u32 Cmd_TRB_Pointer_hi;
		struct {
			unsigned long:24;
			unsigned long Completion_Code:8;
		} __attribute__ ((packed));
		struct {
			unsigned long C:1;
			unsigned long:9;
			unsigned long TRB_Type:6;
			unsigned long VF_ID:8;
			unsigned long Slot_ID:8;
		} __attribute__ ((packed));
	} __attribute__ ((packed)) event_cmd_cmpl;

#define TRB_EV_PORTSC 34
	struct {
		struct {
			unsigned long:24;
			unsigned long Port:8;
		} __attribute__ ((packed));
		u32 rsvd;
		struct {
			unsigned long:24;
			unsigned long Completion_Code:8;
		} __attribute__ ((packed));
		struct {
			unsigned long C:1;
			unsigned long:9;
			unsigned long TRB_Type:6;
			unsigned long:16;
		} __attribute__ ((packed));
	} __attribute__ ((packed)) event_portsc;

	// commands
#define TRB_CMD_NOOP 23
	struct {
		u32 rsvd[3];
		struct {
			unsigned long C:1;
			unsigned long:9;
			unsigned long TRB_Type:6;
			unsigned long:16;
		} __attribute__ ((packed));
	} __attribute__ ((packed)) cmd_No_Op;

	// "others"
	struct {
		u32 Ring_Segment_Ptr_lo;
		u32 Ring_Segment_Ptr_hi;
		struct {
			unsigned long:22;
			unsigned long Interrupter_Target;
		} __attribute__ ((packed));
		struct {
			unsigned long C:1;
			unsigned long TC:1;
			unsigned long:2;
			unsigned long CH:1;
			unsigned long IOC:1;
			unsigned long:4;
			unsigned long TRB_Type:6;
			unsigned long:16;
		} __attribute__ ((packed));
	} __attribute__ ((packed)) link;
} trb_t;

typedef struct slotctx {
	struct {
		unsigned long Route_String:20;
		unsigned long Speed:4;
		unsigned long:1;
		unsigned long MTT:1;
		unsigned long Hub:1;
		unsigned long Context_Entries:5;
	} __attribute__ ((packed));
	struct {
		unsigned long Max_Exit_Latency:16;
		unsigned long Root_Hub_Port_Number:8;
		unsigned long Number_of_Ports:8;
	} __attribute__ ((packed));
	struct {
		unsigned long TT_Hub_Slot_ID:8;
		unsigned long TT_Port_Number:8;
		unsigned long TTT:2;
		unsigned long:4;
		unsigned long Interrupter_Target:10;
	} __attribute__ ((packed));
	struct {
		unsigned long USB_Device_Address:8;
		unsigned long:19;
		unsigned long Slot_State:5;
	} __attribute__ ((packed));
	u32 rsvd[4];
} slotctx_t;

typedef struct epctx {
	struct {
		unsigned long EP_State:3;
		unsigned long:5;
		unsigned long Mult:2;
		unsigned long MaxPStreams:5;
		unsigned long LSA:1;
		unsigned long Interval:8;
		unsigned long:8;
	} __attribute__ ((packed));
	struct {
		unsigned long:1;
		unsigned long CErr:2;
		unsigned long EP_Type:3;
		unsigned long:1;
		unsigned long HID:1;
		unsigned long Max_Burst_Size:8;
		unsigned long Max_Packet_Size:16;
	} __attribute__ ((packed));
	union {
		u32 TR_Dequeue_Pointer_lo;
		struct {
			unsigned long DCS:1;
			unsigned long:3;
		} __attribute__ ((packed));
	} __attribute__ ((packed));
	u32 TR_Dequeue_Pointer_hi;
	struct {
		unsigned long Average_TRB_Length:16;
		unsigned long Max_ESIT_Payload:16;
	} __attribute__ ((packed));
	u32 rsvd[3];
} epctx_t;

typedef struct devctx {
	slotctx_t slot;
	epctx_t ep0;
	struct {
		epctx_t out;
		epctx_t in;
	} eps[15];
} devctx_t;

typedef struct devctxp {
	devctx_t *ptr;
	void *upper;
} devctxp_t;

typedef struct erst_entry {
	u32 seg_base_lo;
	u32 seg_base_hi;
	u32 seg_size;
	u32 rsvd;
} erst_entry_t;

typedef struct xhci {
	/* capreg is read-only, so no need for volatile,
	   and thus 32bit accesses can be assumed. */
	struct capreg {
		u8 caplength;
		u8 res1;
		union {
			u16 hciversion;
			struct {
				u8 hciver_lo;
				u8 hciver_hi;
			} __attribute__ ((packed));
		} __attribute__ ((packed));
		union {
			u32 hcsparams1;
			struct {
				unsigned long MaxSlots:7;
				unsigned long MaxIntrs:11;
				unsigned long:6;
				unsigned long MaxPorts:8;
			} __attribute__ ((packed));
		} __attribute__ ((packed));
		union {
			u32 hcsparams2;
			struct {
				unsigned long IST:4;
				unsigned long ERST_Max:4;
				unsigned long:18;
				unsigned long SPR:1;
				unsigned long Max_Scratchpad_Bufs:5;
			} __attribute__ ((packed));
		} __attribute__ ((packed));
		union {
			u32 hcsparams3;
			struct {
				unsigned long u1latency:8;
				unsigned long:8;
				unsigned long u2latency:16;
			} __attribute__ ((packed));
		} __attribute__ ((packed));
		union {
			u32 hccparams;
			struct {
				unsigned long ac64:1;
				unsigned long bnc:1;
				unsigned long csz:1;
				unsigned long ppc:1;
				unsigned long pind:1;
				unsigned long lhrc:1;
				unsigned long ltc:1;
				unsigned long nss:1;
				unsigned long:4;
				unsigned long MaxPSASize:4;
				unsigned long xECP:16;
			} __attribute__ ((packed));
		} __attribute__ ((packed));
		u32 dboff;
		u32 rtsoff;
	} __attribute__ ((packed)) *capreg;

	/* opreg is R/W is most places, so volatile access is necessary.
	   volatile means that the compiler seeks byte writes if possible,
	   making bitfields unusable for MMIO register blocks. Yay C :-( */
	volatile struct opreg {
		u32 usbcmd;
#define USBCMD_RS 1<<0
#define USBCMD_HCRST 1<<1
		u32 usbsts;
#define USBSTS_HCH 1<<0
#define USBSTS_HSE 1<<2
#define USBSTS_EINT 1<<3
#define USBSTS_PCD 1<<4
#define USBSTS_CNR 1<<11
		u32 pagesize;
		u8 res1[0x13-0x0c+1];
		u32 dnctrl;
		u32 crcr_lo;
		u32 crcr_hi;
#define CRCR_RCS 1<<0
#define CRCR_CS 1<<1
#define CRCR_CA 1<<2
#define CRCR_CRR 1<<3
		u8 res2[0x2f-0x20+1];
		u32 dcbaap_lo;
		u32 dcbaap_hi;
		u32 config;
#define CONFIG_MASK_MaxSlotsEn 0xff
		u8 res3[0x3ff-0x3c+1];
		struct {
			u32 portsc;
#define PORTSC_CCS 1<<0
#define PORTSC_PED 1<<1
	// BIT 2 rsvdZ
#define PORTSC_OCA 1<<3
#define PORTSC_PR 1<<4
#define PORTSC_PLS 1<<5
#define PORTSC_PLS_MASK MASK(5, 4)
#define PORTSC_PP 1<<9
#define PORTSC_PORT_SPEED 1<<10
#define PORTSC_PORT_SPEED_MASK MASK(10, 4)
#define PORTSC_PIC 1<<14
#define PORTSC_PIC_MASK MASK(14, 2)
#define PORTSC_LWS 1<<16
#define PORTSC_CSC 1<<17
#define PORTSC_PEC 1<<18
#define PORTSC_WRC 1<<19
#define PORTSC_OCC 1<<20
#define PORTSC_PRC 1<<21
#define PORTSC_PLC 1<<22
#define PORTSC_CEC 1<<23
#define PORTSC_CAS 1<<24
#define PORTSC_WCE 1<<25
#define PORTSC_WDE 1<<26
#define PORTSC_WOE 1<<27
	// BIT 29:28 rsvdZ
#define PORTSC_DR 1<<30
#define PORTSC_WPR 1<<31
#define PORTSC_RW_MASK PORTSC_PR | PORTSC_PLS_MASK | PORTSC_PP | PORTSC_PIC_MASK | PORTSC_LWS | PORTSC_WCE | PORTSC_WDE | PORTSC_WOE
			u32 portpmsc;
			u32 portli;
			u32 res;
		} __attribute__ ((packed)) prs[];
	} __attribute__ ((packed)) *opreg;

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
		} __attribute__ ((packed)) intrrs[]; // up to 1024, but maximum host specific, given in capreg->MaxIntrs
	} __attribute__ ((packed)) *hcrreg;

	/* R/W, volatile, MMIO -> no bitfields */
	volatile u32 *dbreg;

	/* R/W, volatile, Memory -> bitfields allowed */
	volatile devctxp_t *dcbaa;

	trb_t *cmd_ring;
	trb_t *ev_ring;
	volatile erst_entry_t *ev_ring_table;
	int cmd_ccs, ev_ccs;

	usbdev_t *roothub;
} xhci_t;

#define XHCI_INST(controller) ((xhci_t*)((controller)->instance))

#endif
