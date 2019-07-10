/*
 *
 * Copyright (C) 2012 secunet Security Networks AG
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

#ifndef _STORAGE_STORAGE_H
#define _STORAGE_STORAGE_H

#include <stdint.h>
#include <unistd.h>

#if !CONFIG(LP_STORAGE_64BIT_LBA)
typedef u32 lba_t;
#else
typedef u64 lba_t;
#endif

typedef enum {
	PORT_TYPE_IDE	= (1 << 0),
	PORT_TYPE_SATA	= (1 << 1),
	PORT_TYPE_USB	= (1 << 2),
	PORT_TYPE_NVME	= (1 << 3),
} storage_port_t;

typedef enum {
	POLL_MEDIUM_ERROR	= -3,
	POLL_NO_DEVICE		= -2,
	POLL_ERROR		= -1,
	POLL_NO_MEDIUM		=  0,
	POLL_MEDIUM_PRESENT	=  1,
} storage_poll_t;

struct storage_dev;

typedef struct storage_dev {
	storage_port_t port_type;

	storage_poll_t (*poll)(struct storage_dev *);
	ssize_t (*read_blocks512)(struct storage_dev *, lba_t start, size_t count, unsigned char *buf);
	ssize_t (*write_blocks512)(struct storage_dev *, lba_t start, size_t count, const unsigned char *buf);

	void (*detach_device)(struct storage_dev *);
} storage_dev_t;

int storage_device_count(void);
int storage_attach_device(storage_dev_t *dev);

storage_poll_t storage_probe(size_t dev_num);
ssize_t storage_read_blocks512(size_t dev_num, lba_t start, size_t count, unsigned char *buf);

#endif
