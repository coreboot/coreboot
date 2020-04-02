/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#ifndef ELOG_INTERNAL_H_
#define ELOG_INTERNAL_H_

/* ELOG header */
struct elog_header {
	u32 magic;
	u8 version;
	u8 header_size;
	u8 reserved[2];
} __packed;

/* ELOG related constants */
#define ELOG_SIGNATURE			0x474f4c45  /* 'ELOG' */
#define ELOG_VERSION			1
#define ELOG_MIN_AVAILABLE_ENTRIES	2  /* Shrink when this many can't fit */
#define ELOG_SHRINK_PERCENTAGE		25 /* Percent of total area to remove */

/* SMBIOS event log header */
struct event_header {
	u8 type;
	u8 length;
	u8 year;
	u8 month;
	u8 day;
	u8 hour;
	u8 minute;
	u8 second;
} __packed;

/* SMBIOS Type 15 related constants */
#define ELOG_HEADER_TYPE_OEM		0x88

#endif /* ELOG_INTERNAL_H_ */
