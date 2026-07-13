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

/*
 * Record the boot mode in cbmem for payload.
 */
enum boot_mode_t get_boot_mode(void)
{
	static bool initialized = false;
	static enum boot_mode_t boot_mode = LB_BOOT_MODE_NORMAL;
	if (!initialized) {
		enum boot_mode_t *boot_mode_ptr = cbmem_find(CBMEM_ID_BOOT_MODE);
		if (boot_mode_ptr)
			boot_mode = *boot_mode_ptr;
		printk(BIOS_INFO, "Boot mode is %d\n", boot_mode);
		initialized = true;
	}
	return boot_mode;
}
