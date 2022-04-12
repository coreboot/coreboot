/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/cpu.h>
#include <types.h>
#include <cpu/amd/microcode.h>
#include <commonlib/helpers.h>
#include <console/console.h>
#include <cpu/x86/msr.h>
#include <cpu/amd/msr.h>
#include <cbfs.h>
#include <stdio.h>
#include <timestamp.h>

#define CPU_MICROCODE_BLOB_NAME "cpu_microcode_XXXX.bin"
#define CPU_MICROCODE_BLOB_FORMAT "cpu_microcode_%04x.bin"

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

static const struct microcode *find_microcode(const struct microcode *ucode,
					      uint16_t equivalent_processor_rev_id)
{
	if (ucode->processor_rev_id == equivalent_processor_rev_id)
		return ucode;

	printk(BIOS_WARNING, "Failed to find microcode for processor rev: %hx.\n",
	       equivalent_processor_rev_id);

	return NULL;
}

void amd_update_microcode_from_cbfs(void)
{
	static const struct microcode *ucode_cache;
	static bool cache_valid;

	struct microcode *ucode;
	char name[] = CPU_MICROCODE_BLOB_NAME;
	uint16_t equivalent_processor_rev_id;

	/* Cache the buffer so each CPU doesn't need to read the uCode from flash */
	/* Note that this code assumes all cores are the same */
	if (!cache_valid) {
		timestamp_add_now(TS_READ_UCODE_START);
		equivalent_processor_rev_id = get_equivalent_processor_rev_id();
		snprintf(name, sizeof(name), CPU_MICROCODE_BLOB_FORMAT, equivalent_processor_rev_id);
		ucode = cbfs_map(name, NULL);
		if (!ucode) {
			printk(BIOS_WARNING, "%s not found. Skipping updates.\n", name);
			return;
		}

		ucode_cache = find_microcode(ucode, equivalent_processor_rev_id);

		if (!ucode_cache) {
			cbfs_unmap(ucode);
			return;
		}

		cache_valid = true;
		timestamp_add_now(TS_READ_UCODE_END);
	}

	apply_microcode_patch(ucode_cache);
}

void preload_microcode(void)
{
	if (!CONFIG(CBFS_PRELOAD))
		return;

	char name[] = CPU_MICROCODE_BLOB_NAME;
	uint16_t equivalent_processor_rev_id = get_equivalent_processor_rev_id();

	snprintf(name, sizeof(name), CPU_MICROCODE_BLOB_FORMAT, equivalent_processor_rev_id);
	printk(BIOS_DEBUG, "Preloading microcode %s\n", name);
	cbfs_preload(name);
}
