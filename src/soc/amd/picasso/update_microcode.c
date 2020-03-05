/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdint.h>
#include <cpu/amd/microcode.h>
#include <commonlib/helpers.h>
#include <console/console.h>
#include <cpu/x86/msr.h>
#include <cpu/amd/msr.h>
#include <cbfs.h>

#define MPB_MAX_SIZE 3200
#define MPB_DATA_OFFSET 32

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

	uint8_t m_patch_data[MPB_MAX_SIZE-MPB_DATA_OFFSET];
} __packed;

static void apply_microcode_patch(const struct microcode *m)
{
	uint32_t new_patch_id;
	msr_t msr;

	msr.hi = (uint64_t)(uintptr_t)m >> 32;
	msr.lo = (uintptr_t)m & 0xffffffff;

	wrmsr(MSR_PATCH_LOADER, msr);

	printk(BIOS_DEBUG, "microcode: patch id to apply = 0x%08x\n",
		m->patch_id);

	msr = rdmsr(MSR_PATCH_LEVEL);
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

	for (m = (struct microcode *)ucode;
		m < (struct microcode *)ucode + ucode_len/MPB_MAX_SIZE; m++) {
		if (m->processor_rev_id == equivalent_processor_rev_id)
			apply_microcode_patch(m);
	}
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

	amd_update_microcode(ucode, ucode_len, equivalent_processor_rev_id);
}
