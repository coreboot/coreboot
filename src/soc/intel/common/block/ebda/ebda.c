/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017 Intel Corporation.
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

#include <arch/ebda.h>
#include <intelblocks/ebda.h>
#include <string.h>

/*
 * Mainboard Override function
 *
 * Mainboard directory may implement below functionality for romstage.
 */

/* Fill up EBDA structure inside Mainboard directory */
__attribute__((weak)) void create_mainboard_ebda(struct ebda_config *cfg)
{
	/* no-op */
}

static void create_soc_ebda(struct ebda_config *cfg)
{
	/* Create EBDA header */
	cfg->signature = EBDA_SIGNATURE;
	/* Fill up memory layout information */
	fill_soc_memmap_ebda(cfg);
}

void fill_ebda_area(void)
{
	struct ebda_config ebda_cfg;

	/* Initialize EBDA area early during romstage. */
	setup_default_ebda();
	create_soc_ebda(&ebda_cfg);
	create_mainboard_ebda(&ebda_cfg);
	write_ebda_data(&ebda_cfg, sizeof(ebda_cfg));
}

void retrieve_ebda_object(struct ebda_config *cfg)
{
	read_ebda_data(cfg, sizeof(*cfg));

	if (cfg->signature != EBDA_SIGNATURE)
		memset(cfg, 0, sizeof(*cfg));
}
