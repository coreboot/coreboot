/*
 * This file is part of the libpayload project.
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
	unsigned int protocol;
	endpoint_t *bulk_in;
	endpoint_t *bulk_out;
	int usbdisk_created;
	void *data; /* For use by consumers of libpayload. */
} usbmsc_inst_t;

#define MSC_INST(dev) ((usbmsc_inst_t*)(dev)->data)

typedef enum { cbw_direction_data_in = 0x80, cbw_direction_data_out = 0
} cbw_direction;

int readwrite_blocks_512 (usbdev_t *dev, int start, int n, cbw_direction dir, u8 *buf);
int readwrite_blocks (usbdev_t *dev, int start, int n, cbw_direction dir, u8 *buf);

#endif
