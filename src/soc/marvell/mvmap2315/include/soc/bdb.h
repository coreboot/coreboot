/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Marvell, Inc.
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

#ifndef __SOC_MARVELL_MVMAP2315_BDB_H__
#define __SOC_MARVELL_MVMAP2315_BDB_H__

#include <stdint.h>
#include <compiler.h>

#define MVMAP2315_BDB_LCM_BASE		0xE0000000

enum {
	BDB_RESERVED = 0,
	SP_RW_FIRMWARE = 1,
	AP_RW_FIRMWARE = 2,
	MCU_FIRMWARE = 3,
	APMU_FIRMWARE = 4,
	KERNEL_IMAGE = 128,
	KERNEL_COMMAND_LINE = 129,
	SIXTEEN_BIT_VMLINUX_HEADER = 130
};

struct bdb_header {
	u32 struct_magic;
	u8 struct_major_version;
	u8 struct_minor_version;
	u16 struct_size;
	u64 bdb_load_address;
	u32 bdb_size;
	u32 signed_size;
	u32 oem_area_0_size;
	u8 reserved0[8];
} __packed;

struct bdb_key {
	u32 struct_magic;
	u8 struct_major_version;
	u8 struct_minor_version;
	u16 struct_size;
	u8 hash_alg;
	u8 sig_alg;
	u8 reserved0[2];
	u32 key_version;
	char description[128];
	u8 key_data[];
} __packed;

struct bdb_sig {
	u32 struct_magic;
	u8 struct_major_version;
	u8 struct_minor_version;
	u16 struct_size;
	u8 hash_alg;
	u8 sig_alg;
	u8 reserved0[2];
	u32 signed_size;
	char description[128];
	u8 sig_SOC_MARVELL_MVMAP2315_data[];
} __packed;

struct bdb_data {
	u32 struct_magic;
	u8 struct_major_version;
	u8 struct_minor_version;
	u16 struct_size;
	u32 data_version;
	u32 oem_area_1_size;
	u8 num_hashes;
	u8 hash_entry_size;
	u8 reserved0[2];
	u32 signed_size;
	u8 reserved1[8];
	char description[128];
} __packed;

struct bdb_hash {
	u64 offset;
	u32 size;
	u8 partition;
	u8 type;
	u8 reserved0[2];
	u64 load_address;
	u8 digest[32];
} __packed;

struct bdb_pointer {
	struct bdb_header *bdb_h;
	struct bdb_key	*bdb_k;
	u8 *bdb_oem_0;
	struct bdb_key *sub_k;
	struct bdb_sig *bdb_h_s;
	struct bdb_data *bdb_d;
	u8 *oem_1;
	struct bdb_hash *bdb_hash;
	struct bdb_sig *bdb_s;
} __packed;

void set_bdb_pointers(u8 *start_addr, struct bdb_pointer *bdb_in);
struct bdb_hash *find_bdb_image(struct bdb_pointer *bdb_info, u32 image_type);

#endif /* __SOC_MARVELL_MVMAP2315_BDB_H__ */
