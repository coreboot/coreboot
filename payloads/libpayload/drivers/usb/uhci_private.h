/*
 *
 * Copyright (C) 2008-2010 coresystems GmbH
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

#ifndef __UHCI_PRIVATE_H
#define __UHCI_PRIVATE_H

typedef enum { UHCI_SETUP = 0x2d, UHCI_IN = 0x69, UHCI_OUT = 0xe1 } uhci_pid_t;

typedef u32 flistp_t;
#define FLISTP_TERMINATE 1
#define FLISTP_QH 2

typedef struct {
	u32 ptr;
#define TD_TERMINATE 1
#define TD_QH 2
#define TD_DEPTH_FIRST 4

	u32 ctrlsts;
#define TD_STATUS_MASK (0xff << 16)
#define TD_STATUS_BITSTUFF_ERR (1 << 17)
#define TD_STATUS_CRC_ERR (1 << 18)
#define TD_STATUS_NAK_RCVD (1 << 19)
#define TD_STATUS_BABBLE (1 << 20)
#define TD_STATUS_DATABUF_ERR (1 << 21)
#define TD_STATUS_STALLED (1 << 22)
#define TD_STATUS_ACTIVE (1 << 23)
#define TD_LOWSPEED (1 << 26)
#define TD_COUNTER_SHIFT 27

	u32 token;
#define TD_PID_MASK 0xff
#define TD_DEVADDR_SHIFT 8
#define TD_DEVADDR_MASK (((1 << 7)-1) << TD_DEVADDR_SHIFT)
#define TD_EP_SHIFT 15
#define TD_EP_MASK (0xf << TD_EP_SHIFT)
#define TD_TOGGLE_SHIFT 19
#define TD_MAXLEN_SHIFT 21
#define TD_TOGGLE_DATA0 0
#define TD_TOGGLE_DATA1 (1 << TD_TOGGLE_SHIFT)

	u32 bufptr;

} __packed
     td_t;

     typedef struct {
	     flistp_t headlinkptr;
	     volatile flistp_t elementlinkptr;
     } __packed
     qh_t;

     typedef enum { USBCMD = 0, USBSTS = 2, USBINTR = 4, FRNUM =
		     6, FLBASEADD = 8, SOFMOD = 0xc, PORTSC1 = 0x10, PORTSC2 =
		     0x12
     } usbreg;

     void uhci_reg_write32(hci_t *ctrl, usbreg reg, u32 value);
     u32 uhci_reg_read32(hci_t *ctrl, usbreg reg);
     void uhci_reg_write16(hci_t *ctrl, usbreg reg, u16 value);
     u16 uhci_reg_read16(hci_t *ctrl, usbreg reg);
     void uhci_reg_write8(hci_t *ctrl, usbreg reg, u8 value);
     u8 uhci_reg_read8(hci_t *ctrl, usbreg reg);

     typedef struct uhci {
	     flistp_t *framelistptr;
	     qh_t *qh_prei, *qh_intr, *qh_data, *qh_last;
	     usbdev_t *roothub;
     } uhci_t;

#define UHCI_INST(controller) ((uhci_t*)((controller)->instance))

#endif
