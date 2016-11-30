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
 */

#include <stddef.h>
#include <stdint.h>
#include <symbols.h>

#include "header.h"

struct config_headers {
	// The table of contents.
	struct configuration_header_toc_item toc_chsettings;
	struct configuration_header_toc_item toc_end;

	// An inert instance of chsettings.
	struct configuration_header_settings chsettings;
} __attribute__((packed));

struct omap_image_headers {
	union {
		struct config_headers config_headers;
		uint8_t bytes[512];
	};
	struct gp_device_header image_header;
};

// A symbol which defines how much of the image the iROM should load.
extern char header_load_size;

struct omap_image_headers headers __attribute__((section(".header"))) = {
	.config_headers = {
		.toc_chsettings = {
			.start = offsetof(struct omap_image_headers,
					  config_headers.chsettings),
			.size = sizeof(struct configuration_header_settings),
			.reserved = { 0, 0, 0 },
			.filename = "CHSETTINGS\0"
		},
		.toc_end = {
			.start = 0xffffffff,
			.size = 0xffffffff,
			.reserved = { 0xffffffff, 0xffffffff, 0xffffffff },
			.filename = { 0xff, 0xff, 0xff, 0xff,
				      0xff, 0xff, 0xff, 0xff,
				      0xff, 0xff, 0xff, 0xff }
		},
		.chsettings = {
			.key = 0xc0c0c0c1,
			.valid = 0,
			.version = 1,
			.reserved = 0,
			.flags = 0
		}
	},
	.image_header = {
		.size = (uintptr_t)&header_load_size,
		.destination = (uintptr_t)_dram
	}
};
