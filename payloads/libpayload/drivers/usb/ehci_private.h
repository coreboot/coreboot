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

#ifndef __EHCI_PRIVATE_H
#define __EHCI_PRIVATE_H

#include <pci.h>
#include <usb/usb.h>

#define USBBASE 0x10
#define FLADJ 0x61
#define FLADJ_framelength(x) (((x)-59488)/16)

typedef volatile u32 portsc_t;
#define P_CURR_CONN_STATUS (1 << 0)
#define P_CONN_STATUS_CHANGE (1 << 1)
#define P_PORT_ENABLE (1 << 2)
#define P_PORT_RESET (1 << 8)
#define P_LINE_STATUS (3 << 10)
#define P_LINE_STATUS_LOWSPEED (1 << 10)
#define P_PP (1 << 12)
#define P_PORT_OWNER (1 << 13)

typedef volatile struct {
#define HCS_NPORTS_MASK 0xf
#define HCS_PORT_POWER_CONTROL 0x10
	u8 caplength;
	u8 res1;
	u16 hciversion;
	u32 hcsparams;
	u32 hccparams;
	u64 hcsp_portroute;
} __attribute__ ((packed)) hc_cap_t;

typedef volatile struct {
	u32 usbcmd;
#define HC_OP_RS 1
#define HC_OP_HC_RESET (1 << 1)
#define HC_OP_PERIODIC_SCHED_EN_SHIFT 4
#define HC_OP_PERIODIC_SCHED_EN (1 << HC_OP_PERIODIC_SCHED_EN_SHIFT)
#define HC_OP_ASYNC_SCHED_EN_SHIFT 5
#define HC_OP_ASYNC_SCHED_EN (1 << HC_OP_ASYNC_SCHED_EN_SHIFT)
	u32 usbsts;
#define HC_OP_PERIODIC_SCHED_STAT_SHIFT 14
#define HC_OP_PERIODIC_SCHED_STAT (1 << HC_OP_PERIODIC_SCHED_STAT_SHIFT)
#define HC_OP_ASYNC_SCHED_STAT_SHIFT 15
#define HC_OP_ASYNC_SCHED_STAT (1 << HC_OP_ASYNC_SCHED_STAT_SHIFT)
#define HC_OP_HC_HALTED_SHIFT 12
#define HC_OP_HC_HALTED (1 << HC_OP_HC_HALTED_SHIFT)
	u32 usbintr;
	u32 frindex;
	u32 ctrldssegment;
	u32 periodiclistbase;
	u32 asynclistaddr;
	u8 res1[0x3f-0x1c];
	u32 configflag;
	portsc_t portsc[0];
} hc_op_t;

typedef volatile struct {
#define QTD_TERMINATE 1
#define QTD_PTR_MASK ~0x1f
	u32 next_qtd;
	u32 alt_next_qtd;
	u32 token;
#define QTD_STATUS_MASK 0xff
#define QTD_HALTED (1 << 6)
#define QTD_ACTIVE (1 << 7)
#define QTD_PID_SHIFT 8
#define QTD_PID_MASK (3 << QTD_PID_SHIFT)
#define QTD_CERR_SHIFT 10
#define QTD_CERR_MASK (3 << QTD_CERR_SHIFT)
#define QTD_CPAGE_SHIFT 12
#define QTD_CPAGE_MASK (7 << QTD_CPAGE_SHIFT)
#define QTD_TOTAL_LEN_SHIFT 16
#define QTD_TOTAL_LEN_MASK (((1<<15)-1) << QTD_TOTAL_LEN_SHIFT)
#define QTD_TOGGLE_SHIFT 31
#define QTD_TOGGLE_MASK (1 << 31)
#define QTD_TOGGLE_DATA0 0
#define QTD_TOGGLE_DATA1 (1 << QTD_TOGGLE_SHIFT)
	u32 bufptrs[5];
	u32 bufptrs64[5];
} __attribute__ ((packed)) qtd_t;

typedef volatile struct {
	u32 horiz_link_ptr;
#define QH_TERMINATE 1
#define QH_iTD (0<<1)
#define QH_QH (1<<1)
#define QH_siTD (2<<1)
#define QH_FSTN (3<<1)
	u32 epchar;
#define QH_EP_SHIFT 8
#define QH_EPS_SHIFT 12
#define QH_DTC_SHIFT 14
#define QH_RECLAIM_HEAD_SHIFT 15
#define QH_MPS_SHIFT 16
#define QH_NON_HS_CTRL_EP_SHIFT 27
#define QH_NAK_CNT_SHIFT 28
	u32 epcaps;
#define QH_UFRAME_CMASK_SHIFT 8
#define QH_HUB_ADDRESS_SHIFT 16
#define QH_PORT_NUMBER_SHIFT 23
#define QH_PIPE_MULTIPLIER_SHIFT 30
	volatile u32 current_td_ptr;
	volatile qtd_t td;
} ehci_qh_t;

typedef struct ehci {
	hc_cap_t *capabilities;
	hc_op_t *operation;
	ehci_qh_t *dummy_qh;
} ehci_t;

#define PS_TERMINATE 1
#define PS_TYPE_QH 1 << 1
#define PS_PTR_MASK ~0x1f


#define EHCI_INST(controller) ((ehci_t*)((controller)->instance))

#endif
