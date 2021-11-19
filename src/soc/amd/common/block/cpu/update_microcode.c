/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/cpu.h>
#include <stdint.h>
#include <cpu/amd/microcode.h>
#include <commonlib/helpers.h>
#include <console/console.h>
#include <cpu/x86/msr.h>
#include <cpu/amd/msr.h>
#include <cbfs.h>
#include <timestamp.h>

#define CPU_MICROCODE_BLOB_NAME "cpu_microcode_blob.bin"

_Static_assert(CONFIG_SOC_AMD_COMMON_BLOCK_UCODE_SIZE > 0,
	       "SOC_AMD_COMMON_BLOCK_UCODE_SIZE is not set");

#define MPB_MAX_SIZE CONFIG_SOC_AMD_COMMON_BLOCK_UCODE_SIZE
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

_Static_assert(sizeof(struct microcode) == MPB_MAX_SIZE, "microcode size is invalid");

static void apply_microcode_patch(const struct microcode *m)
{
	uint32_t new_patch_id;
	msr_t msr;

	msr.hi = (uint64_t)(uintptr_t)m >> 32;
	msr.lo = (uintptr_t)m & 0xffffffff;

	wrmsr(MSR_PATCH_LOADER, msr);

	printk(BIOS_DEBUG, "microcode: patch id to apply = 0x%08x\n",
		m->patch_id);

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

static const struct microcode *find_microcode(const struct microcode *ucode, size_t ucode_len)
{
	uint16_t equivalent_processor_rev_id = get_equivalent_processor_rev_id();
	const struct microcode *m;

	for (m = ucode; m < ucode + ucode_len / MPB_MAX_SIZE; m++) {
		if (m->processor_rev_id == equivalent_processor_rev_id)
			return m;
	}

	printk(BIOS_WARNING, "Failed to find microcode for processor rev: %hx.\n",
	       equivalent_processor_rev_id);

	return NULL;
}

void amd_update_microcode_from_cbfs(void)
{
	static struct microcode ucode_cache;
	static bool cache_valid;

	struct microcode *ucode_list;
	const struct microcode *matching_ucode;
	size_t ucode_len;

	/* Cache the buffer so each CPU doesn't need to read the uCode from flash */
	if (!cache_valid) {
		timestamp_add_now(TS_READ_UCODE_START);
		ucode_list = cbfs_map(CPU_MICROCODE_BLOB_NAME, &ucode_len);
		if (!ucode_list) {
			printk(BIOS_WARNING,
			       CPU_MICROCODE_BLOB_NAME " not found. Skipping updates.\n");
			return;
		}

		matching_ucode = find_microcode(ucode_list, ucode_len);

		if (!matching_ucode) {
			cbfs_unmap(ucode_list);
			return;
		}

		ucode_cache = *matching_ucode;
		cache_valid = true;

		cbfs_unmap(ucode_list);

		timestamp_add_now(TS_READ_UCODE_END);
	}

	apply_microcode_patch(&ucode_cache);
}

void preload_microcode(void)
{
	if (!CONFIG(CBFS_PRELOAD))
		return;

	printk(BIOS_DEBUG, "Preloading microcode %s\n", CPU_MICROCODE_BLOB_NAME);
	cbfs_preload(CPU_MICROCODE_BLOB_NAME);
}
