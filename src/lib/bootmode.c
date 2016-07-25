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

#include <rules.h>
#include <bootmode.h>
#include <vendorcode/google/chromeos/chromeos.h>

#if ENV_RAMSTAGE
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
#endif

int display_init_required(void)
{
	/* For Chrome OS always honor vboot_handoff_skip_display_init(). */
	if (IS_ENABLED(CONFIG_CHROMEOS))
		return !vboot_handoff_skip_display_init();

	/* By default always initialize display. */
	return 1;
}
