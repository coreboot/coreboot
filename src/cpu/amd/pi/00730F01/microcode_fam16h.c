/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Advanced Micro Devices, Inc.
 * Copyright (C) 2015 Raptor Engineering
 * Copyright (C) 2018 PC Engines GmbH
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

#define F16H_MPB_MAX_SIZE 	3458
#define F16H_MPB_DATA_OFFSET    32

 /*
  * STRUCTURE OF A MICROCODE (UCODE) FILE FOR FAM16h
  *	Microcode Patch Block
  *		Microcode Header
  *		Microcode "Blob"
  *		...
  *		...
  *		(end of file)
  *
  *
  * MICROCODE HEADER (offset 0 bytes from start of file)
  * Total size = 32 bytes
  *	[0:3]	Date code		(32 bits)
  *	[4:7]	Patch level		(32 bits)
  *	[8:9]	Microcode patch data ID (16 bits)
  *	[10:15]	Reserved		(48 bits)
  *	[16:19]	Chipset 1 device ID	(32 bits)
  *	[20:23]	Chipset 2 device ID	(32 bits)
  *	[24:25]	Processor Revisions ID	(16 bits)
  *	[26]	Chipset 1 revision ID	(8 bits)
  *	[27]	Chipset 2 revision ID	(8 bits)
  *	[28:31]	Reserved		(32 bits)
  *
  * MICROCODE BLOB (offset += 32)
  * Total size = m bytes
  *
  */

struct microcode {
	uint32_t date_code;
	uint32_t patch_id;

	uint16_t mc_patch_data_id;
	uint8_t reserved1[6];

	uint32_t chipset1_dev_id;
        uint32_t chipset2_dev_id;

	uint16_t processor_rev_id;

	uint8_t chipset1_rev_id;
	uint8_t chipset2_rev_id;

	uint8_t reserved2[4];

	uint8_t m_patch_data[F16H_MPB_MAX_SIZE-F16H_MPB_DATA_OFFSET];

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

	/* patch authentication */
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

        m = (struct microcode *)c;

        if (m->processor_rev_id == equivalent_processor_rev_id) {
                apply_microcode_patch(m);
        }
}

static const char *microcode_cbfs_file[] = {
	"microcode_amd.bin",
	"microcode_amd_fam16h.bin",
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

		if(ucode_len > F16H_MPB_MAX_SIZE) {
			UCODE_DEBUG("microcode file invalid. Skipping updates.\n");
		}

		amd_update_microcode(ucode, ucode_len, equivalent_processor_rev_id);

#ifdef __PRE_RAM__
#if IS_ENABLED(CONFIG_HAVE_ROMSTAGE_MICROCODE_CBFS_SPINLOCK)
		spin_unlock(romstage_microcode_cbfs_lock());
#endif
#endif
	}
}
