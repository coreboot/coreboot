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

void initialize_ebda_area(void)
{
	struct ebda_config ebda_cfg;

	/* Initialize EBDA area early during romstage. */
	setup_default_ebda();
	ebda_cfg.signature = EBDA_SIGNATURE;
	fill_memmap_ebda(&ebda_cfg);
	write_ebda_data(&ebda_cfg, sizeof(ebda_cfg));
}

void retrieve_ebda_object(struct ebda_config *cfg)
{
	read_ebda_data(cfg, sizeof(*cfg));

	if (cfg->signature != EBDA_SIGNATURE)
		memset(cfg, 0, sizeof(*cfg));
}
