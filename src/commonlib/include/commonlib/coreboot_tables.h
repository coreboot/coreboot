/*
 * This file is part of the coreboot project.
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

#ifndef COMMONLIB_COREBOOT_TABLES_H
#define COMMONLIB_COREBOOT_TABLES_H

#include <stdint.h>

/* The coreboot table information is for conveying information
 * from the firmware to the loaded OS image.  Primarily this
 * is expected to be information that cannot be discovered by
 * other means, such as querying the hardware directly.
 *
 * All of the information should be Position Independent Data.
 * That is it should be safe to relocated any of the information
 * without it's meaning/correctness changing.   For table that
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
 * dropped.  Of course it is polite and expedite to include extra
 * table entries and be backwards compatible, but it is not required.
 */

/* Since coreboot is usually compiled 32bit, gcc will align 64bit
 * types to 32bit boundaries. If the coreboot table is dumped on a
 * 64bit system, a uint64_t would be aligned to 64bit boundaries,
 * breaking the table format.
 *
 * lb_uint64 will keep 64bit coreboot table values aligned to 32bit
 * to ensure compatibility. They can be accessed with the two functions
 * below: unpack_lb64() and pack_lb64()
 *
 * See also: util/lbtdump/lbtdump.c
 */

struct lb_uint64 {
	uint32_t lo;
	uint32_t hi;
};

static inline uint64_t unpack_lb64(struct lb_uint64 value)
{
	uint64_t result;
	result = value.hi;
	result = (result << 32) + value.lo;
	return result;
}

static inline struct lb_uint64 pack_lb64(uint64_t value)
{
	struct lb_uint64 result;
	result.lo = (value >> 0) & 0xffffffff;
	result.hi = (value >> 32) & 0xffffffff;
	return result;
}

struct lb_header
{
	uint8_t  signature[4]; /* LBIO */
	uint32_t header_bytes;
	uint32_t header_checksum;
	uint32_t table_bytes;
	uint32_t table_checksum;
	uint32_t table_entries;
};

/* Every entry in the boot environment list will correspond to a boot
 * info record.  Encoding both type and size.  The type is obviously
 * so you can tell what it is.  The size allows you to skip that
 * boot environment record if you don't know what it is.  This allows
 * forward compatibility with records not yet defined.
 */
struct lb_record {
	uint32_t tag;		/* tag ID */
	uint32_t size;		/* size of record (in bytes) */
};

#define LB_TAG_UNUSED		0x0000

#define LB_TAG_MEMORY		0x0001

struct lb_memory_range {
	struct lb_uint64 start;
	struct lb_uint64 size;
	uint32_t type;
#define LB_MEM_RAM		 1	/* Memory anyone can use */
#define LB_MEM_RESERVED		 2	/* Don't use this memory region */
#define LB_MEM_ACPI		 3	/* ACPI Tables */
#define LB_MEM_NVS		 4	/* ACPI NVS Memory */
#define LB_MEM_UNUSABLE		 5	/* Unusable address space */
#define LB_MEM_VENDOR_RSVD	 6	/* Vendor Reserved */
#define LB_MEM_TABLE		16	/* Ram configuration tables are kept in */
};

struct lb_memory {
	uint32_t tag;
	uint32_t size;
	struct lb_memory_range map[0];
};

#define LB_TAG_HWRPB		0x0002
struct lb_hwrpb {
	uint32_t tag;
	uint32_t size;
	uint64_t hwrpb;
};

#define LB_TAG_MAINBOARD	0x0003
struct lb_mainboard {
	uint32_t tag;
	uint32_t size;
	uint8_t  vendor_idx;
	uint8_t  part_number_idx;
	uint8_t  strings[0];
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
	uint32_t tag;
	uint32_t size;
	uint8_t  string[0];
};

#define LB_TAG_VERSION_TIMESTAMP	0x0026
struct lb_timestamp {
	uint32_t tag;
	uint32_t size;
	uint32_t timestamp;
};


/* 0xe is taken by v3 */

#define LB_TAG_SERIAL		0x000f
struct lb_serial {
	uint32_t tag;
	uint32_t size;
#define LB_SERIAL_TYPE_IO_MAPPED     1
#define LB_SERIAL_TYPE_MEMORY_MAPPED 2
	uint32_t type;
	uint32_t baseaddr;
	uint32_t baud;
	uint32_t regwidth;

