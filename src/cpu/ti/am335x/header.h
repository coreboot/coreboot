/*
 * Copyright (C) 2013 Google Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifndef __CPU_TI_AM335X_HEADER_H
#define __CPU_TI_AM335X_HEADER_H

#include <stdint.h>

struct configuration_header_toc_item {
	// Offset from the start address of the TOC to the actual address of
	// a section.
	uint32_t start;

	// Size of a section.
	uint32_t size;

	// Reserved.
	uint32_t reserved[3];

	// 12-character name of a section, including the zero (\0) terminator.
	char filename[12];
} __attribute__((packed));

struct configuration_header_settings {
	// Key used for section verification.
	uint32_t key;

	// Enables or disables the section.
	// 00h: Disable.
	// Other: Enable.
	uint8_t valid;

	// Configuration header version.
	uint8_t version;

	// Reserved.
	uint16_t reserved;

	// Flags. It's not clear what this is used for.
	uint32_t flags;
} __attribute__((packed));

struct gp_device_header {
	// Size of the image.
	uint32_t size;

	// Address to store the image/code entry point.
	uint32_t destination;
} __attribute__((packed));

#endif
