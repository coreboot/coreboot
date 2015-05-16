/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Advanced Micro Devices, Inc.
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
 * Foundation, Inc.
 */

#include <stdint.h>
#include <console/console.h>
#include <cpu/x86/msr.h>
#include <cpu/amd/microcode.h>
#include <cbfs.h>

#define UCODE_DEBUG(fmt, args...)	\
	do { printk(BIOS_DEBUG, "[microcode] "fmt, ##args); } while(0)

struct microcode {
	u32 date_code;
	u32 patch_id;

	u16 m_patch_data_id;
	u8 m_patch_data_len;
	u8 init_flag;

	u32 m_patch_data_cksum;

	u32 nb_dev_id;
	u32 ht_io_hub_dev_id;

	u16 processor_rev_id;
	u8 ht_io_hub_rev_id;
	u8 nb_rev_id;

	u8 bios_api_rev;
	u8 resv1[3];

	u32 match_reg[8];

	u8 m_patch_data[896];
	u8 resv2[896];

	u8 x86_code_present;
	u8 x86_code_entry[191];
};

static void apply_microcode_patch(const struct microcode *m)
{
	uint32_t new_patch_id;
	msr_t msr;

	/* apply patch */
	msr.hi = 0;
	msr.lo = (uint32_t)m;

	wrmsr(0xc0010020, msr);

	UCODE_DEBUG("patch id to apply = 0x%08x\n", m->patch_id);

	/* read the patch_id again */
	msr = rdmsr(0x8b);
	new_patch_id = msr.lo;

	UCODE_DEBUG("updated to patch id = 0x%08x %s\n", new_patch_id ,
		    (new_patch_id == m->patch_id) ? "success" : "fail");
}

static void amd_update_microcode(const void *ucode,  size_t ucode_len,
				 uint32_t equivalent_processor_rev_id)
{
	const struct microcode *m;
	const uint8_t *c = ucode;
	const uint8_t *ucode_end = (uint8_t*)ucode + ucode_len;

	while (c <= (ucode_end - 2048)) {
		m = (struct microcode *)c;
		if (m->processor_rev_id == equivalent_processor_rev_id) {
			apply_microcode_patch(m);
			break;
		}
		c += 2048;
	}
}

#define MICROCODE_CBFS_FILE "cpu_microcode_blob.bin"

void amd_update_microcode_from_cbfs(u32 equivalent_processor_rev_id)
{
	const void *ucode;
	size_t ucode_len;

	if (equivalent_processor_rev_id == 0) {
		UCODE_DEBUG("rev id not found. Skipping microcode patch!\n");
		return;
	}

	ucode = cbfs_boot_map_with_leak(MICROCODE_CBFS_FILE,
					CBFS_TYPE_MICROCODE, &ucode_len);
	if (!ucode) {
		UCODE_DEBUG("microcode file not found. Skipping updates.\n");
		return;
	}

	amd_update_microcode(ucode, ucode_len, equivalent_processor_rev_id);
}
