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

#ifndef __ROMCC__
#include <stdint.h>
#include <console/console.h>
#include <cpu/x86/msr.h>
#include <cpu/amd/microcode.h>
#endif

#ifndef __PRE_RAM__
#include <cpu/cpu.h>
#include <cpu/x86/cache.h>
#endif

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

static int need_apply_patch(struct microcode *m, u32 equivalent_processor_rev_id)
{

	if (m->processor_rev_id != equivalent_processor_rev_id) {
		printk(BIOS_ERR, "microcode: rev id (%x) does not match this patch.\n", m->processor_rev_id);
		printk(BIOS_ERR, "microcode: Not updated! Fix microcode_updates[] \n");
		return 0;
	}
	if (m->nb_dev_id) {
		  //look at the device id, if not found return;
		  //if(m->nb_rev_id != installed_nb_rev_id) return 0;
	}

	if (m->ht_io_hub_dev_id) {
		  //look at the device id, if not found return;
		  //if(m->ht_io_hub_rev_id != installed_ht_io_bub_rev_id) return 0;
	}

	if (m->x86_code_present) {
		  //if(!x86_code_execute()) return 0;
	}

	return 1;
}


void amd_update_microcode(void *microcode_updates, u32 equivalent_processor_rev_id)
{
	u32 patch_id, new_patch_id;
	struct microcode *m;
	char *c;
	msr_t msr;

	msr = rdmsr(0x8b);
	patch_id = msr.lo;

	printk(BIOS_DEBUG, "microcode: equivalent rev id  = 0x%04x, current patch id = 0x%08x\n", equivalent_processor_rev_id, patch_id);

	m = microcode_updates;

	for(c = microcode_updates; m->date_code;  m = (struct microcode *)c) {

		if( need_apply_patch(m, equivalent_processor_rev_id) ) {
			//apply patch

			msr.hi = 0;
			msr.lo = (u32)m;

			wrmsr(0xc0010020, msr);

			printk(BIOS_DEBUG, "microcode: patch id to apply = 0x%08x\n", m->patch_id);

			//read the patch_id again
			msr = rdmsr(0x8b);
			new_patch_id = msr.lo;

			printk(BIOS_DEBUG, "microcode: updated to patch id = 0x%08x %s\n", new_patch_id , (new_patch_id == m->patch_id)?" success\n":" fail\n" );
			break;
		}
		c += 2048;
	}

}
