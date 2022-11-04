/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/cbmem_console.h>
#include <reset.h>
#include <security/vboot/misc.h>
#include <security/vboot/vboot_common.h>
#include <security/vboot/vbnv.h>
#include <vb2_api.h>

#include "antirollback.h"

void vboot_save_data(struct vb2_context *ctx)
{
	if (ctx->flags & VB2_CONTEXT_SECDATA_FIRMWARE_CHANGED &&
			(CONFIG(VBOOT_MOCK_SECDATA) || tlcl_lib_init() == VB2_SUCCESS)) {
		printk(BIOS_INFO, "Saving secdata firmware\n");
		antirollback_write_space_firmware(ctx);
		ctx->flags &= ~VB2_CONTEXT_SECDATA_FIRMWARE_CHANGED;
	}

	if (ctx->flags & VB2_CONTEXT_SECDATA_KERNEL_CHANGED &&
			(CONFIG(VBOOT_MOCK_SECDATA) || tlcl_lib_init() == VB2_SUCCESS)) {
		printk(BIOS_INFO, "Saving secdata kernel\n");
		antirollback_write_space_kernel(ctx);
		ctx->flags &= ~VB2_CONTEXT_SECDATA_KERNEL_CHANGED;
	}

	if (ctx->flags & VB2_CONTEXT_NVDATA_CHANGED) {
		printk(BIOS_INFO, "Saving nvdata\n");
		save_vbnv(ctx->nvdata);
		ctx->flags &= ~VB2_CONTEXT_NVDATA_CHANGED;
	}
}

/* Check if it is okay to enable USB Device Controller (UDC). */
int vboot_can_enable_udc(void)
{
	/* Allow UDC in all vboot modes. */
	if (!CONFIG(CHROMEOS) && CONFIG(VBOOT_ALWAYS_ALLOW_UDC))
		return 1;

	/* Always disable if not in developer mode */
	if (!vboot_developer_mode_enabled())
		return 0;
	/* Enable if GBB flag is set */
	if (vboot_is_gbb_flag_set(VB2_GBB_FLAG_ENABLE_UDC))
		return 1;
	/* Enable if VBNV flag is set */
	if (vbnv_udc_enable_flag())
		return 1;
	/* Otherwise disable */
	return 0;
}

/* ============================ VBOOT REBOOT ============================== */
void __weak vboot_platform_prepare_reboot(void)
{
}

void vboot_reboot(void)
{
	if (CONFIG(CONSOLE_CBMEM_DUMP_TO_UART))
		cbmem_dump_console_to_uart();
	vboot_platform_prepare_reboot();
	board_reset();
}

void vboot_save_and_reboot(struct vb2_context *ctx, uint8_t subcode)
{
	printk(BIOS_INFO, "vboot: reboot requested (%#x)\n", subcode);
	vboot_save_data(ctx);
	vboot_reboot();
}

void vboot_fail_and_reboot(struct vb2_context *ctx, uint8_t reason, uint8_t subcode)
{
	if (reason)
		vb2api_fail(ctx, reason, subcode);

	vboot_save_and_reboot(ctx, subcode);
}
