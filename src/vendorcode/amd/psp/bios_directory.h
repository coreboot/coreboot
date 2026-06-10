/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef _AMD_PSP_BIOS_DIRECTORY_H_
#define  _AMD_PSP_BIOS_DIRECTORY_H_

#include <commonlib/bsd/compiler.h>
#include <stdint.h>

/*
 * AMD PSP directory structures.
 * Document #55758.
 */
struct bios_directory_hdr {
	uint32_t cookie;
	uint32_t checksum;
	uint32_t num_entries;
	union {
		uint32_t additional_info;
		struct {
			uint32_t dir_size:10;		/* in 4K blocks */
			uint32_t spi_block_size:4;
			uint32_t base_addr:15;
			uint32_t address_mode:2;
			uint32_t version:1;		/* Always 0 */
		} __packed additional_info_fields;
		struct {
			uint32_t dir_size:16;		/* in 4K blocks */
			uint32_t spi_block_size:4;	/* 4K << (1 << value) */
			uint32_t dir_hdr_size:4;	/* in 1K blocks */
			uint32_t address_mode:2;	/* directory address mode */
			uint32_t reserved:5;
			uint32_t version:1;		/* Always 1 */
		} __packed additional_info_fields_v1;
	};
} __packed __aligned(16);

struct bios_directory_entry {
	uint8_t type;
	uint8_t region_type;
	uint8_t reset:1;
	uint8_t copy:1;
	uint8_t ro:1;
	uint8_t compressed:1;
	uint8_t inst:4;
	uint8_t subprog:3;
	uint8_t romid:2;
	uint8_t writable:1;
	uint8_t rsvd:2;
	uint32_t size;
	uint64_t source:62;
	uint64_t address_mode:2;
	uint64_t dest;
} __packed;

struct bios_directory_table {
	struct bios_directory_hdr header;
	struct bios_directory_entry entries[];
} __packed __aligned(16);

#define BHD_COOKIE 0x44484224		/* 'DHB$ */
#define BHDL2_COOKIE 0x324c4224		/* '2LB$ */

enum amd_bios_type {
	AMD_BIOS_RTM_PUBKEY = 0x05,
	AMD_BIOS_SIG = 0x07,
	AMD_BIOS_APCB = 0x60,
	AMD_BIOS_APOB = 0x61,
	AMD_BIOS_BIN = 0x62,
	AMD_BIOS_APOB_NV = 0x63,
	AMD_BIOS_PMUI = 0x64,
	AMD_BIOS_PMUD = 0x65,
	AMD_BIOS_UCODE = 0x66,
	AMD_BIOS_APCB_BK = 0x68,
	AMD_BIOS_EARLY_VGA = 0x69,
	AMD_BIOS_MP2_CFG = 0x6a,
	AMD_BIOS_PSP_SHARED_MEM = 0x6b,
	AMD_BIOS_NV_ST = 0x6d,
	AMD_BIOS_L2_PTR =  0x70,
	AMD_BIOS_INVALID,
};

#endif  /* _AMD_PSP_BIOS_DIRECTORY_H_ */
