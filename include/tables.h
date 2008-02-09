/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2002 Linux Networx
 * (Written by Eric Biederman <ebiederman@lnxi.com> for Linux Networx)
 * Copyright (C) 2005-2007 coresystems GmbH
 * (Written by Stefan Reinauer <stepan@coresystems.de> for coresystems GmbH)
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

#ifndef TABLES_H
#define TABLES_H

/*
 * Table management structs and prototypes for coreboot.
 *
 * ALL structs and prototypes for tables that coreboot generates should be
 * defined here. 
 */

struct lb_memory *write_tables(void);

/* The coreboot table information is for conveying information
 * from the firmware to the loaded OS image.  Primarily this
 * is expected to be information that cannot be discovered by
 * other means, such as quering the hardware directly.
 *
 * All of the information should be Position Independent Data.  
 * That is it should be safe to relocated any of the information
 * without it's meaning/correctnes changing.   For table that
 * can reasonably be used on multiple architectures the data
 * size should be fixed.  This should ease the transition between
 * 32 bit and 64 bit architectures etc.
 *
 * The completeness test for the information in this table is:
 * - Can all of the hardware be detected?
 * - Are the per motherboard constants available?
 * - Is there enough to allow a kernel to run that was written before
 *   a particular motherboard is constructed? (Assuming the kernel
 *   has drivers for all of the hardware but it does not have
 *   assumptions on how the hardware is connected together).
 *
 * With this test it should be straight forward to determine if a
 * table entry is required or not.  This should remove much of the
 * long term compatibility burden as table entries which are
 * irrelevant or have been replaced by better alternatives may be
 * dropped.  Of course it is polite and expidite to include extra
 * table entries and be backwards compatible, but it is not required.
 */

/* Since coreboot is usually compiled 32bit, gcc will align 64bit 
 * types to 32bit boundaries. If the coreboot table is dumped on a 
 * 64bit system, a u64 would be aligned to 64bit boundaries, 
 * breaking the table format.
 *
 * lb_uint64 will keep 64bit coreboot table values aligned to 32bit
 * to ensure compatibility. They can be accessed with the two functions
 * below: unpack_lb64() and pack_lb64()
 *
 * See also: util/lbtdump/lbtdump.c
 */

struct lb_uint64 {
	u32 lo;
	u32 hi;
};

static inline u64 unpack_lb64(struct lb_uint64 value)
{
        u64 result;
        result = value.hi;
        result = (result << 32) + value.lo;
        return result;
}

static inline struct lb_uint64 pack_lb64(u64 value)
{
        struct lb_uint64 result;
        result.lo = (value >> 0) & 0xffffffff;
        result.hi = (value >> 32) & 0xffffffff;
        return result;
}



struct lb_header
{
	u8  signature[4]; /* LBIO */
	u32 header_bytes;
	u32 header_checksum;
	u32 table_bytes;
	u32 table_checksum;
	u32 table_entries;
};

/* Every entry in the boot enviroment list will correspond to a boot
 * info record.  Encoding both type and size.  The type is obviously
 * so you can tell what it is.  The size allows you to skip that
 * boot enviroment record if you don't know what it easy.  This allows
 * forward compatibility with records not yet defined.
 */
struct lb_record {
	u32 tag;		/* tag ID */
	u32 size;		/* size of record (in bytes) */
};

#define LB_TAG_UNUSED	0x0000

#define LB_TAG_MEMORY	0x0001

struct lb_memory_range {
	struct lb_uint64 start;
	struct lb_uint64 size;
	u32 type;
#define LB_MEM_RAM       1	/* Memory anyone can use */
#define LB_MEM_RESERVED  2	/* Don't use this memory region */
#define LB_MEM_TABLE     16	/* Ram configuration tables are kept in */
};

struct lb_memory {
	u32 tag;
	u32 size;
	struct lb_memory_range map[0];
};

#define LB_TAG_HWRPB	0x0002
struct lb_hwrpb {
	u32 tag;
	u32 size;
	u64 hwrpb;
};

#define LB_TAG_MAINBOARD	0x0003
struct lb_mainboard {
	u32 tag;
	u32 size;
	u8  vendor_idx;
	u8  part_number_idx;
	u8  strings[0];
};

