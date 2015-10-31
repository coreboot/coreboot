/*
 * This file is part of the coreboot project.
 *
 * Copyright 2015 Google Inc.
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

PHDRS
{
	to_load PT_LOAD;
}

/*
 * For CONFIG_SEPARATE_VERSTAGE romstage doesn't have the cache-as-ram setup.
 * It only contains the teardown code. The verstage has the cache-as-ram setup
 * code. Therefore, it needs the protected_start symbol as its entry point.
 * The romstage entry will be named _start for consistency, but it's likely
 * to be implemented in the chipset code in order to control the logic flow.
 */
#if IS_ENABLED(CONFIG_SEPARATE_VERSTAGE)
	#if ENV_RAMSTAGE || ENV_RMODULE || ENV_ROMSTAGE
	ENTRY(_start)
	#elif ENV_VERSTAGE
	ENTRY(protected_start)
	#endif
#else
	#if ENV_RAMSTAGE || ENV_RMODULE
	ENTRY(_start)
	#elif ENV_ROMSTAGE
	ENTRY(protected_start)
	#endif
#endif
