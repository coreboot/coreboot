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

#ifndef _STORAGE_ATAPI_H
#define _STORAGE_ATAPI_H

#include <stdint.h>

#include "storage.h"
#include "ata.h"

/* ATAPI commands */
enum {
	ATAPI_TEST_UNIT_READY			= 0x00,
	ATAPI_REQUEST_SENSE			= 0x03,
	ATAPI_START_STOP_UNIT			= 0x1b,
	ATAPI_PREVENT_ALLOW_MEDIUM_REMOVAL	= 0x1e,
	ATAPI_READ_CAPACITY			= 0x25,
	ATAPI_READ_10				= 0x28,
};

/* ATAPI sense codes */
enum {
	ATAPI_SENSE_NOT_READY		= 0x02,
	ATAPI_SENSE_UNIT_ATTENTION	= 0x06,
};

/* ATAPI additional sense codes (particularize the above) */
enum {
	ATAPI_ADDITIONAL_SENSE_LOGICAL_UNIT_NOT_READY	= 0x04,
	ATAPI_ADDITIONAL_SENSE_MEDIUM_NOT_PRESENT	= 0x3a,
};

struct atapi_dev;

typedef struct atapi_dev {
	/* Keep this at offset 0 so we can cast to ata_dev_t. */
	ata_dev_t ata_dev;

	int (*identify)(struct ata_dev *, u8 *buf); /* yes, ata_dev_t */
	ssize_t (*packet_read_cmd)(struct atapi_dev *, const u8 *cmd, size_t cmdlen, u8 *buf, size_t buflen);

	u8 sense_data[19]; /* We needed 19 in usbmsc.c. */
	u8 medium_present;

	void (*detach_device)(struct atapi_dev *);
} atapi_dev_t;

int atapi_attach_device(atapi_dev_t *, storage_port_t);

#endif
