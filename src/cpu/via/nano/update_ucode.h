/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012  Alexandru Gagniuc <mr.nuke.me@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */
#ifndef __UPDATE_UCODE_H
#define __UPDATE_UCODE_H

#include <cpu/cpu.h>

#define MSR_IA32_BIOS_UPDT_TRIG         0x00000079
#define MSR_IA32_BIOS_SIGN_ID           0x0000008b
#define MSR_UCODE_UPDATE_STATUS         0x00001205

#define NANO_UCODE_SIGNATURE            0x53415252
#define NANO_UCODE_HEADER_SIZE          0x30

/* These are values returned by the CPU after we attempt microcode updates.
 * We care what these values are exactly, so we define them to be sure */
typedef enum {
	UCODE_UPDATE_NOT_ATTEMPTED     = 0x0,
	UCODE_UPDATE_SUCCESS           = 0x1,
	UCODE_UPDATE_FAIL              = 0x2,
	UCODE_UPDATE_WRONG_CPU         = 0x3,
	UCODE_INVALID_UPDATE_BLOCK     = 0x4,
} ucode_update_status;


typedef enum {
	NANO_UCODE_VALID = 0, /* We only care that valid == 0 */
	NANO_UCODE_SIGNATURE_ERROR,
	NANO_UCODE_WRONG_SIZE,
	NANO_UCODE_CHECKSUM_FAIL,
} ucode_validity;

typedef struct {
	u32 signature;          /* NANO_UCODE_SIGNATURE */
	u32 update_revision;    /* Revision of the update header */
	u16 year;               /* Year  of patch release */
	u8  day;                /* Day   of patch release */
	u8  month;              /* Month of patch release */
	u32 applicable_fms;     /* Fam/model/stepping to which ucode applies */
	u32 checksum;           /* Two's complement checksum of ucode+header */
	u32 loader_revision;    /* Revision of hardware ucode update loader*/
	u32 rfu_1;              /* Reserved for future use */
	u32 payload_size;       /* Size of the ucode payload only */
	u32 total_size;         /* Size of the ucode, including header */
	char name[8];           /* ASCII string of ucode filename */
	u32 rfu_2;              /* Reserved for future use */
	/* First double-word of the ucode payload
	 * Its address represents the beginning of the ucode update we need to
	 * send to the CPU */
	u32 ucode_start;

} nano_ucode_header;

unsigned int nano_update_ucode(void);

#endif /* __UPDATE_UCODE_H */
