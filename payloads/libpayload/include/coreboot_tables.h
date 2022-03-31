/*
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
#include <stdint.h>

enum {
	CB_TAG_UNUSED			= 0x0000,
	CB_TAG_MEMORY			= 0x0001,
	CB_TAG_HWRPB			= 0x0002,
	CB_TAG_MAINBOARD		= 0x0003,
	CB_TAG_VERSION			= 0x0004,
	CB_TAG_EXTRA_VERSION		= 0x0005,
	CB_TAG_BUILD			= 0x0006,
	CB_TAG_COMPILE_TIME		= 0x0007,
	CB_TAG_COMPILE_BY		= 0x0008,
	CB_TAG_COMPILE_HOST		= 0x0009,
	CB_TAG_COMPILE_DOMAIN		= 0x000a,
	CB_TAG_COMPILER			= 0x000b,
	CB_TAG_LINKER			= 0x000c,
	CB_TAG_ASSEMBLER		= 0x000d,
	CB_TAG_SERIAL			= 0x000f,
	CB_TAG_CONSOLE			= 0x0010,
	CB_TAG_FORWARD			= 0x0011,
	CB_TAG_FRAMEBUFFER		= 0x0012,
	CB_TAG_GPIO			= 0x0013,
	CB_TAG_TIMESTAMPS		= 0x0016,
	CB_TAG_CBMEM_CONSOLE		= 0x0017,
	CB_TAG_MRC_CACHE		= 0x0018,
	CB_TAG_VBNV			= 0x0019,
	CB_TAG_VBOOT_HANDOFF		= 0x0020,  /* deprecated */
	CB_TAG_X86_ROM_MTRR		= 0x0021,
	CB_TAG_DMA			= 0x0022,
	CB_TAG_RAM_OOPS			= 0x0023,
	CB_TAG_ACPI_GNVS		= 0x0024,
	CB_TAG_BOARD_ID			= 0x0025,
	CB_TAG_VERSION_TIMESTAMP	= 0x0026,
	CB_TAG_WIFI_CALIBRATION		= 0x0027,
	CB_TAG_RAM_CODE			= 0x0028,
	CB_TAG_SPI_FLASH		= 0x0029,
	CB_TAG_SERIALNO			= 0x002a,
	CB_TAG_MTC			= 0x002b,
	CB_TAG_VPD			= 0x002c,
	CB_TAG_SKU_ID			= 0x002d,
	CB_TAG_BOOT_MEDIA_PARAMS	= 0x0030,
	CB_TAG_CBMEM_ENTRY		= 0x0031,
	CB_TAG_TSC_INFO			= 0x0032,
	CB_TAG_MAC_ADDRS		= 0x0033,
	CB_TAG_VBOOT_WORKBUF		= 0x0034,
	CB_TAG_MMC_INFO			= 0x0035,
	CB_TAG_TCPA_LOG			= 0x0036,
	CB_TAG_FMAP			= 0x0037,
	CB_TAG_SMMSTOREV2		= 0x0039,
	CB_TAG_BOARD_CONFIG		= 0x0040,
	CB_TAG_ACPI_CNVS		= 0x0041,
	CB_TAG_TYPE_C_INFO		= 0x0042,
	CB_TAG_ACPI_RSDP                = 0x0043,
	CB_TAG_PCIE			= 0x0044,
	CB_TAG_CMOS_OPTION_TABLE	= 0x00c8,
	CB_TAG_OPTION			= 0x00c9,
	CB_TAG_OPTION_ENUM		= 0x00ca,
	CB_TAG_OPTION_DEFAULTS		= 0x00cb,
	CB_TAG_OPTION_CHECKSUM		= 0x00cc,
};

typedef __aligned(4) uint64_t cb_uint64_t;

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

struct cb_memory_range {
	cb_uint64_t start;
	cb_uint64_t size;
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

struct cb_hwrpb {
	u32 tag;
	u32 size;
	u64 hwrpb;
};

struct cb_mainboard {
	u32 tag;
	u32 size;
	u8 vendor_idx;
	u8 part_number_idx;
	u8 strings[0];
};

enum type_c_orientation {
	TYPEC_ORIENTATION_NONE,
	TYPEC_ORIENTATION_NORMAL,
	TYPEC_ORIENTATION_REVERSE,
};

struct type_c_port_info {
	/*
	 * usb2_port_number and usb3_port_number are expected to be
	 * the port numbers as seen by the USB controller in the SoC.
	 */
	uint8_t usb2_port_number;
	uint8_t usb3_port_number;

	/*
	 * Valid sbu_orientation and data_orientation values will be of
	 * type enum type_c_orienation.
	 */
	uint8_t sbu_orientation;
	uint8_t data_orientation;
};

struct type_c_info {
	u32 port_count;
	struct type_c_port_info port_info[0];
};

struct cb_string {
	u32 tag;
	u32 size;
	u8 string[0];
};

struct cb_serial {
	u32 tag;
	u32 size;
#define CB_SERIAL_TYPE_IO_MAPPED     1
#define CB_SERIAL_TYPE_MEMORY_MAPPED 2
	u32 type;
	u32 baseaddr;
	u32 baud;
	u32 regwidth;

	/* Crystal or input frequency to the chip containing the UART.
	 * Provide the board specific details to allow the payload to
	 * initialize the chip containing the UART and make independent
	 * decisions as to which dividers to select and their values
	 * to eventually arrive at the desired console baud-rate. */
	u32 input_hertz;

