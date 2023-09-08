/* SPDX-License-Identifier: BSD-3-Clause */

#include <libpayload-config.h>
#include <arch/virtual.h>
#include <assert.h>
#include <libpayload.h>
#include <stdio.h>
#include <stdlib.h>
#include <sysinfo.h>
#include <vb2_api.h>
#include <lp_vboot.h>

struct vb2_context *vboot_get_context(void)
{
	static struct vb2_context *ctx;

	if (ctx)
		return ctx;

	die_if(lib_sysinfo.vboot_workbuf == 0, "vboot workbuf pointer is not set\n");

	/* Use the firmware verification workbuf from coreboot. */
	vb2_error_t rv = vb2api_reinit(phys_to_virt(lib_sysinfo.vboot_workbuf), &ctx);

	die_if(rv, "vboot workbuf could not be initialized, error: %#x\n", rv);

	return ctx;
}

void vboot_fail_and_reboot(struct vb2_context *ctx, uint8_t reason, uint8_t subcode)
{
	if (reason)
		vb2api_fail(ctx, reason, subcode);

	printf("vboot: reboot requested (reason: %#x, subcode %#x)", reason, subcode);
	vb2ex_commit_data(ctx);
	reboot();
}

int vboot_recovery_mode_enabled(void)
{
	return !!(vboot_get_context()->flags & VB2_CONTEXT_RECOVERY_MODE);
}