#define LB_TAG_VERSION		0x0004
#define LB_TAG_EXTRA_VERSION	0x0005
#define LB_TAG_BUILD		0x0006
#define LB_TAG_COMPILE_TIME	0x0007
#define LB_TAG_COMPILE_BY	0x0008
#define LB_TAG_COMPILE_HOST	0x0009
#define LB_TAG_COMPILE_DOMAIN	0x000a
#define LB_TAG_COMPILER		0x000b
#define LB_TAG_LINKER		0x000c
#define LB_TAG_ASSEMBLER	0x000d
struct lb_string {
	u32 tag;
	u32 size;
	u8  string[0];
};

#define LB_TAG_DEVTREE_PTR	0x000e

struct lb_devtree {
	u32 tag;
	u32 size;
	u32 dev_root_ptr; /* Pointer to the root device */
};

/* The following structures are for the cmos definitions table */
#define LB_TAG_CMOS_OPTION_TABLE 200
/* cmos header record */
struct cmos_option_table {
	u32 tag;               /* CMOS definitions table type */
	u32 size;               /* size of the entire table */
	u32 header_length;      /* length of header */
};

/* cmos entry record
        This record is variable length.  The name field may be
        shorter than CMOS_MAX_NAME_LENGTH. The entry may start
        anywhere in the byte, but can not span bytes unless it
        starts at the beginning of the byte and the length is
        fills complete bytes.
*/
#define LB_TAG_OPTION 201
struct cmos_entries {
	u32 tag;                /* entry type */
	u32 size;               /* length of this record */
	u32 bit;                /* starting bit from start of image */
	u32 length;             /* length of field in bits */
	u32 config;             /* e=enumeration, h=hex, r=reserved */
	u32 config_id;          /* a number linking to an enumeration record */
#define CMOS_MAX_NAME_LENGTH 32
	u8 name[CMOS_MAX_NAME_LENGTH]; /* name of entry in ascii, 
					       variable length int aligned */
};


/* cmos enumerations record
        This record is variable length.  The text field may be
        shorter than CMOS_MAX_TEXT_LENGTH.
*/
#define LB_TAG_OPTION_ENUM 202
struct cmos_enums {
	u32 tag;		     /* enumeration type */
	u32 size; 		     /* length of this record */
	u32 config_id;          /* a number identifying the config id */
	u32 value;              /* the value associated with the text */
#define CMOS_MAX_TEXT_LENGTH 32
	u8 text[CMOS_MAX_TEXT_LENGTH]; /* enum description in ascii, 
						variable length int aligned */
};

/* cmos defaults record
        This record contains default settings for the cmos ram.
*/
#define LB_TAG_OPTION_DEFAULTS 203
struct cmos_defaults {
	u32 tag;                /* default type */
	u32 size;               /* length of this record */
	u32 name_length;        /* length of the following name field */
	u8 name[CMOS_MAX_NAME_LENGTH]; /* name identifying the default */
#define CMOS_IMAGE_BUFFER_SIZE 128
	u8 default_set[CMOS_IMAGE_BUFFER_SIZE]; /* default settings */
};

#define LB_TAG_OPTION_CHECKSUM 204
struct	cmos_checksum {
	u32 tag;
	u32 size;
	/* In practice everything is byte aligned, but things are measured
	 * in bits to be consistent.
	 */
	u32 range_start;	/* First bit that is checksummed (byte aligned) */
	u32 range_end;	/* Last bit that is checksummed (byte aligned) */
	u32 location;	/* First bit of the checksum (byte aligned) */
	u32 type;		/* Checksum algorithm that is used */
#define CHECKSUM_NONE	0
#define CHECKSUM_PCBIOS	1
};

struct lb_memory *arch_write_tables(void);
unsigned long write_coreboot_table(
	unsigned long low_table_start, unsigned long low_table_end,
	unsigned long rom_table_start, unsigned long rom_table_end);

struct lb_header *lb_table_init(unsigned long addr);
struct lb_record *lb_first_record(struct lb_header *header);
struct lb_record *lb_last_record(struct lb_header *header);
struct lb_record *lb_next_record(struct lb_record *rec);
struct lb_record *lb_new_record(struct lb_header *header);
struct lb_memory *lb_memory(struct lb_header *header);
struct lb_mainboard *lb_mainboard(struct lb_header *header);
unsigned long lb_table_fini(struct lb_header *header);

/* Routines to extract part so the coreboot table or information
 * from the coreboot table.
 */
struct lb_memory *get_lb_mem(void);

struct cmos_option_table *get_option_table(void);

#ifdef CONFIG_PIRQ_TABLE
unsigned long write_pirq_routing_table(unsigned long addr);
#endif


#endif /* TABLES_H */
