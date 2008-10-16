/*
 * This file is part of the libpayload project.
 *
 * Copyright (C) 2008 coresystems GmbH
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

#ifndef __UHCI_H
#define __UHCI_H

#include <pci.h>
#include <usb/usb.h>

typedef union {
	struct {
		unsigned long terminate:1;
		unsigned long queue_head:1;
		unsigned long:2;
		unsigned long ptr_part:28;
	};
	u32 ptr;
} __attribute__ ((packed)) flistp_t;

typedef struct {
	union {
		struct {
			unsigned long terminate:1;
			unsigned long queue_head:1;
			unsigned long depth_first:1;
			unsigned long:29;
		} __attribute__ ((packed));
		u32 ptr;
	} __attribute__ ((packed));

	volatile unsigned long actlen:11;
	volatile unsigned long:5;
	union {
		struct {
			unsigned long:1;	// bit 0
			unsigned long status_bitstuff_err:1;
			unsigned long status_crc_err:1;
			unsigned long status_nakrcvd:1;
			unsigned long status_babble:1;
			unsigned long status_databuf_err:1;
			unsigned long status_stalled:1;
			unsigned long status_active:1;	// bit 7
		} __attribute__ ((packed));
		unsigned char status;
	} __attribute__ ((packed));
	volatile unsigned long ioc:1;	/* interrupt on complete */
	volatile unsigned long isochronous:1;
	volatile unsigned long lowspeed:1;
	volatile unsigned long counter:2;
	volatile unsigned long shortpck:1;
	volatile unsigned long:2;

	unsigned long pid:8;
	unsigned long dev_addr:7;
	unsigned long endp:4;
	unsigned long data_toggle:1;
	unsigned long:1;
	unsigned long maxlen:11;

	u32 bufptr;

} __attribute__ ((packed))
     td_t;

     typedef struct {
	     flistp_t headlinkptr;
	     volatile flistp_t elementlinkptr;
     } __attribute__ ((packed))
     qh_t;

     typedef enum { USBCMD = 0, USBSTS = 2, USBINTR = 4, FRNUM =
		     6, FLBASEADD = 8, SOFMOD = 0xc, PORTSC1 = 0x10, PORTSC2 =
		     0x12
     } usbreg;

     void uhci_reg_write32 (hci_t *ctrl, usbreg reg, u32 value);
     u32 uhci_reg_read32 (hci_t *ctrl, usbreg reg);
     void uhci_reg_write16 (hci_t *ctrl, usbreg reg, u16 value);
     u16 uhci_reg_read16 (hci_t *ctrl, usbreg reg);
     void uhci_reg_write8 (hci_t *ctrl, usbreg reg, u8 value);
     u8 uhci_reg_read8 (hci_t *ctrl, usbreg reg);
     void uhci_reg_mask32 (hci_t *ctrl, usbreg reg, u32 andmask, u32 ormask);
     void uhci_reg_mask16 (hci_t *ctrl, usbreg reg, u16 andmask, u16 ormask);
     void uhci_reg_mask8 (hci_t *ctrl, usbreg reg, u8 andmask, u8 ormask);

     typedef struct uhci {
	     flistp_t *framelistptr;
	     qh_t *qh_prei, *qh_intr, *qh_data, *qh_last;
	     usbdev_t *roothub;
     } uhci_t;

#define UHCI_INST(controller) ((uhci_t*)((controller)->instance))

     hci_t *uhci_init (pcidev_t addr);

     void uhci_rh_init (usbdev_t *dev);

#endif
