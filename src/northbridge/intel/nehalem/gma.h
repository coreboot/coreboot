/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Chromium OS Authors
 * Copyright (C) 2013 Vladimir Serbinenko
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef __NORTHBRIDGE_INTEL_NEHALEM_GMA_H__
#define __NORTHBRIDGE_INTEL_NEHALEM_GMA_H__

/* mailbox 0: header */
typedef struct {
	u8	signature[16];
	u32	size;
	u32	version;
	u8	sbios_version[32];
	u8	vbios_version[16];
	u8	driver_version[16];
	u32	mailboxes;
	u8	reserved[164];
} __attribute__((packed)) opregion_header_t;

#define IGD_OPREGION_SIGNATURE "IntelGraphicsMem"
#define IGD_OPREGION_VERSION  2

#define IGD_MBOX1	(1 << 0)
#define IGD_MBOX2	(1 << 1)
#define IGD_MBOX3	(1 << 2)
#define IGD_MBOX4	(1 << 3)
#define IGD_MBOX5	(1 << 4)

#define MAILBOXES_MOBILE  (IGD_MBOX1 | IGD_MBOX2 | IGD_MBOX3 | \
			   IGD_MBOX4 | IGD_MBOX5)
#define MAILBOXES_DESKTOP (IGD_MBOX2 | IGD_MBOX4)

#define SBIOS_VERSION_SIZE 32

/* mailbox 1: public acpi methods */
typedef struct {
	u32	drdy;
	u32	csts;
	u32	cevt;
	u8	reserved1[20];
	u32	didl[8];
	u32	cpdl[8];
	u32	cadl[8];
	u32	nadl[8];
	u32	aslp;
	u32	tidx;
	u32	chpd;
	u32	clid;
	u32	cdck;
	u32	sxsw;
	u32	evts;
	u32	cnot;
	u32	nrdy;
	u8	reserved2[60];
} __attribute__((packed)) opregion_mailbox1_t;

/* mailbox 2: software sci interface */
typedef struct {
	u32	scic;
	u32	parm;
	u32	dslp;
	u8	reserved[244];
} __attribute__((packed)) opregion_mailbox2_t;

/* mailbox 3: power conservation */
typedef struct {
	u32	ardy;
	u32	aslc;
	u32	tche;
	u32	alsi;
	u32	bclp;
	u32	pfit;
	u32	cblv;
	u16	bclm[20];
	u32	cpfm;
	u32	epfm;
	u8	plut[74];
	u32	pfmb;
	u32	ccdv;
	u32	pcft;
	u8	reserved[94];
} __attribute__((packed)) opregion_mailbox3_t;

#define IGD_BACKLIGHT_BRIGHTNESS 0xff
#define IGD_INITIAL_BRIGHTNESS 0x64

#define IGD_FIELD_VALID	(1 << 31)
#define IGD_WORD_FIELD_VALID (1 << 15)
#define IGD_PFIT_STRETCH 6

/* mailbox 4: vbt */
typedef struct {
	u8 gvd1[7168];
} __attribute__((packed)) opregion_vbt_t;

/* IGD OpRegion */
typedef struct {
	opregion_header_t header;
	opregion_mailbox1_t mailbox1;
	opregion_mailbox2_t mailbox2;
	opregion_mailbox3_t mailbox3;
	opregion_vbt_t vbt;
} __attribute__((packed)) igd_opregion_t;

#endif /* __NORTHBRIDGE_INTEL_NEHALEM_GMA_H__ */
