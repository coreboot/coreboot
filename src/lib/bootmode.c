/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 The ChromiumOS Authors.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <assert.h>
#include <bootmode.h>
#include <vendorcode/google/chromeos/chromeos.h>

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
	/* For vboot, always honor VBOOT_WD_FLAG_DISPLAY_INIT. */
	if (CONFIG(VBOOT)) {
		/* Must always select MUST_REQUEST_DISPLAY when using this
		   function. */
		if (!CONFIG(VBOOT_MUST_REQUEST_DISPLAY))
			dead_code();
		return vboot_get_working_data()->flags
			& VBOOT_WD_FLAG_DISPLAY_INIT;
	}

	/* By default always initialize display. */
	return 1;
}
