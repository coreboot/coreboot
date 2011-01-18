/*
 * Copyright (C) 2009 coresystems GmbH
 *                 written by Patrick Georgi <patrick.georgi@coresystems.de>
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

#include <stdint.h>

struct cbfs_header {
	uint32_t magic;
	uint32_t version;
	uint32_t romsize;
	uint32_t bootblocksize;
	uint32_t align;
	uint32_t offset;
	uint32_t pad[2];
} __attribute__ ((packed));

struct cbfs_file {
	char magic[8];
	uint32_t len;
	uint32_t type;
	uint32_t checksum;
	uint32_t offset;
} __attribute__ ((packed));

struct cbfs_stage {
	unsigned int compression;
	unsigned long long entry;
	unsigned long long load;
	unsigned int len;
	unsigned int memlen;
} __attribute__ ((packed));

#define PAYLOAD_SEGMENT_CODE	0x45444F43
#define PAYLOAD_SEGMENT_DATA	0x41544144
#define PAYLOAD_SEGMENT_BSS	0x20535342
#define PAYLOAD_SEGMENT_PARAMS	0x41524150
#define PAYLOAD_SEGMENT_ENTRY	0x52544E45

struct cbfs_payload_segment {
	unsigned int type;
	unsigned int compression;
	unsigned int offset;
	unsigned long long load_addr;
	unsigned int len;
	unsigned int mem_len;
} __attribute__ ((packed));

struct cbfs_payload {
	struct cbfs_payload_segment segments;
} __attribute__ ((packed));

/** These are standard component types for well known
    components (i.e - those that coreboot needs to consume.
    Users are welcome to use any other value for their
    components */

#define CBFS_COMPONENT_STAGE      0x10
#define CBFS_COMPONENT_PAYLOAD    0x20
#define CBFS_COMPONENT_OPTIONROM  0x30
#define CBFS_COMPONENT_BOOTSPLASH 0x40
#define CBFS_COMPONENT_RAW        0x50
#define CBFS_COMPONENT_VSA        0x51
#define CBFS_COMPONENT_MBI        0x52
#define CBFS_COMPONENT_MICROCODE  0x53
#define CBFS_COMPONENT_CMOS_DEFAULT 0xaa
#define CBFS_COMPONENT_CMOS_LAYOUT 0x01aa

/* The deleted type is chosen to be a value
 * that can be written in a FLASH from all other
 * values.
 */
#define CBFS_COMPONENT_DELETED 0

/* for all known FLASH, this value can be changed
 * to all other values. This allows NULL files to be
 * changed without a block erase
 */
#define CBFS_COMPONENT_NULL 0xFFFFFFFF

int cbfs_file_header(uint32_t physaddr);
struct cbfs_file *cbfs_create_empty_file(uint32_t physaddr, uint32_t size);
