/* SPDX-License-Identifier: GPL-2.0-only */

#include <cbmem.h>
#include <console/console.h>
#include <imd.h>
#include <stage_cache.h>
#include <string.h>

static struct imd imd_stage_cache;

static void stage_cache_create_empty(void)
{
	struct imd *imd;
	void *base;
	size_t size;

	imd = &imd_stage_cache;
	stage_cache_external_region(&base, &size);
	imd_handle_init(imd, (void *)(size + (uintptr_t)base));

	printk(BIOS_DEBUG, "External stage cache:\n");
	imd_create_tiered_empty(imd, 4096, 4096, 1024, 32);
	if (imd_limit_size(imd, size))
		printk(BIOS_DEBUG, "Could not limit stage cache size.\n");
}

static void stage_cache_recover(void)
{
	struct imd *imd;
	void *base;
	size_t size;

	imd = &imd_stage_cache;
	stage_cache_external_region(&base, &size);
	imd_handle_init(imd, (void *)(size + (uintptr_t)base));
	if (imd_recover(imd))
		printk(BIOS_DEBUG, "Unable to recover external stage cache.\n");
}

void stage_cache_add(int stage_id, const struct prog *stage)
{
	struct imd *imd;
	const struct imd_entry *e;
	struct stage_cache *meta;
	void *c;

	imd = &imd_stage_cache;
	e = imd_entry_add(imd, CBMEM_ID_STAGEx_META + stage_id, sizeof(*meta));

	if (e == NULL) {
		printk(BIOS_DEBUG, "Error: Can't add %x metadata to imd\n",
				CBMEM_ID_STAGEx_META + stage_id);
		return;
	}

	meta = imd_entry_at(imd, e);

	meta->load_addr = (uintptr_t)prog_start(stage);
	meta->entry_addr = (uintptr_t)prog_entry(stage);
	meta->arg = (uintptr_t)prog_entry_arg(stage);

	e = imd_entry_add(imd, CBMEM_ID_STAGEx_CACHE + stage_id,
				prog_size(stage));

	if (e == NULL) {
		printk(BIOS_DEBUG, "Error: Can't add stage_cache %x to imd\n",
				CBMEM_ID_STAGEx_CACHE + stage_id);
		return;
	}

	c = imd_entry_at(imd, e);

	memcpy(c, prog_start(stage), prog_size(stage));
}

void stage_cache_add_raw(int stage_id, const void *base, const size_t size)
{
	struct imd *imd;
	const struct imd_entry *e;
	void *c;

	imd = &imd_stage_cache;
	e = imd_entry_add(imd, CBMEM_ID_STAGEx_RAW + stage_id, size);
	if (e == NULL) {
		printk(BIOS_DEBUG, "Error: Can't add %x raw data to imd\n",
				CBMEM_ID_STAGEx_RAW + stage_id);
		return;
	}

	c = imd_entry_at(imd, e);
	if (c == NULL) {
		printk(BIOS_DEBUG, "Error: Can't get %x raw entry in imd\n",
				CBMEM_ID_STAGEx_RAW + stage_id);
		return;
	}

	memcpy(c, base, size);
}

void stage_cache_get_raw(int stage_id, void **base, size_t *size)
{
	struct imd *imd;
	const struct imd_entry *e;

	imd = &imd_stage_cache;
	e = imd_entry_find(imd, CBMEM_ID_STAGEx_RAW + stage_id);
	if (e == NULL) {
		printk(BIOS_DEBUG, "Error: Can't find %x raw data to imd\n",
				CBMEM_ID_STAGEx_RAW + stage_id);
		return;
	}

	*base = imd_entry_at(imd, e);
	*size = imd_entry_size(e);
}

void stage_cache_load_stage(int stage_id, struct prog *stage)
{
	struct imd *imd;
	struct stage_cache *meta;
	const struct imd_entry *e;
	void *c;
	size_t size;

	imd = &imd_stage_cache;
	e = imd_entry_find(imd, CBMEM_ID_STAGEx_META + stage_id);
	if (e == NULL) {
		printk(BIOS_DEBUG, "Error: Can't find %x metadata in imd\n",
				CBMEM_ID_STAGEx_META + stage_id);
		return;
	}

	meta = imd_entry_at(imd, e);

	e = imd_entry_find(imd, CBMEM_ID_STAGEx_CACHE + stage_id);

	if (e == NULL) {
		printk(BIOS_DEBUG, "Error: Can't find stage_cache %x in imd\n",
				CBMEM_ID_STAGEx_CACHE + stage_id);
		return;
	}

	c = imd_entry_at(imd, e);
	size = imd_entry_size(e);

	memcpy((void *)(uintptr_t)meta->load_addr, c, size);

	prog_set_area(stage, (void *)(uintptr_t)meta->load_addr, size);
	prog_set_entry(stage, (void *)(uintptr_t)meta->entry_addr,
			(void *)(uintptr_t)meta->arg);
}

static void stage_cache_setup(int is_recovery)
{
	if (is_recovery)
		stage_cache_recover();
	else
		stage_cache_create_empty();
}

CBMEM_READY_HOOK(stage_cache_setup);
