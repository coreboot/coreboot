/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __SOC_TI_AM335X_HEADER_H
#define __SOC_TI_AM335X_HEADER_H

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
} __packed;

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
} __packed;

struct gp_device_header {
	// Size of the image.
	uint32_t size;

	// Address to store the image/code entry point.
	uint32_t destination;
} __packed;

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

#endif
