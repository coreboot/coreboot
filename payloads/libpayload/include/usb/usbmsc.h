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

#ifndef __USBMSC_H
#define __USBMSC_H
typedef struct {
	unsigned int blocksize;
	unsigned int numblocks;
	endpoint_t *bulk_in;
	endpoint_t *bulk_out;
	u8 quirks		: 7;
	u8 usbdisk_created	: 1;
	s8 ready;
	u8 lun;
	u8 num_luns;
	void *data; /* For use by consumers of libpayload. */
} usbmsc_inst_t;

/* Possible values for quirks field. */
enum {
	/* Don't check for LUNs (force assumption that there's only one LUN). */
	USB_MSC_QUIRK_NO_LUNS	= 1 << 0,
	/* Never do a BULK_ONLY reset, just continue. This means that the device
	   cannot recover from phase errors and won't detach automatically for
	   unrecoverable errors. Do not use unless you have to. */
	USB_MSC_QUIRK_NO_RESET	= 1 << 1,
};

/* Possible values for ready field. */
enum {
	USB_MSC_DETACHED = -1, /* Disk detached or out to lunch. */
	USB_MSC_NOT_READY = 0, /* Disk not ready yet -- empty card reader */
	USB_MSC_READY = 1,     /* Disk ready to communicate. */
};

#define MSC_INST(dev) ((usbmsc_inst_t*)(dev)->data)

typedef enum { cbw_direction_data_in = 0x80, cbw_direction_data_out = 0
} cbw_direction;

int readwrite_blocks_512 (usbdev_t *dev, int start, int n, cbw_direction dir, u8 *buf);
int readwrite_blocks (usbdev_t *dev, int start, int n, cbw_direction dir, u8 *buf);

/* Force a device to enumerate as MSC, without checking class/protocol types.
   It must still have a bulk endpoint pair and respond to MSC commands. */
void usb_msc_force_init (usbdev_t *dev, u32 quirks);

#endif
