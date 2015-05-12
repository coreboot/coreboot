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
#include <console/streams.h>
#include <cpu/x86/tsc.h>
#include <program_loading.h>
#include <rmodule.h>
#include <stage_cache.h>
#if IS_ENABLED(CONFIG_CHROMEOS)
#include <vendorcode/google/chromeos/vboot_handoff.h>
#endif

#include <soc/ramstage.h>
#include <soc/efi_wrapper.h>

static void ABI_X86 send_to_console(unsigned char b)
{
	console_tx_byte(b);
}

static efi_wrapper_entry_t load_refcode_from_cache(void)
{
	struct prog refcode;

	printk(BIOS_DEBUG, "refcode loading from cache.\n");

	stage_cache_load_stage(STAGE_REFCODE, &refcode);

	return (efi_wrapper_entry_t)prog_entry(&refcode);
}

static void cache_refcode(const struct rmod_stage_load *rsl)
{
	stage_cache_add(STAGE_REFCODE, rsl->prog);
}

#if IS_ENABLED(CONFIG_CHROMEOS)
static int load_refcode_from_vboot(struct rmod_stage_load *refcode)
{
	struct vboot_handoff *vboot_handoff;
	const struct firmware_component *fwc;
	struct cbfs_stage *stage;

	vboot_handoff = cbmem_find(CBMEM_ID_VBOOT_HANDOFF);
	fwc = &vboot_handoff->components[CONFIG_VBOOT_REFCODE_INDEX];

	if (vboot_handoff == NULL ||
	    vboot_handoff->selected_firmware == VB_SELECT_FIRMWARE_READONLY ||
	    CONFIG_VBOOT_REFCODE_INDEX >= MAX_PARSED_FW_COMPONENTS ||
	    fwc->size == 0 || fwc->address == 0)
		return -1;

	printk(BIOS_DEBUG, "refcode loading from vboot rw area.\n");
	stage = (void *)(uintptr_t)fwc->address;

	if (rmodule_stage_load(refcode, stage)) {
		printk(BIOS_DEBUG, "Error loading reference code.\n");
		return -1;
	}
	return 0;
}
#else
static int load_refcode_from_vboot(struct rmod_stage_load *refcode)
{
	return -1;
}
#endif

static int load_refcode_from_cbfs(struct rmod_stage_load *refcode)
{
	printk(BIOS_DEBUG, "refcode loading from cbfs.\n");

	if (rmodule_stage_load_from_cbfs(refcode)) {
		printk(BIOS_DEBUG, "Error loading reference code.\n");
		return -1;
	}

	return 0;
}

static efi_wrapper_entry_t load_reference_code(void)
{
	struct prog prog = {
		.name = CONFIG_CBFS_PREFIX "/refcode",
	};
	struct rmod_stage_load refcode = {
		.cbmem_id = CBMEM_ID_REFCODE,
		.prog = &prog,
	};

	if (acpi_is_wakeup_s3()) {
		return load_refcode_from_cache();
	}

	if (load_refcode_from_vboot(&refcode) &&
		load_refcode_from_cbfs(&refcode))
			return NULL;

	/* Cache loaded reference code. */
	cache_refcode(&refcode);

	return prog_entry(&prog);
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
