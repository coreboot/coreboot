/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/cpu.h>
#include <cbfs.h>
#include <commonlib/helpers.h>
#include <console/console.h>
#include <cpu/amd/microcode.h>
#include <cpu/amd/msr.h>
#include <cpu/cpu.h>
#include <cpu/x86/msr.h>
#include <stdio.h>
#include <timestamp.h>
#include <types.h>

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

static const struct microcode *ucode;

/* This function requires the ucode variable to be initialized by amd_load_microcode_from_cbfs()
   and then allocated memory should be freed by the amd_free_microcode(). */
void amd_apply_microcode_patch(void)
{
	uint32_t new_patch_id;
	msr_t msr;

	if (!ucode) {
		printk(BIOS_ERR, "%s: NULL pointer to microcode\n", __func__);
		return;
	}

	msr.raw = (uintptr_t)ucode;

	wrmsr(MSR_PATCH_LOADER, msr);

	printk(BIOS_DEBUG, "microcode: patch id to apply = 0x%08x\n", ucode->patch_id);

	msr = rdmsr(IA32_BIOS_SIGN_ID);
	new_patch_id = msr.lo;

	if (new_patch_id == ucode->patch_id)
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

static const struct microcode *find_microcode(const struct microcode *microcode,
					      uint16_t equivalent_processor_rev_id)
{
	if (microcode->processor_rev_id == equivalent_processor_rev_id)
		return microcode;

	printk(BIOS_WARNING, "Failed to find microcode for processor rev: %hx.\n",
	       equivalent_processor_rev_id);

	return NULL;
}

void amd_load_microcode_from_cbfs(void)
{
	char name[] = CPU_MICROCODE_BLOB_NAME;
	uint16_t equivalent_processor_rev_id;

	if (ucode)
		return;

	/* Store the pointer to the buffer in global variable, so each CPU doesn't need to read
	 * the uCode from flash. Note that this code assumes all cores are the same */
	timestamp_add_now(TS_READ_UCODE_START);
	equivalent_processor_rev_id = get_equivalent_processor_rev_id();
	snprintf(name, sizeof(name), CPU_MICROCODE_BLOB_FORMAT, equivalent_processor_rev_id);

	ucode = cbfs_map(name, NULL);
	if (!ucode) {
		printk(BIOS_WARNING, "%s not found. Skipping updates.\n", name);
		timestamp_add_now(TS_READ_UCODE_END);
		return;
	}

	if (find_microcode(ucode, equivalent_processor_rev_id) == NULL) {
		cbfs_unmap((void *)ucode);
		ucode = NULL;
	}

	timestamp_add_now(TS_READ_UCODE_END);
}

void amd_free_microcode(void)
{
	if (ucode) {
		cbfs_unmap((void *)ucode);
		ucode = NULL;
	}
}
