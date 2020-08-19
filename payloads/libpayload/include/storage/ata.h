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

#ifndef _STORAGE_ATA_H
#define _STORAGE_ATA_H

#include <stdint.h>

#include "storage.h"

/* ATA commands */
enum {
	ATA_READ_DMA			= 0xc8,
	ATA_READ_DMA_EXT		= 0x25,
	ATA_IDENTIFY_DEVICE		= 0xec,
	ATA_PACKET			= 0xa0,
	ATA_IDENTIFY_PACKET_DEVICE	= 0xa1,
};

/* 16-bit-word indices into id structure from ATA_IDENTIFY_DEVICE */
enum {
	ATA_CMDS_AND_FEATURE_SETS	=  82,
	ATA_ID_SECTOR_SIZE		= 106,
	ATA_ID_LOGICAL_SECTOR_SIZE	= 117,
};

#define DEFAULT_ATA_SECTOR_SIZE 512

struct ata_dev;
typedef struct ata_dev {
	storage_dev_t storage_dev;

	int (*identify)(struct ata_dev *, u8 *buf);
	ssize_t (*read_sectors)(struct ata_dev *, lba_t start, size_t count, u8 *buf);

	u8 read_cmd;
	u8 identify_cmd;
	size_t sector_size;
	size_t sector_size_shift;

	void (*detach_device)(struct ata_dev *);
} ata_dev_t;

int ata_attach_device(ata_dev_t *, storage_port_t);

char *ata_strncpy(char *dest, const u16 *src, size_t n);
int ata_set_sector_size(ata_dev_t *, u32 sector_size);
void ata_initialize_storage_ops(ata_dev_t *);

#endif
