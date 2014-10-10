/*
 * This file is part of the coreboot project.
 *
 * Copyright 2014 Google Inc.
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

#include <2api.h>
#include <2struct.h>
#include <antirollback.h>
#include <console/console.h>
#include <console/vtxprintf.h>
#include <string.h>

#include "../chromeos.h"
#include "misc.h"

#define TODO_BLOCK_SIZE 1024

static int is_slot_a(struct vb2_context *ctx)
{
	return !(ctx->flags & VB2_CONTEXT_FW_SLOT_B);
}

/* exports */

void vb2ex_printf(const char *func, const char *fmt, ...)
{
	va_list args;

	printk(BIOS_INFO, "VB2:%s() ", func);
	va_start(args, fmt);
	vprintk(BIOS_INFO, fmt, args);
	va_end(args);

	return;
}

int vb2ex_tpm_clear_owner(struct vb2_context *ctx)
{
	uint32_t rv;
	printk(BIOS_INFO, "Clearing TPM owner\n");
	rv = tpm_clear_and_reenable();
	if (rv)
		return VB2_ERROR_EX_TPM_CLEAR_OWNER;
	return VB2_SUCCESS;
}

int vb2ex_read_resource(struct vb2_context *ctx,
			enum vb2_resource_index index,
			uint32_t offset,
			void *buf,
			uint32_t size)
{
	struct vboot_region region;

	switch (index) {
	case VB2_RES_GBB:
		vboot_locate_region("GBB", &region);
		break;
	case VB2_RES_FW_VBLOCK:
		if (is_slot_a(ctx))
			vboot_locate_region("VBLOCK_A", &region);
		else
			vboot_locate_region("VBLOCK_B", &region);
		break;
	default:
		return VB2_ERROR_EX_READ_RESOURCE_INDEX;
	}

	if (offset + size > region.size)
		return VB2_ERROR_EX_READ_RESOURCE_SIZE;

	if (vboot_get_region(region.offset_addr + offset, size, buf) == NULL)
		return VB2_ERROR_UNKNOWN;

	return VB2_SUCCESS;
}

static int hash_body(struct vb2_context *ctx, struct vboot_region *fw_main)
{
	uint32_t expected_size;
	MAYBE_STATIC uint8_t block[TODO_BLOCK_SIZE];
	size_t block_size = sizeof(block);
	uintptr_t offset;
	int rv;

	expected_size = fw_main->size;
	offset = fw_main->offset_addr;

	/* Start the body hash */
	rv = vb2api_init_hash(ctx, VB2_HASH_TAG_FW_BODY, &expected_size);
	if (rv)
		return rv;

	/* Extend over the body */
	while (expected_size) {
		void *b;
		if (block_size > expected_size)
			block_size = expected_size;

		b = vboot_get_region(offset, block_size, block);
		if (b == NULL)
			return VB2_ERROR_UNKNOWN;
		rv = vb2api_extend_hash(ctx, b, block_size);
		if (rv)
			return rv;

		expected_size -= block_size;
		offset += block_size;
	}

	/* Check the result */
	rv = vb2api_check_hash(ctx);
	if (rv)
		return rv;

	return VB2_SUCCESS;
}

static int locate_firmware(struct vb2_context *ctx,
			   struct vboot_region *fw_main)
{
	if (is_slot_a(ctx))
		vboot_locate_region("FW_MAIN_A", fw_main);
	else
		vboot_locate_region("FW_MAIN_B", fw_main);

	if (fw_main->size < 0)
		return 1;

	return 0;
}

/**
 * Save non-volatile and/or secure data if needed.
 */
static void save_if_needed(struct vb2_context *ctx)
{
	if (ctx->flags & VB2_CONTEXT_NVDATA_CHANGED) {
		printk(BIOS_INFO, "Saving nvdata\n");
		save_vbnv(ctx->nvdata);
		ctx->flags &= ~VB2_CONTEXT_NVDATA_CHANGED;
	}
	if (ctx->flags & VB2_CONTEXT_SECDATA_CHANGED) {
		printk(BIOS_INFO, "Saving secdata\n");
		antirollback_write_space_firmware(ctx);
		ctx->flags &= ~VB2_CONTEXT_SECDATA_CHANGED;
	}
}

/**
 * Verify and select the firmware in the RW image
 *
 * TODO: Avoid loading a stage twice (once in hash_body & again in load_stage).
 * when per-stage verification is ready.
 */
#if CONFIG_RETURN_FROM_VERSTAGE
void main(void)
#else
void verstage_main(void)
#endif /* CONFIG_RETURN_FROM_VERSTAGE */
{
	struct vb2_context ctx;
	struct vboot_region fw_main;
	struct vb2_working_data *wd = vboot_get_working_data();
	int rv;

	/* Set up context and work buffer */
	memset(&ctx, 0, sizeof(ctx));
	ctx.workbuf = (uint8_t *)(uintptr_t)wd->buffer;
	ctx.workbuf_size = wd->buffer_size;

	/* Read nvdata from a non-volatile storage */
	read_vbnv(ctx.nvdata);

	/* Read secdata from TPM. Initialize TPM if secdata not found. We don't
	 * check the return value here because vb2api_fw_phase1 will catch
	 * invalid secdata and tell us what to do (=reboot). */
	antirollback_read_space_firmware(&ctx);

	if (get_developer_mode_switch())
		ctx.flags |= VB2_CONTEXT_FORCE_DEVELOPER_MODE;
	if (get_recovery_mode_switch()) {
		clear_recovery_mode_switch();
		ctx.flags |= VB2_CONTEXT_FORCE_RECOVERY_MODE;
	}

	/* Do early init */
	printk(BIOS_INFO, "Phase 1\n");
	rv = vb2api_fw_phase1(&ctx);
	if (rv) {
		printk(BIOS_INFO, "Recovery requested (%x)\n", rv);
		/* If we need recovery mode, leave firmware selection now */
		save_if_needed(&ctx);
		return;
	}

	/* Determine which firmware slot to boot */
	printk(BIOS_INFO, "Phase 2\n");
	rv = vb2api_fw_phase2(&ctx);
	if (rv) {
		printk(BIOS_INFO, "Reboot requested (%x)\n", rv);
		save_if_needed(&ctx);
		vboot_reboot();
	}

	/* Try that slot */
	printk(BIOS_INFO, "Phase 3\n");
	rv = vb2api_fw_phase3(&ctx);
	if (rv) {
		printk(BIOS_INFO, "Reboot requested (%x)\n", rv);
		save_if_needed(&ctx);
		vboot_reboot();
	}

	printk(BIOS_INFO, "Phase 4\n");
	rv = locate_firmware(&ctx, &fw_main);
	if (rv)
		die("Failed to read FMAP to locate firmware");

	rv = hash_body(&ctx, &fw_main);
	save_if_needed(&ctx);
	if (rv) {
		printk(BIOS_INFO, "Reboot requested (%x)\n", rv);
		vboot_reboot();
	}

	/* Lock TPM */
	rv = antirollback_lock_space_firmware();
	if (rv) {
		printk(BIOS_INFO, "Failed to lock TPM (%x)\n", rv);
		vb2api_fail(&ctx, VB2_RECOVERY_RO_TPM_L_ERROR, 0);
		save_if_needed(&ctx);
		vboot_reboot();
	}

	printk(BIOS_INFO, "Slot %c is selected\n", is_slot_a(&ctx) ? 'A' : 'B');
	vb2_set_selected_region(wd, &fw_main);
}
