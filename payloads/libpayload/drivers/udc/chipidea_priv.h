/*
 * This file is part of the libpayload project.
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

#ifndef __CHIPIDEA_PRIV_H__
#define __CHIPIDEA_PRIV_H__

#include <queue.h>

struct chipidea_opreg {
	uint8_t pad0[0x130];
	uint32_t usbcmd; // 0x130
	uint32_t usbsts; // 0x134
	uint32_t pad138[3];
	uint32_t usbadr; // 0x144
		/* 31:25: address
		 * 24: staging: 1 -> commit new address after
		 *                   next ctrl-in on ep0
		 */
	uint32_t epbase; // 0x148
	uint32_t pad14c[10];
	uint32_t portsc; // 0x174
	uint32_t pad178[15];
	uint32_t devlc; // 0x1b4
		/* 25:26: host-desired usb version
		 * 23: force full speed */
	uint32_t pad1b8[16];
	uint32_t usbmode; // 0x1f8
		/* 0:1: 2 -> device mode */
	uint32_t pad1fc[3];
	uint32_t epsetupstat; // 0x208
		/* 0:15: 1 -> epX received setup packet */
	uint32_t epprime; // 0x20c
		/*  0:15: 1 -> rx buffer for epX (OUT) is primed
		 *             (ie. ready for controller-side processing)
		 * 16:31: 1 -> tx buffer for ep(X-16) (IN/INTR) is primed
		 *             (ie. ready for controller-side processing)
		 *
		 * controller will read new td from qh and process it,
		 * then set the bit to 0
		 */
	uint32_t epflush; // 0x210
		/* 0:31: 1 -> flush buffer (as defined in epprime),
		 *            so it's uninitialized again.
		 * controller resets to 0 when done
		 */
	uint32_t epstat; // 0x214
		/* 0:31: 1 -> command in epprime is done, EP is ready
		 *           (which may be later than epprime reset)
		 */
	uint32_t epcomplete; // 0x218
		/*  0:15: 1 -> incoming out/setup packet for epX was handled.
		 *             software should check QH state
		 * 16:31: 1 -> incoming intr/in packet for ep(X-16) was
		 *             handled. software should check QH state
		 */
	uint32_t epctrl[16]; // 0x21c
		/* epctrl[0] is hardcoded as enabled control endpoint.
		 * TXS/RXS for stalling can be written.
		 *
		 * 23: TXE tx endpoint enable
		 * 22: TXR reset tx data toggle (for every configuration event)
		 * 18:19: 0=ctrl, 1=isoc, 2=bulk, 3=intr endpoint
		 * 16: TXS stall tx
		 *
		 *  7: RXE rx endpoint enable
		 *  6: RXR reset rx data toggle (for every configuration event)
		 *  2:3: endpoint type (like 18:19)
		 *  0: RXS stall rx
		 */
	uint32_t pad25c[0x69]; // 0x25c
	uint32_t susp_ctrl; // 0x400
};

#define CI_PDATA(ctrl) ((struct chipidea_pdata *)((ctrl)->pdata))
#define CI_QHELEMENTS 32

#define QH_NO_AUTO_ZLT (1 << 29) /* no automatic ZLT handling by chipset */
#define QH_MPS(x) ((x) << 16)
#define QH_IOS (1 << 15) /* IRQ on setup */

#define TD_INFO_LEN(x) ((x) << 16)
#define TD_INFO_IOC (1 << 15)
#define TD_INFO_ACTIVE (1 << 7)
#define TD_TERMINATE 1

#define USBCMD_8MICRO (8 << 16)
#define USBCMD_RST 2
#define USBCMD_RUN 1

#define USBSTS_SLI (1 << 8)
#define USBSTS_URI (1 << 6)
#define USBSTS_PCI (1 << 2)
#define USBSTS_UEI (1 << 1)
#define USBSTS_UI  (1 << 0)

#define DEVLC_HOSTSPEED(x) (x << 25)
#define DEVLC_HOSTSPEED_MASK DEVLC_HOSTSPEED(3)

struct td {
	/* points to next td */
	uint32_t next;
	uint32_t info;
	/* page0..4 are like EHCI pages: up to 4k each
	 * page0 from addr to page end, page4 to its length
	 */
	uint32_t page0;
	uint32_t page1;
	uint32_t page2;
	uint32_t page3;
	uint32_t page4;
	uint32_t res;
};

struct qh {
	uint32_t config;
	uint32_t current;
	struct td td;
	/* contains the data of a setup request */
	uint8_t setup_data[8];
	uint32_t res[4];
};

struct job {
	SIMPLEQ_ENTRY(job) queue; // linkage
	struct td *tds; // for later free()ing
	int td_count;
	void *data;
	size_t length;
	int zlp; // append zero length packet?
	int autofree; // free after processing?
};

SIMPLEQ_HEAD(job_queue, job);

struct chipidea_pdata {
	struct chipidea_opreg *opreg;
	struct qh *qhlist;
	struct job_queue job_queue[16][2];
	int ep_busy[16][2];
};

#endif
