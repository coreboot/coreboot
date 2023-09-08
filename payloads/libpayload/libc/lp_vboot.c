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
