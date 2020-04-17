/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootstate.h>
#include <cpu/x86/smm.h>
#include <commonlib/helpers.h>
#include <commonlib/region.h>
#include <console/console.h>
#include <smmstore.h>
#include <types.h>
#include <cbmem.h>

static struct smmstore_params_info info;

void lb_smmstorev2(struct lb_header *header)
{
	struct lb_record *rec;
	struct lb_smmstorev2 *store;
	const struct cbmem_entry *e;

	e = cbmem_entry_find(CBMEM_ID_SMM_COMBUFFER);
	if (!e)
		return;

	rec = lb_new_record(header);
	store = (struct lb_smmstorev2 *)rec;

	store->tag = LB_TAG_SMMSTOREV2;
	store->size = sizeof(*store);
	store->com_buffer = (uintptr_t)cbmem_entry_start(e);
	store->com_buffer_size = cbmem_entry_size(e);
	store->mmap_addr = info.mmap_addr;
	store->num_blocks = info.num_blocks;
	store->block_size = info.block_size;
	store->apm_cmd = APM_CNT_SMMSTORE;
}

static void init_store(void *unused)
{
	struct smmstore_params_init args;
	uint32_t eax = ~0;
	uint32_t ebx;

	if (smmstore_get_info(&info) < 0) {
		printk(BIOS_INFO, "SMMSTORE: Failed to get meta data\n");
		return;
	}

	void *ptr = cbmem_add(CBMEM_ID_SMM_COMBUFFER, info.block_size);
	if (!ptr) {
		printk(BIOS_ERR, "SMMSTORE: Failed to add com buffer\n");
		return;
	}

	args.com_buffer = (uintptr_t)ptr;
	args.com_buffer_size = info.block_size;
	ebx = (uintptr_t)&args;

	printk(BIOS_INFO, "SMMSTORE: Setting up SMI handler\n");

	/* Issue SMI using APM to update the com buffer and to lock the SMMSTORE */
	__asm__ __volatile__ (
		"outb %%al, %%dx"
		: "=a" (eax)
		: "a" ((SMMSTORE_CMD_INIT << 8) | APM_CNT_SMMSTORE),
		  "b" (ebx),
		  "d" (APM_CNT)
		: "memory");

	if (eax != SMMSTORE_RET_SUCCESS) {
		printk(BIOS_ERR, "SMMSTORE: Failed to install com buffer\n");
		return;
	}
}

/* The SMI APM handler is installed at DEV_INIT phase */
BOOT_STATE_INIT_ENTRY(BS_DEV_INIT, BS_ON_EXIT, init_store, NULL);