	/* Crystal or input frequency to the chip containing the UART.
	 * Provide the board specific details to allow the payload to
	 * initialize the chip containing the UART and make independent
	 * decisions as to which dividers to select and their values
	 * to eventually arrive at the desired console baud-rate. */
	uint32_t input_hertz;

	/* UART PCI address: bus, device, function
	 * 1 << 31 - Valid bit, PCI UART in use
	 * Bus << 20
	 * Device << 15
	 * Function << 12
	 */
	uint32_t uart_pci_addr;
};

#define LB_TAG_CONSOLE		0x0010
struct lb_console {
	uint32_t tag;
	uint32_t size;
	uint16_t type;
};

#define LB_TAG_CONSOLE_SERIAL8250	0
#define LB_TAG_CONSOLE_VGA		1 // OBSOLETE
#define LB_TAG_CONSOLE_BTEXT		2 // OBSOLETE
#define LB_TAG_CONSOLE_LOGBUF		3 // OBSOLETE
#define LB_TAG_CONSOLE_SROM		4 // OBSOLETE
#define LB_TAG_CONSOLE_EHCI		5
#define LB_TAG_CONSOLE_SERIAL8250MEM	6

#define LB_TAG_FORWARD		0x0011
struct lb_forward {
	uint32_t tag;
	uint32_t size;
	uint64_t forward;
};

#define LB_TAG_FRAMEBUFFER	0x0012
struct lb_framebuffer {
	uint32_t tag;
	uint32_t size;

	uint64_t physical_address;
	uint32_t x_resolution;
	uint32_t y_resolution;
	uint32_t bytes_per_line;
	uint8_t bits_per_pixel;
	uint8_t red_mask_pos;
	uint8_t red_mask_size;
	uint8_t green_mask_pos;
	uint8_t green_mask_size;
	uint8_t blue_mask_pos;
	uint8_t blue_mask_size;
	uint8_t reserved_mask_pos;
	uint8_t reserved_mask_size;
};

#define LB_TAG_GPIO	0x0013

struct lb_gpio {
	uint32_t port;
	uint32_t polarity;
#define ACTIVE_LOW	0
#define ACTIVE_HIGH	1
	uint32_t value;
#define GPIO_MAX_NAME_LENGTH 16
        uint8_t name[GPIO_MAX_NAME_LENGTH];
};

struct lb_gpios {
	uint32_t tag;
	uint32_t size;

	uint32_t count;
	struct lb_gpio gpios[0];
};

#define LB_TAG_VDAT		0x0015
#define LB_TAG_VBNV		0x0019
#define LB_TAB_VBOOT_HANDOFF	0x0020
#define LB_TAB_DMA		0x0022
#define LB_TAG_RAM_OOPS		0x0023
#define LB_TAG_MTC		0x002b
struct lb_range {
	uint32_t tag;
	uint32_t size;

	uint64_t range_start;
	uint32_t range_size;
};

void lb_ramoops(struct lb_header *header);

#define LB_TAG_TIMESTAMPS	0x0016
#define LB_TAG_CBMEM_CONSOLE	0x0017
#define LB_TAG_MRC_CACHE	0x0018
#define LB_TAG_ACPI_GNVS	0x0024
#define LB_TAG_WIFI_CALIBRATION	0x0027
#define LB_TAG_VPD		0x002c
struct lb_cbmem_ref {
	uint32_t tag;
	uint32_t size;

	uint64_t cbmem_addr;
};

#define LB_TAG_X86_ROM_MTRR	0x0021
struct lb_x86_rom_mtrr {
	uint32_t tag;
	uint32_t size;
	/* The variable range MTRR index covering the ROM. */
	uint32_t index;
};

#define LB_TAG_BOARD_ID		0x0025
struct lb_board_id {
	uint32_t tag;
	uint32_t size;
	/* Board ID as retrieved from the board revision GPIOs. */
	uint32_t board_id;
};

#define LB_TAG_MAC_ADDRS	0x0026
struct mac_address {
	uint8_t mac_addr[6];
	uint8_t pad[2];		/* Pad it to 8 bytes to keep it simple. */
};

