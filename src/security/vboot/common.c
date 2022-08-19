/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <cbmem.h>
#include <fmap.h>
#include <vb2_api.h>
#include <security/vboot/misc.h>
#include <security/vboot/symbols.h>
#include <security/vboot/vboot_common.h>

static struct vb2_context *vboot_ctx;

static void *vboot_get_workbuf(void)
{
	void *wb = NULL;

	if (cbmem_possibly_online())
		wb = cbmem_find(CBMEM_ID_VBOOT_WORKBUF);

	if (!wb && !CONFIG(VBOOT_STARTS_IN_ROMSTAGE) && preram_symbols_available())
		wb = _vboot2_work;

	assert(wb);

	return wb;
}

struct vb2_context *vboot_get_context(void)
{
	void *wb;

	/* Return if context has already been initialized/restored. */
	if (vboot_ctx)
		return vboot_ctx;

	wb = vboot_get_workbuf();

	/* Restore context from a previous stage. */
	if (vboot_logic_executed()) {
		assert(vb2api_reinit(wb, &vboot_ctx) == VB2_SUCCESS);
		return vboot_ctx;
	}

	assert(verification_should_run());

	/* Initialize vb2_shared_data and friends. */
	assert(vb2api_init(wb, VB2_FIRMWARE_WORKBUF_RECOMMENDED_SIZE,
			   &vboot_ctx) == VB2_SUCCESS);

	return vboot_ctx;
}

int vboot_locate_firmware(struct vb2_context *ctx, struct region_device *fw)
{
	const char *name;

	if (vboot_is_firmware_slot_a(ctx))
		name = "FW_MAIN_A";
	else
		name = "FW_MAIN_B";

	int ret = fmap_locate_area_as_rdev(name, fw);
	if (ret)
		return ret;

	/*
	 * Truncate area to the size that was actually signed by vboot.
	 * It is only required for old verification mechanism calculating full body hash.
	 * New verification mechanism uses signature with zero data size, so truncation
	 * is not possible.
	 */
	if (!CONFIG(VBOOT_CBFS_INTEGRATION))
		return rdev_chain(fw, fw, 0, vb2api_get_firmware_size(ctx));

	return 0;
}

static void vboot_setup_cbmem(int unused)
{
	vb2_error_t rv;
	const size_t cbmem_size = VB2_KERNEL_WORKBUF_RECOMMENDED_SIZE;
	void *wb_cbmem = cbmem_add(CBMEM_ID_VBOOT_WORKBUF, cbmem_size);
	assert(wb_cbmem);
	/*
	 * On platforms where VBOOT_STARTS_BEFORE_BOOTBLOCK, the verification
	 * occurs before the main processor starts running.  The vboot data-
	 * structure is available in the _vboot2_work memory area as soon
	 * as the main processor is released.
	 *
	 * For platforms where VBOOT_STARTS_IN_BOOTBLOCK, vboot verification
	 * occurs before CBMEM is brought online, using pre-RAM. In order to
	 * make vboot data structures available downstream, copy vboot workbuf
	 * from SRAM/CAR into CBMEM.
	 *
	 * For platforms where VBOOT_STARTS_IN_ROMSTAGE, verification occurs
	 * after CBMEM is brought online.  Directly initialize vboot data
	 * structures in CBMEM, which will also be available downstream.
	 */
	if (!CONFIG(VBOOT_STARTS_IN_ROMSTAGE))
		rv = vb2api_relocate(wb_cbmem, _vboot2_work, cbmem_size,
				     &vboot_ctx);
	else
		rv = vb2api_init(wb_cbmem, cbmem_size, &vboot_ctx);

	assert(rv == VB2_SUCCESS);
}
CBMEM_CREATION_HOOK(vboot_setup_cbmem);
