/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <bootmode.h>
#include <security/vboot/misc.h>
#include <vb2_api.h>

static int gfx_init_done = -1;

int gfx_get_init_done(void)
{
	if (gfx_init_done < 0)
		return 0;
	return gfx_init_done;
}

void gfx_set_init_done(int done)
{
	gfx_init_done = done;
}

int display_init_required(void)
{
	/* For vboot, honor VB2_CONTEXT_DISPLAY_INIT. */
	if (CONFIG(VBOOT)) {
		/*
		 * Display init requires VBOOT_MUST_REQUEST_DISPLAY || VBOOT_ALWAYS_ENABLE_DISPLAY;
		 * else assert build.
		 */
		if (!CONFIG(VBOOT_MUST_REQUEST_DISPLAY) && !CONFIG(VBOOT_ALWAYS_ENABLE_DISPLAY))
			dead_code();
		return vboot_get_context()->flags & VB2_CONTEXT_DISPLAY_INIT;
	}

	/* By default always initialize display. */
	return 1;
}
