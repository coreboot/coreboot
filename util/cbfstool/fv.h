/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google, Inc.
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

#define FV_SIGNATURE 0x4856465f
typedef struct {
	uint8_t  padding[16];
	uint8_t  guid[16];
	uint64_t fv_length;
	uint32_t signature;
	uint32_t attributes;
	uint16_t header_length;
	uint16_t checksum;
	uint16_t ext_header_offs;
	uint8_t  reserved;
	uint8_t  revision;
	/* not used here: block map entries */
} firmware_volume_header_t;

#define FILETYPE_SEC 0x03
#define FILETYPE_PAD 0xf0
typedef struct {
	uint8_t  name[16];
	uint16_t integrity;
	uint8_t  file_type;
	uint8_t  attributes;
	uint8_t  size[3];
	uint8_t  state;
} ffs_file_header_t;

#define SECTION_PE32 0x10
#define SECTION_RAW 0x19
typedef struct {
	uint8_t size[3];
	uint8_t section_type;
} common_section_header_t;
