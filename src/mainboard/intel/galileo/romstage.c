/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
 * Copyright (C) 2015-2016 Intel Corp.
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

#if IS_ENABLED(CONFIG_PLATFORM_USES_FSP1_1)
#include <fsp/romstage.h>

/* All FSP specific code goes in this block */
void mainboard_romstage_entry(struct romstage_params *rp)
{
	/* Call back into chipset code with platform values updated. */
	romstage_common(rp);
}
#endif /* IS_ENABLED(CONFIG_PLATFORM_USES_FSP1_1) */
