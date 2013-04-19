/*
 * This file is part of the libpayload project.
 *
 * Copyright (C) 2008 Advanced Micro Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef _COREBOOT_TABLES_H
#define _COREBOOT_TABLES_H

#include <arch/types.h>
#include <ipchksum.h>

struct cbuint64 {
	u32 lo;
	u32 hi;
};

struct cb_header {
	u8 signature[4];
	u32 header_bytes;
	u32 header_checksum;
	u32 table_bytes;
	u32 table_checksum;
	u32 table_entries;
};

struct cb_record {
	u32 tag;
	u32 size;
};

#define CB_TAG_UNUSED     0x0000
#define CB_TAG_MEMORY     0x0001

struct cb_memory_range {
	struct cbuint64 start;
	struct cbuint64 size;
	u32 type;
};

#define CB_MEM_RAM          1
#define CB_MEM_RESERVED     2
#define CB_MEM_ACPI         3
#define CB_MEM_NVS          4
#define CB_MEM_UNUSABLE     5
#define CB_MEM_VENDOR_RSVD  6
#define CB_MEM_TABLE       16

struct cb_memory {
	u32 tag;
	u32 size;
	struct cb_memory_range map[0];
};

#define CB_TAG_HWRPB      0x0002

struct cb_hwrpb {
	u32 tag;
	u32 size;
	u64 hwrpb;
};

#define CB_TAG_MAINBOARD  0x0003

struct cb_mainboard {
	u32 tag;
	u32 size;
	u8 vendor_idx;
	u8 part_number_idx;
	u8 strings[0];
};

#define CB_TAG_VERSION        0x0004
#define CB_TAG_EXTRA_VERSION  0x0005
#define CB_TAG_BUILD          0x0006
#define CB_TAG_COMPILE_TIME   0x0007
#define CB_TAG_COMPILE_BY     0x0008
#define CB_TAG_COMPILE_HOST   0x0009
#define CB_TAG_COMPILE_DOMAIN 0x000a
#define CB_TAG_COMPILER       0x000b
#define CB_TAG_LINKER         0x000c
#define CB_TAG_ASSEMBLER      0x000d

struct cb_string {
	u32 tag;
	u32 size;
	u8 string[0];
};

#define CB_TAG_SERIAL         0x000f

struct cb_serial {
	u32 tag;
	u32 size;
#define CB_SERIAL_TYPE_IO_MAPPED     1
#define CB_SERIAL_TYPE_MEMORY_MAPPED 2
	u32 type;
	u32 baseaddr;
	u32 baud;
};

#define CB_TAG_CONSOLE       0x00010

struct cb_console {
	u32 tag;
	u32 size;
	u16 type;
};

#define CB_TAG_CONSOLE_SERIAL8250 0
#define CB_TAG_CONSOLE_VGA        1 // OBSOLETE
#define CB_TAG_CONSOLE_BTEXT      2 // OBSOLETE
#define CB_TAG_CONSOLE_LOGBUF     3
#define CB_TAG_CONSOLE_SROM       4 // OBSOLETE
#define CB_TAG_CONSOLE_EHCI       5

#define CB_TAG_FORWARD       0x00011

struct cb_forward {
	u32 tag;
	u32 size;
	u64 forward;
};

#define CB_TAG_FRAMEBUFFER      0x0012
struct cb_framebuffer {
	u32 tag;
	u32 size;

	u64 physical_address;
	u32 x_resolution;
	u32 y_resolution;
	u32 bytes_per_line;
	u8 bits_per_pixel;
        u8 red_mask_pos;
	u8 red_mask_size;
	u8 green_mask_pos;
	u8 green_mask_size;
	u8 blue_mask_pos;
	u8 blue_mask_size;
	u8 reserved_mask_pos;
	u8 reserved_mask_size;
};

#define CB_TAG_GPIO 0x0013
#define CB_GPIO_ACTIVE_LOW 0
#define CB_GPIO_ACTIVE_HIGH 1
#define CB_GPIO_MAX_NAME_LENGTH 16
struct cb_gpio {
	u32 port;
	u32 polarity;
	u32 value;
	u8 name[CB_GPIO_MAX_NAME_LENGTH];
};

struct cb_gpios {
	u32 tag;
	u32 size;

	u32 count;
	struct cb_gpio gpios[0];
};

#define CB_TAG_VDAT	0x0015
struct cb_vdat {
	uint32_t tag;
	uint32_t size;	/* size of the entire entry */
	uint64_t vdat_addr;
	uint32_t vdat_size;
};

