/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google, Inc.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <stdint.h>
#include <stddef.h>
#include <cbfs.h>
#include <cbmem.h>
#include <console/console.h>
#include <console/vtxprintf.h>
#include <pc80/tpm.h>
#include <reset.h>
#include <romstage_handoff.h>
#include <rmodule.h>
#include <string.h>
#include <stdlib.h>
#include <timestamp.h>
#include "chromeos.h"
#include "fmap.h"
#include "vboot_context.h"
#include "vboot_handoff.h"

static void vboot_run_stub(struct vboot_context *context)
{
	const struct cbmem_entry *vboot_entry;
	struct rmodule vbootstub;
	struct cbfs_stage *stage;
	size_t region_size;
	int rmodule_offset;
	int load_offset;
	char *vboot_region;
	void (*entry)(struct vboot_context *context);

	stage = cbfs_get_file_content(CBFS_DEFAULT_MEDIA,
	                              CONFIG_CBFS_PREFIX "/vboot",
	                              CBFS_TYPE_STAGE);

	if (stage == NULL)
		return;

	rmodule_offset =
		rmodule_calc_region(DYN_CBMEM_ALIGN_SIZE,
	                            stage->memlen, &region_size, &load_offset);

	vboot_entry = cbmem_entry_add(0xffffffff, region_size);

	if (vboot_entry == NULL) {
		printk(BIOS_DEBUG, "Couldn't get region for vboot stub.\n");
		return;
	}

	vboot_region = cbmem_entry_start(vboot_entry);

	if (!cbfs_decompress(stage->compression, &stage[1],
	                     &vboot_region[rmodule_offset], stage->len)) {
		printk(BIOS_DEBUG, "Couldn't decompress vboot stub.\n");
		goto out;
	}

	if (rmodule_parse(&vboot_region[rmodule_offset], &vbootstub)) {
		printk(BIOS_DEBUG, "Couldn't parse vboot stub rmodule.\n");
		goto out;
	}

	if (rmodule_load(&vboot_region[load_offset], &vbootstub)) {
		printk(BIOS_DEBUG, "Couldn't load vboot stub.\n");
		goto out;
	}

	entry = rmodule_entry(&vbootstub);

	/* Call stub. */
	entry(context);

out:
	/* Tear down the region no longer needed. */
	cbmem_entry_remove(vboot_entry);
}

/* Helper routines for the vboot stub. */
static void log_msg(const char *fmt, va_list args)
{
	vtxprintf(console_tx_byte, fmt, args);
	console_tx_flush();
}

static void fatal_error(void)
{
	printk(BIOS_ERR, "vboot encountered fatal error. Resetting.\n");
	hard_reset();
}

static void vboot_invoke_wrapper(struct vboot_handoff *vboot_handoff)
{
	VbCommonParams cparams;
	VbSelectFirmwareParams fparams;
	struct vboot_context context;
	uint32_t *iflags;

	vboot_handoff->selected_firmware = VB_SELECT_FIRMWARE_READONLY;

	memset(&cparams, 0, sizeof(cparams));
	memset(&fparams, 0, sizeof(fparams));
	memset(&context, 0, sizeof(context));

	iflags = &vboot_handoff->init_params.flags;
	if (get_developer_mode_switch())
		*iflags |= VB_INIT_FLAG_DEV_SWITCH_ON;
	if (get_recovery_mode_switch())
		*iflags |= VB_INIT_FLAG_REC_BUTTON_PRESSED;
	if (get_write_protect_state())
		*iflags |= VB_INIT_FLAG_WP_ENABLED;
	if (CONFIG_VIRTUAL_DEV_SWITCH)
		*iflags |= VB_INIT_FLAG_VIRTUAL_DEV_SWITCH;
	if (CONFIG_EC_SOFTWARE_SYNC)
		*iflags |= VB_INIT_FLAG_EC_SOFTWARE_SYNC;

	context.handoff = vboot_handoff;
	context.cparams = &cparams;
	context.fparams = &fparams;

	cparams.gbb_size = find_fmap_entry("GBB", &cparams.gbb_data);
	cparams.shared_data_blob = &vboot_handoff->shared_data[0];
	cparams.shared_data_size = VB_SHARED_DATA_MIN_SIZE;
	cparams.caller_context = &context;

	fparams.verification_size_A =
		find_fmap_entry("VBLOCK_A", &fparams.verification_block_A);
	fparams.verification_size_B =
		find_fmap_entry("VBLOCK_B", &fparams.verification_block_B);

	context.fw_a_size =
		find_fmap_entry("FW_MAIN_A", (void **)&context.fw_a);
	context.fw_b_size =
		find_fmap_entry("FW_MAIN_B", (void **)&context.fw_b);

	/* Check all fmap entries. */
	if (context.fw_a == NULL || context.fw_b == NULL ||
	    fparams.verification_block_A == NULL ||
	    fparams.verification_block_B == NULL ||
	    cparams.gbb_data == NULL) {
		printk(BIOS_DEBUG, "Not all fmap entries found for vboot.\n");
		return;
	}

	/* Initialize callbacks. */
	context.read_vbnv = &read_vbnv;
	context.save_vbnv = &save_vbnv;
	context.tis_init = &tis_init;
	context.tis_open = &tis_open;
	context.tis_close = &tis_close;
	context.tis_sendrecv = &tis_sendrecv;
	context.log_msg = &log_msg;
	context.fatal_error = &fatal_error;

	vboot_run_stub(&context);
}

