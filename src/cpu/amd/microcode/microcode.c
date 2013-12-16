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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <stdint.h>
#include <console/console.h>
#include <cpu/x86/msr.h>
#include <cpu/amd/microcode.h>
#include <cbfs.h>

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

static void amd_update_microcode(const void *microcode_updates, u32 microcode_len, u32 equivalent_processor_rev_id)
{
	u32 new_patch_id;
	msr_t msr;
	const void *c = microcode_updates;
	const void *ucode_end = microcode_updates + microcode_len;

	while ((c < ucode_end) && (c + 2048 <= ucode_end)) {
		const struct microcode *m = c;
		if (m->processor_rev_id == equivalent_processor_rev_id) {
			//apply patch

			printk(BIOS_DEBUG, "microcode: patch id to apply = 0x%08x\n", m->patch_id);
			msr.hi = 0;
			msr.lo = (u32)m;
			wrmsr(0xc0010020, msr);

			//read the patch_id again
			msr = rdmsr(0x8b);
			new_patch_id = msr.lo;

			printk(BIOS_DEBUG, "microcode: updated to patch id = 0x%08x %s\n", new_patch_id , (new_patch_id == m->patch_id)?" success\n":" fail\n" );
			break;
		}
		c += 2048;
	}

}

#define MICROCODE_CBFS_FILE "cpu_microcode_blob.bin"

void amd_update_microcode_from_cbfs(u32 equivalent_processor_rev_id)
{
	struct cbfs_file *microcode_file;
	const void *microcode_updates;
	u32 microcode_len, patch_id;
	msr_t msr;

	msr = rdmsr(0x8b);
	patch_id = msr.lo;

	printk(BIOS_DEBUG, "microcode: equivalent rev id  = 0x%04x, current patch id = 0x%08x\n", equivalent_processor_rev_id, patch_id);
	if (equivalent_processor_rev_id == 0) {
		printk(BIOS_DEBUG, "microcode: rev id not found. Skipping microcode patch!\n");
		return;
	}

	microcode_file = cbfs_get_file(CBFS_DEFAULT_MEDIA, MICROCODE_CBFS_FILE);
	if (!microcode_file)
		return;

	microcode_updates = CBFS_SUBHEADER(microcode_file);
	microcode_len = ntohl(microcode_file->len);
	amd_update_microcode(microcode_updates, microcode_len, equivalent_processor_rev_id);
}