#define CB_TAG_TIMESTAMPS	0x0016
#define CB_TAG_CBMEM_CONSOLE	0x0017
#define CB_TAG_MRC_CACHE	0x0018
struct cb_cbmem_tab {
	uint32_t tag;
	uint32_t size;
	uint64_t cbmem_tab;
};

#define CB_TAG_VBNV		0x0019
struct cb_vbnv {
	uint32_t tag;
	uint32_t size;
	uint32_t vbnv_start;
	uint32_t vbnv_size;
};

#define CB_TAG_VBOOT_HANDOFF	0x0020
struct cb_vboot_handoff {
	uint32_t tag;
	uint32_t size;
	uint64_t vboot_handoff_addr;
	uint32_t vboot_handoff_size;
};

#define CB_TAG_X86_ROM_MTRR	0x0021
struct cb_x86_rom_mtrr {
	uint32_t tag;
	uint32_t size;
	/* The variable range MTRR index covering the ROM. If one wants to
	 * enable caching the ROM, the variable MTRR needs to be set to
	 * write-protect. To disable the caching after enabling set the
	 * type to uncacheable. */
	uint32_t index;
};


#define CB_TAG_CMOS_OPTION_TABLE 0x00c8
struct cb_cmos_option_table {
	u32 tag;
	u32 size;
	u32 header_length;
};

#define CB_TAG_OPTION         0x00c9
#define CB_CMOS_MAX_NAME_LENGTH    32
struct cb_cmos_entries {
	u32 tag;
	u32 size;
	u32 bit;
	u32 length;
	u32 config;
	u32 config_id;
	u8 name[CB_CMOS_MAX_NAME_LENGTH];
};


#define CB_TAG_OPTION_ENUM    0x00ca
#define CB_CMOS_MAX_TEXT_LENGTH 32
struct cb_cmos_enums {
	u32 tag;
	u32 size;
	u32 config_id;
	u32 value;
	u8 text[CB_CMOS_MAX_TEXT_LENGTH];
};

#define CB_TAG_OPTION_DEFAULTS 0x00cb
#define CB_CMOS_IMAGE_BUFFER_SIZE 128
struct cb_cmos_defaults {
	u32 tag;
	u32 size;
	u32 name_length;
	u8 name[CB_CMOS_MAX_NAME_LENGTH];
	u8 default_set[CB_CMOS_IMAGE_BUFFER_SIZE];
};

#define CB_TAG_OPTION_CHECKSUM 0x00cc
#define CB_CHECKSUM_NONE	0
#define CB_CHECKSUM_PCBIOS	1
struct	cb_cmos_checksum {
	u32 tag;
	u32 size;
	u32 range_start;
	u32 range_end;
	u32 location;
	u32 type;
};

/* Helpful inlines */

static inline u64 cb_unpack64(struct cbuint64 val)
{
	return (((u64) val.hi) << 32) | val.lo;
}

static inline u16 cb_checksum(const void *ptr, unsigned len)
{
	return ipchksum(ptr, len);
}

static inline const char *cb_mb_vendor_string(const struct cb_mainboard *cbm)
{
	return (char *)(cbm->strings + cbm->vendor_idx);
}

static inline const char *cb_mb_part_string(const struct cb_mainboard *cbm)
{
	return (char *)(cbm->strings + cbm->part_number_idx);
}

/* Helpful macros */

#define MEM_RANGE_COUNT(_rec) \
	(((_rec)->size - sizeof(*(_rec))) / sizeof((_rec)->map[0]))

#define MEM_RANGE_PTR(_rec, _idx) \
	(void *)(((u8 *) (_rec)) + sizeof(*(_rec)) \
		+ (sizeof((_rec)->map[0]) * (_idx)))

#endif
