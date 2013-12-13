/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <string.h>
#include <arch/acpi.h>
#include <cbmem.h>
#include <console/console.h>
#include <cpu/x86/tsc.h>
#include <rmodule.h>
#include <ramstage_cache.h>
#include <vendorcode/google/chromeos/vboot_handoff.h>

#include <baytrail/ramstage.h>
#include <baytrail/efi_wrapper.h>

static inline int is_s3_resume(void)
{
#if CONFIG_HAVE_ACPI_RESUME
	return acpi_slp_type == 3;
#else
	return 0;
#endif
}

static inline struct ramstage_cache *next_cache(struct ramstage_cache *c)
{
	return (struct ramstage_cache *)&c->program[c->size];
}

static void ABI_X86 send_to_console(unsigned char b)
{
	console_tx_byte(b);
}

static efi_wrapper_entry_t load_refcode_from_cache(void)
{
	struct ramstage_cache *c;
	long cache_size;

	printk(BIOS_DEBUG, "refcode loading from cache.\n");

	c = ramstage_cache_location(&cache_size);

	if (!ramstage_cache_is_valid(c)) {
		printk(BIOS_DEBUG, "Invalid ramstage cache descriptor.\n");
		return NULL;
	}

	c = next_cache(c);
	if (!ramstage_cache_is_valid(c)) {
		printk(BIOS_DEBUG, "Invalid refcode cache descriptor.\n");
		return NULL;
	}

	printk(BIOS_DEBUG, "Loading cached reference code from 0x%08x(%x)\n",
	       c->load_address, c->size);
	memcpy((void *)c->load_address, &c->program[0], c->size);

	return (efi_wrapper_entry_t)c->entry_point;
}

static void cache_refcode(const struct rmod_stage_load *rsl)
{
	struct ramstage_cache *c;
	long cache_size;

	c = ramstage_cache_location(&cache_size);

	if (!ramstage_cache_is_valid(c)) {
		printk(BIOS_DEBUG, "No place to cache reference code.\n");
		return;
	}

	/* Determine how much remaining cache available. */
	cache_size -= c->size + sizeof(*c);

	if (cache_size < (sizeof(*c) + cbmem_entry_size(rsl->cbmem_entry))) {
		printk(BIOS_DEBUG, "Not enough cache space for ref code.\n");
		return;
	}

	c = next_cache(c);
	c->magic = RAMSTAGE_CACHE_MAGIC;
	c->entry_point = (uint32_t)rsl->entry;
	c->load_address = (uint32_t)cbmem_entry_start(rsl->cbmem_entry);
	c->size = cbmem_entry_size(rsl->cbmem_entry);;

	printk(BIOS_DEBUG, "Caching refcode at 0x%p(%x)\n",
	       &c->program[0], c->size);
	memcpy(&c->program[0], (void *)c->load_address, c->size);
}

static int load_refcode_from_vboot(struct rmod_stage_load *refcode,
				    struct cbfs_stage *stage)
{
	printk(BIOS_DEBUG, "refcode loading from vboot rw area.\n");

	if (rmodule_stage_load(refcode, stage) || refcode->entry == NULL) {
		printk(BIOS_DEBUG, "Error loading reference code.\n");
		return -1;
	}
	return 0;
}

static int load_refcode_from_cbfs(struct rmod_stage_load *refcode)
{
	printk(BIOS_DEBUG, "refcode loading from cbfs.\n");

	if (rmodule_stage_load_from_cbfs(refcode) || refcode->entry == NULL) {
		printk(BIOS_DEBUG, "Error loading reference code.\n");
		return -1;
	}

	return 0;
}

static efi_wrapper_entry_t load_reference_code(void)
{
	struct vboot_handoff *vboot_handoff;
	const struct firmware_component *fwc;
	struct rmod_stage_load refcode = {
		.cbmem_id = CBMEM_ID_REFCODE,
		.name = CONFIG_CBFS_PREFIX "/refcode",
	};
	int ret;

	if (is_s3_resume()) {
		return load_refcode_from_cache();
	}

	vboot_handoff = cbmem_find(CBMEM_ID_VBOOT_HANDOFF);
	fwc = &vboot_handoff->components[CONFIG_VBOOT_REFCODE_INDEX];

	if (vboot_handoff == NULL ||
	    vboot_handoff->selected_firmware == VB_SELECT_FIRMWARE_READONLY ||
	    CONFIG_VBOOT_REFCODE_INDEX >= MAX_PARSED_FW_COMPONENTS ||
	    fwc->size == 0 || fwc->address == 0) {
		ret = load_refcode_from_cbfs(&refcode);
	} else {
		ret = load_refcode_from_vboot(&refcode, (void *)fwc->address);

		if (ret < 0)
			ret = load_refcode_from_cbfs(&refcode);
	}

	if (ret < 0)
		return NULL;

	/* Cache loaded reference code. */
	cache_refcode(&refcode);

	return refcode.entry;
}

void baytrail_run_reference_code(void)
{
	int ret;
	efi_wrapper_entry_t entry;
	struct efi_wrapper_params wrp = {
		.version = EFI_WRAPPER_VER,
		.console_out = send_to_console,
	};

	entry = load_reference_code();

	if (entry == NULL)
		return;

	wrp.tsc_ticks_per_microsecond = tsc_freq_mhz();

	/* Call into reference code. */
	ret = entry(&wrp);

	if (ret != 0) {
		printk(BIOS_DEBUG, "Reference code returned %d\n", ret);
		return;
	}
}

