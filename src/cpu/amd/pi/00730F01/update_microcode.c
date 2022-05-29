/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdint.h>
#include <arch/cpu.h>
#include <cpu/amd/microcode.h>
#include <commonlib/helpers.h>
#include <console/console.h>
#include <cpu/x86/msr.h>
#include <cpu/amd/msr.h>
#include <cbfs.h>

/*
 * Values and header structure from:
 * BKDG for AMD Family 16h Models 30h-3Fh Processors
 * 52740 Rev 3.06 - March 18, 2016
 */

#define F16H_MPB_MAX_SIZE 3458
#define F16H_MPB_DATA_OFFSET 32

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

	uint8_t m_patch_data[F16H_MPB_MAX_SIZE - F16H_MPB_DATA_OFFSET];
} __packed;

static void apply_microcode_patch(const struct microcode *m)
{
	uint32_t new_patch_id;
	msr_t msr;

	msr.hi = (uint64_t)(uintptr_t)m >> 32;
	msr.lo = (uintptr_t)m & 0xffffffff;

	wrmsr(MSR_PATCH_LOADER, msr);

	printk(BIOS_DEBUG, "microcode: patch id to apply = 0x%08x\n", m->patch_id);

	msr = rdmsr(IA32_BIOS_SIGN_ID);
	new_patch_id = msr.lo;

	if (new_patch_id == m->patch_id)
		printk(BIOS_INFO, "microcode: being updated to patch id = 0x%08x succeeded\n",
		       new_patch_id);
	else
		printk(BIOS_ERR, "microcode: being updated to patch id = 0x%08x failed\n",
		       new_patch_id);
}

static uint16_t get_equivalent_processor_rev_id(void)
{
	uint32_t cpuid_family = cpuid_eax(1);

	return (uint16_t)((cpuid_family & 0xff0000) >> 8 | (cpuid_family & 0xff));
}

static void amd_update_microcode(const void *ucode, size_t ucode_len,
				 uint16_t equivalent_processor_rev_id)
{
	const struct microcode *m;
	const uint8_t *c = ucode;

	m = (struct microcode *)c;

	/* Assume cpu_microcode_blob only contains one microcode. */
	if (m->processor_rev_id == equivalent_processor_rev_id)
		apply_microcode_patch(m);
}

void amd_update_microcode_from_cbfs(void)
{
	const void *ucode;
	size_t ucode_len;
	uint16_t equivalent_processor_rev_id = get_equivalent_processor_rev_id();

	ucode = cbfs_map("cpu_microcode_blob.bin", &ucode_len);
	if (!ucode) {
		printk(BIOS_WARNING, "cpu_microcode_blob.bin not found. Skipping updates.\n");
		return;
	}

	if (ucode_len > F16H_MPB_MAX_SIZE || ucode_len < F16H_MPB_DATA_OFFSET) {
		printk(BIOS_DEBUG, "microcode file invalid. Skipping updates.\n");
		return;
	}

	amd_update_microcode(ucode, ucode_len, equivalent_processor_rev_id);
}