struct lb_macs {
	uint32_t tag;
	uint32_t size;
	uint32_t count;
	struct mac_address mac_addrs[0];
};

#define LB_TAG_RAM_CODE		0x0028
struct lb_ram_code {
	uint32_t tag;
	uint32_t size;
	uint32_t ram_code;
};

#define LB_TAG_SPI_FLASH	0x0029
struct lb_spi_flash {
	uint32_t tag;
	uint32_t size;
	uint32_t flash_size;
	uint32_t sector_size;
	uint32_t erase_cmd;
};

#define LB_TAG_BOOT_MEDIA_PARAMS 0x0030
struct lb_boot_media_params {
	uint32_t tag;
	uint32_t size;
	/* offsets are relative to start of boot media */
	uint64_t fmap_offset;
	uint64_t cbfs_offset;
	uint64_t cbfs_size;
	uint64_t boot_media_size;
};

/*
 * There can be more than one of these records as there is one per cbmem entry.
 */
#define LB_TAG_CBMEM_ENTRY 0x0031
struct lb_cbmem_entry {
	uint32_t tag;
	uint32_t size;

	uint64_t address;
	uint32_t entry_size;
	uint32_t id;
};

#define LB_TAG_TSC_INFO 0x0032
struct lb_tsc_info {
	uint32_t tag;
	uint32_t size;

	uint32_t freq_khz;
};

#define LB_TAG_SERIALNO		0x002a
#define MAX_SERIALNO_LENGTH	32

/* The following structures are for the cmos definitions table */
#define LB_TAG_CMOS_OPTION_TABLE 200
/* cmos header record */
struct cmos_option_table {
	uint32_t tag;               /* CMOS definitions table type */
	uint32_t size;               /* size of the entire table */
	uint32_t header_length;      /* length of header */
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
	uint32_t tag;                /* entry type */
	uint32_t size;               /* length of this record */
	uint32_t bit;                /* starting bit from start of image */
	uint32_t length;             /* length of field in bits */
	uint32_t config;             /* e=enumeration, h=hex, r=reserved */
	uint32_t config_id;          /* a number linking to an enumeration record */
#define CMOS_MAX_NAME_LENGTH 32
	uint8_t name[CMOS_MAX_NAME_LENGTH]; /* name of entry in ascii,
					       variable length int aligned */
};


/* cmos enumerations record
        This record is variable length.  The text field may be
        shorter than CMOS_MAX_TEXT_LENGTH.
*/
#define LB_TAG_OPTION_ENUM 202
struct cmos_enums {
	uint32_t tag;		     /* enumeration type */
	uint32_t size; 		     /* length of this record */
	uint32_t config_id;          /* a number identifying the config id */
	uint32_t value;              /* the value associated with the text */
#define CMOS_MAX_TEXT_LENGTH 32
	uint8_t text[CMOS_MAX_TEXT_LENGTH]; /* enum description in ascii,
						variable length int aligned */
};

/* cmos defaults record
        This record contains default settings for the cmos ram.
*/
#define LB_TAG_OPTION_DEFAULTS 203
struct cmos_defaults {
	uint32_t tag;                /* default type */
	uint32_t size;               /* length of this record */
	uint32_t name_length;        /* length of the following name field */
	uint8_t name[CMOS_MAX_NAME_LENGTH]; /* name identifying the default */
#define CMOS_IMAGE_BUFFER_SIZE 256
	uint8_t default_set[CMOS_IMAGE_BUFFER_SIZE]; /* default settings */
};

#define LB_TAG_OPTION_CHECKSUM 204
struct	cmos_checksum {
	uint32_t tag;
	uint32_t size;
	/* In practice everything is byte aligned, but things are measured
	 * in bits to be consistent.
	 */
	uint32_t range_start;	/* First bit that is checksummed (byte aligned) */
	uint32_t range_end;	/* Last bit that is checksummed (byte aligned) */
	uint32_t location;	/* First bit of the checksum (byte aligned) */
	uint32_t type;		/* Checksum algorithm that is used */
#define CHECKSUM_NONE	0
#define CHECKSUM_PCBIOS	1
};

#endif
