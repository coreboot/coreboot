/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <stdint.h>
#include <symbols.h>

#include "header.h"

struct config_headers {
	// The table of contents.
	struct configuration_header_toc_item toc_chsettings;
	struct configuration_header_toc_item toc_end;

	// An inert instance of chsettings.
	struct configuration_header_settings chsettings;
} __packed;

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
		.destination = (uintptr_t)_sram
	}
};
