/* SPDX-License-Identifier: GPL-2.0-only */

#include "commonlib/bsd/cb_err.h"
#include <amdblocks/smn.h>
#include <amdblocks/smu.h>
#include <amdblocks/stb.h>
#include <bootstate.h>
#include <cbmem.h>
#include <console/console.h>
#include <soc/smu.h>
#include <soc/stb.h>

#define STB_ENTRIES_PER_ROW 4

static void stb_write32(uint32_t reg, uint32_t val)
{
	smn_write32(STB_CFG_SMN_ADDR + reg, val);
}

static uint32_t stb_read32(uint32_t reg)
{
	return smn_read32(STB_CFG_SMN_ADDR + reg);
}

void soc_post_code(uint8_t value)
{
	if (CONFIG(ADD_POSTCODES_TO_STB))
		stb_write32(AMD_STB_PMI_0, AMD_STB_COREBOOT_POST_PREFIX | value);
}

void write_stb_to_console(void)
{
	int i;
	int printed_data = 0;
	struct stb_entry_struct stb_val;

	/* Add a marker into the STB so it's easy to see where the end is. */
	stb_write32(AMD_STB_PMI_0, AMD_STB_COREBOOT_MARKER);

	for (i = 0; i < AMD_STB_SDRAM_FIFO_SIZE; i++) {
		/*
		 * It's possible to do a single read and leave the timestamp as the first
		 * value of a pair, but by default the value will be first and time stamp
		 * second.  We're just assuming that nothing has messed up the ordering.
		 */
		stb_val.val = stb_read32(AMD_STB_PMI_0);
		stb_val.ts = stb_read32(AMD_STB_PMI_0);

		if (stb_val.val == AMD_STB_COREBOOT_MARKER) {
			if (!printed_data)
				printk(BIOS_DEBUG, "No Smart Trace Buffer Data available.\n");
			else
				// Don't print the coreboot marker
				printk(BIOS_DEBUG, "\n");
			return;
		}

		if (i == 0)
			printk(BIOS_DEBUG, "Available Smart Trace Buffer data:\n");
		if ((i % STB_ENTRIES_PER_ROW) == 0)
			printk(BIOS_DEBUG, "%04d,", i);
		printk(BIOS_DEBUG, " 0x%08x,0x%08x, ", stb_val.ts, stb_val.val);
		if ((i % STB_ENTRIES_PER_ROW) == STB_ENTRIES_PER_ROW - 1)
			printk(BIOS_DEBUG, "\n");
		printed_data = 1;
	}
}

static void init_spill_buffer(void *unused)
{
	struct smu_payload smu_payload = {0};
	uintptr_t stb;
	uint32_t size = CONFIG_AMD_STB_SIZE_IN_MB * MiB;
	int i;

	if (!CONFIG(ENABLE_STB_SPILL_TO_DRAM))
		return;

	stb = (uintptr_t)cbmem_add(CBMEM_ID_AMD_STB, size);
	if (!stb) {
		printk(BIOS_ERR, "Could not allocate cbmem buffer for STB\n");
		return;
	}

	smu_payload.msg[0] = (uint32_t)stb;
	smu_payload.msg[1] = 0;
	smu_payload.msg[2] = size;

	printk(BIOS_DEBUG, "STB spill buffer: allocated %d MiB at %lx\n",
	       CONFIG_AMD_STB_SIZE_IN_MB, stb);

	if (send_smu_message(SMC_MSG_SET_S2D_ADDR, &smu_payload) == CB_ERR)
		printk(BIOS_ERR, "Could not enable STB Spill-to-dram\n");

	for (i = 0; i < SMU_NUM_ARGS; i++)
		printk(BIOS_DEBUG, "smu_payload.msg[%d]: 0x%x\n", i, smu_payload.msg[i]);
}

static void final_stb_console(void *unused)
{
	if (CONFIG(WRITE_STB_BUFFER_TO_CONSOLE))
		write_stb_to_console();
}

BOOT_STATE_INIT_ENTRY(BS_PRE_DEVICE, BS_ON_ENTRY, init_spill_buffer, NULL);
BOOT_STATE_INIT_ENTRY(BS_PAYLOAD_BOOT, BS_ON_ENTRY, final_stb_console, NULL);
