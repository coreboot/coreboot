/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Advanced Micro Devices, Inc.
 * Copyright (C) 2015 Raptor Engineering
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

#include <stdint.h>
#include <console/console.h>
#include <cpu/x86/msr.h>
#include <cpu/amd/microcode.h>
#include <cbfs.h>
#include <arch/io.h>
#include <smp/spinlock.h>

#define UCODE_DEBUG(fmt, args...)	\
	do { printk(BIOS_DEBUG, "[microcode] "fmt, ##args); } while (0)

#define UCODE_MAGIC			0x00414d44
#define UCODE_EQUIV_CPU_TABLE_TYPE	0x00000000
#define UCODE_SECTION_START_ID		0x00000001
#define UCODE_MAGIC			0x00414d44

#define F1XH_MPB_MAX_SIZE	2048
#define F15H_MPB_MAX_SIZE	4096
#define CONT_HDR		12
#define SECT_HDR		8

/*
 * STRUCTURE OF A MICROCODE (UCODE) FILE
 *	Container Header
 *	Section Header
 *	Microcode Header
 *	Microcode "Blob"
 *	Section Header
 *	Microcode Header
 *	Microcode "Blob"
 *	...
 *	...
 *	(end of file)
 *
 *
 * CONTAINER HEADER (offset 0 bytes from start of file)
 * Total size = fixed size (12 bytes) + variable size
 *	[0:3]  32-bit unique ID
 *	[4:7]  don't-care
 *	[8-11] Size (n) in bytes of variable portion of container header
 *	[12-n] don't-care
 *
 * SECTION HEADER (offset += 12+n)
 * Total size = 8 bytes
 *	[0:3] Unique identifier signaling start of section (0x00000001)
 *	[4:7] Total size (m) of following microcode section, including microcode header
 *
 * MICROCODE HEADER (offset += 8)
 * Total size = 64 bytes
 *	[0:3]	Data code		(32 bits)
 *	[4:7]	Patch ID		(32 bits)
 *	[8:9]	Microcode patch data ID (16 bits)
 *	[10]	c patch data length	(8  bits)
 *	[11]	init flag		(8 bits)
 *	[12:15]	ucode patch data cksum	(32 bits)
 *	[16:19]	nb dev ID		(32 bits)
 *	[20:23]	sb dev ID		(32 bits)
 *	[24:25]	Processor rev ID	(16 bits)
 *	[26]	nb revision ID		(8 bits)
 *	[27]	sb revision ID		(8 bits)
 *	[28]	BIOS API revision	(8 bits)
 *	[29-31]	Reserved 1 (array of three 8-bit values)
 *	[32-63]	Match reg (array of eight 32-bit values)
 *
 * MICROCODE BLOB (offset += 64)
 * Total size = m bytes
 *
 */

struct microcode {
	uint32_t data_code;
	uint32_t patch_id;

	uint16_t mc_patch_data_id;
	uint8_t mc_patch_data_len;
	uint8_t init_flag;

	uint32_t mc_patch_data_checksum;

	uint32_t nb_dev_id;
	uint32_t sb_dev_id;

	uint16_t processor_rev_id;
	uint8_t nb_rev_id;
	uint8_t sb_rev_id;

	uint8_t bios_api_rev;
	uint8_t reserved1[3];

	uint32_t match_reg[8];

	uint8_t m_patch_data[896];
	uint8_t resv2[896];

	uint8_t x86_code_present;
	uint8_t x86_code_entry[191];
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
	const uint8_t *cur_section_hdr;

	uint32_t container_hdr_id;
	uint32_t container_hdr_size;
	uint32_t blob_size;
	uint32_t sec_hdr_id;

	/* Container Header */
	container_hdr_id = read32(c);
	if (container_hdr_id != UCODE_MAGIC) {
		UCODE_DEBUG("Invalid container header ID\n");
		return;
	}

	container_hdr_size = read32(c + 8);
	cur_section_hdr = c + CONT_HDR + container_hdr_size;

	/* Read in first section header ID */
	sec_hdr_id = read32(cur_section_hdr);
	c = cur_section_hdr + 4;

	/* Loop through sections */
	while (sec_hdr_id == UCODE_SECTION_START_ID &&
		c <= (ucode_end - F15H_MPB_MAX_SIZE)) {

		blob_size = read32(c);

		m = (struct microcode *)(c + 4);

		if (m->processor_rev_id == equivalent_processor_rev_id) {
			apply_microcode_patch(m);
			break;
		}

		cur_section_hdr = c + 4 + blob_size;
		sec_hdr_id = read32(cur_section_hdr);
		c = cur_section_hdr + 4;
	}
}

static const char *microcode_cbfs_file[] = {
	"microcode_amd.bin",
	"microcode_amd_fam15h.bin",
};

void amd_update_microcode_from_cbfs(uint32_t equivalent_processor_rev_id)
{
	const void *ucode;
	size_t ucode_len;

	uint32_t i;

	for (i = 0; i < ARRAY_SIZE(microcode_cbfs_file); i++)
	{
		if (equivalent_processor_rev_id == 0) {
			UCODE_DEBUG("rev id not found. Skipping microcode patch!\n");
			return;
		}

#ifdef __PRE_RAM__
#if IS_ENABLED(CONFIG_HAVE_ROMSTAGE_MICROCODE_CBFS_SPINLOCK)
		spin_lock(romstage_microcode_cbfs_lock());
#endif
#endif

		ucode = cbfs_boot_map_with_leak(microcode_cbfs_file[i],
						CBFS_TYPE_MICROCODE, &ucode_len);
		if (!ucode) {
			UCODE_DEBUG("microcode file not found. Skipping updates.\n");
#ifdef __PRE_RAM__
#if IS_ENABLED(CONFIG_HAVE_ROMSTAGE_MICROCODE_CBFS_SPINLOCK)
			spin_unlock(romstage_microcode_cbfs_lock());
#endif
#endif
			return;
		}

		amd_update_microcode(ucode, ucode_len, equivalent_processor_rev_id);

#ifdef __PRE_RAM__
#if IS_ENABLED(CONFIG_HAVE_ROMSTAGE_MICROCODE_CBFS_SPINLOCK)
		spin_unlock(romstage_microcode_cbfs_lock());
#endif
#endif
	}
}
