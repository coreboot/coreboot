/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 The ChromiumOS Authors.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA, 02110-1301 USA
 */

#ifndef ELOG_INTERNAL_H_
#define ELOG_INTERNAL_H_

/* ELOG header */
struct elog_header {
	u32 magic;
	u8 version;
	u8 header_size;
	u8 reserved[2];
} __attribute__ ((packed));

/* ELOG related constants */
#define ELOG_SIGNATURE			0x474f4c45  /* 'ELOG' */
#define ELOG_VERSION			1

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
} __attribute__ ((packed));

/* SMBIOS Type 15 related constants */
#define ELOG_HEADER_TYPE_OEM		0x88

typedef enum elog_descriptor_type {
	ELOG_DESCRIPTOR_UNKNOWN,
	ELOG_DESCRIPTOR_MEMORY,
	ELOG_DESCRIPTOR_FLASH,
} elog_descriptor_type;

typedef enum elog_area_state {
	ELOG_AREA_UNDEFINED,		/* Initial boot strap state */
	ELOG_AREA_EMPTY,		/* Entire area is empty */
	ELOG_AREA_HAS_CONTENT,		/* Area has some content */
} elog_area_state;

typedef enum elog_header_state {
	ELOG_HEADER_INVALID,
	ELOG_HEADER_VALID,
} elog_header_state;

typedef enum elog_event_buffer_state {
	ELOG_EVENT_BUFFER_OK,
	ELOG_EVENT_BUFFER_CORRUPTED,
} elog_event_buffer_state;

/*
 * Internal handler for event log buffers
 */
struct elog_descriptor {
	elog_descriptor_type	type;
	elog_area_state		area_state;
	elog_header_state	header_state;
	elog_event_buffer_state	event_buffer_state;
	struct elog_header	*staging_header;
	void			*backing_store;
	u8			*data;
	u32			flash_base;
	u16                     total_size;
	u16			data_size;
	u16			next_event_offset;
	u16			last_event_offset;
	u16			last_event_size;
	u16			event_count;
};

#endif /* ELOG_INTERNAL_H_ */