static void vboot_load_ramstage(struct vboot_handoff *vboot_handoff,
                                struct romstage_handoff *handoff)
{
	struct cbfs_stage *stage;
	struct rmodule ramstage;
	void *entry_point;
	size_t region_size;
	char *ramstage_region;
	int rmodule_offset;
	int load_offset;
	const struct cbmem_entry *ramstage_entry;
	const struct firmware_component *fwc;

	if (CONFIG_VBOOT_RAMSTAGE_INDEX >= MAX_PARSED_FW_COMPONENTS) {
		printk(BIOS_ERR, "Invalid ramstage index: %d\n",
		       CONFIG_VBOOT_RAMSTAGE_INDEX);
		return;
	}

	/* Check for invalid address. */
	fwc = &vboot_handoff->components[CONFIG_VBOOT_RAMSTAGE_INDEX];
	if (fwc->address == 0) {
		printk(BIOS_DEBUG, "RW ramstage image address invalid.\n");
		return;
	}

	printk(BIOS_DEBUG, "RW ramstage image at 0x%08x, 0x%08x bytes.\n",
	       fwc->address, fwc->size);

	stage = (void *)fwc->address;

	rmodule_offset =
		rmodule_calc_region(DYN_CBMEM_ALIGN_SIZE,
	                            stage->memlen, &region_size, &load_offset);

	ramstage_entry = cbmem_entry_add(CBMEM_ID_RAMSTAGE, region_size);

	if (ramstage_entry == NULL) {
		vboot_handoff->selected_firmware = VB_SELECT_FIRMWARE_READONLY;
		printk(BIOS_DEBUG, "Could not add ramstage region.\n");
		return;
	}

	timestamp_add_now(TS_START_COPYRAM);

	ramstage_region = cbmem_entry_start(ramstage_entry);

	printk(BIOS_DEBUG, "Decompressing ramstage @ 0x%p (%d bytes)\n",
	       &ramstage_region[rmodule_offset], stage->memlen);

	if (!cbfs_decompress(stage->compression, &stage[1],
	                     &ramstage_region[rmodule_offset], stage->len))
		return;

	if (rmodule_parse(&ramstage_region[rmodule_offset], &ramstage))
		return;

	if (rmodule_load(&ramstage_region[load_offset], &ramstage))
		return;

	entry_point = rmodule_entry(&ramstage);

	cache_loaded_ramstage(handoff, ramstage_entry, entry_point);

	timestamp_add_now(TS_END_COPYRAM);

	__asm__ volatile (
		"movl $0, %%ebp\n"
		"jmp  *%%edi\n"
		:: "D"(entry_point)
	);
}

void vboot_verify_firmware(struct romstage_handoff *handoff)
{
	struct vboot_handoff *vboot_handoff;

	/* Don't go down verified boot path on S3 resume. */
	if (handoff != NULL && handoff->s3_resume)
		return;

	timestamp_add_now(TS_START_VBOOT);

	vboot_handoff = cbmem_add(CBMEM_ID_VBOOT_HANDOFF,
	                          sizeof(*vboot_handoff));

	if (vboot_handoff == NULL) {
		printk(BIOS_DEBUG, "Could not add vboot_handoff structure.\n");
		return;
	}

	memset(vboot_handoff, 0, sizeof(*vboot_handoff));

	vboot_invoke_wrapper(vboot_handoff);

	timestamp_add_now(TS_END_VBOOT);

	/* Take RO firmware path since no RW area was selected. */
	if (vboot_handoff->selected_firmware != VB_SELECT_FIRMWARE_A &&
	    vboot_handoff->selected_firmware != VB_SELECT_FIRMWARE_B) {
		printk(BIOS_DEBUG, "No RW firmware selected: 0x%08x\n",
		       vboot_handoff->selected_firmware);
		return;
	}

	/* Load ramstage from the vboot_handoff structure. */
	vboot_load_ramstage(vboot_handoff, handoff);
}