	/* UART PCI address: bus, device, function
	 * 1 << 31 - Valid bit, PCI UART in use
	 * Bus << 20
	 * Device << 15
	 * Function << 12
	 */
	u32 uart_pci_addr;
};

struct cb_console {
	u32 tag;
	u32 size;
	u16 type;
};

#define CB_TAG_CONSOLE_SERIAL8250 0
#define CB_TAG_CONSOLE_VGA        1 // OBSOLETE
#define CB_TAG_CONSOLE_BTEXT      2 // OBSOLETE
#define CB_TAG_CONSOLE_LOGBUF     3 // OBSOLETE
#define CB_TAG_CONSOLE_SROM       4 // OBSOLETE
#define CB_TAG_CONSOLE_EHCI       5

struct cb_forward {
	u32 tag;
	u32 size;
	u64 forward;
};

/* Panel orientation, matches drm_connector.h in the Linux kernel. */
enum cb_fb_orientation {
	CB_FB_ORIENTATION_NORMAL = 0,
	CB_FB_ORIENTATION_BOTTOM_UP = 1,
	CB_FB_ORIENTATION_LEFT_UP = 2,
	CB_FB_ORIENTATION_RIGHT_UP = 3,
};

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
	u8 orientation;
};

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

struct cb_pcie {
	uint32_t tag;
	uint32_t size;
	cb_uint64_t ctrl_base;	/* Base address of PCIe controller */
};

struct lb_range {
	uint32_t tag;
	uint32_t size;
	cb_uint64_t range_start;
	uint32_t range_size;
};

struct cb_cbmem_tab {
	uint32_t tag;
	uint32_t size;
	cb_uint64_t cbmem_tab;
};

struct cb_x86_rom_mtrr {
	uint32_t tag;
	uint32_t size;
	/* The variable range MTRR index covering the ROM. If one wants to
	 * enable caching the ROM, the variable MTRR needs to be set to
	 * write-protect. To disable the caching after enabling set the
	 * type to uncacheable. */
	uint32_t index;
};

/* Memory map windows to translate addresses between SPI flash space and host address space. */
struct flash_mmap_window {
	uint32_t flash_base;
	uint32_t host_base;
	uint32_t size;
};

struct cb_spi_flash {
	uint32_t tag;
	uint32_t size;
	uint32_t flash_size;
	uint32_t sector_size;
	uint32_t erase_cmd;
	/*
	 * Number of mmap windows used by the platform to decode addresses between SPI flash
	 * space and host address space. This determines the number of entries in mmap_table.
	 */
	uint32_t mmap_count;
	struct flash_mmap_window mmap_table[0];
};

struct cb_boot_media_params {
	uint32_t tag;
	uint32_t size;
	/* offsets are relative to start of boot media */
	cb_uint64_t fmap_offset;
	cb_uint64_t cbfs_offset;
	cb_uint64_t cbfs_size;
	cb_uint64_t boot_media_size;
};


struct cb_cbmem_entry {
	uint32_t tag;
	uint32_t size;

	cb_uint64_t address;
	uint32_t entry_size;
	uint32_t id;
};

struct cb_tsc_info {
	uint32_t tag;
	uint32_t size;

	uint32_t freq_khz;
};

struct mac_address {
	uint8_t mac_addr[6];
	uint8_t pad[2];         /* Pad it to 8 bytes to keep it simple. */
};

struct cb_macs {
	uint32_t tag;
	uint32_t size;
	uint32_t count;
	struct mac_address mac_addrs[0];
};

struct cb_mmc_info {
	uint32_t tag;
	uint32_t size;
	/*
	 * Passes the early mmc status to payload to indicate if firmware
	 * successfully sent CMD0, CMD1 to the card or not. In case of
	 * success, the payload can skip the first step of the initialization
	 * sequence which is to send CMD0, and instead start by sending CMD1
	 * as described in Jedec Standard JESD83-B1 section 6.4.3.
	 * passes 1 on success
	 */
	int32_t early_cmd1_status;
};

struct cb_board_config {
	uint32_t tag;
	uint32_t size;

	cb_uint64_t fw_config;
	uint32_t board_id;
	uint32_t ram_code;
	uint32_t sku_id;
};

#define CB_MAX_SERIALNO_LENGTH	32

struct cb_cmos_option_table {
	u32 tag;
	u32 size;
	u32 header_length;
};

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

#define CB_CMOS_MAX_TEXT_LENGTH 32
struct cb_cmos_enums {
	u32 tag;
	u32 size;
	u32 config_id;
	u32 value;
	u8 text[CB_CMOS_MAX_TEXT_LENGTH];
};

#define CB_CMOS_IMAGE_BUFFER_SIZE 128
struct cb_cmos_defaults {
	u32 tag;
	u32 size;
	u32 name_length;
	u8 name[CB_CMOS_MAX_NAME_LENGTH];
	u8 default_set[CB_CMOS_IMAGE_BUFFER_SIZE];
};

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

/*
 * Handoff the ACPI RSDP
 */
struct cb_acpi_rsdp {
	uint32_t tag;
	uint32_t size;
	cb_uint64_t rsdp_pointer; /* Address of the ACPI RSDP */
};


/* Helpful inlines */

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
