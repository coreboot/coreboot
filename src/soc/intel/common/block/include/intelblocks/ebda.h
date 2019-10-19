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

#ifndef SOC_INTEL_COMMON_BLOCK_EBDA_H
#define SOC_INTEL_COMMON_BLOCK_EBDA_H

#define EBDA_SIGNATURE 0xebdaebda

/* EBDA structure */
struct ebda_config {
	uint32_t signature; /* EBDA signature */
	uint32_t cbmem_top; /* coreboot memory start */
};

/* Initialize EBDA and store structure into EBDA area */
void initialize_ebda_area(void);

/*
 * Fill the ebda object pointed to by cfg. Object will be zero filled
 * if signature check fails. */
void retrieve_ebda_object(struct ebda_config *cfg);

/* API for filling ebda with data in romstage */
void fill_memmap_ebda(struct ebda_config *cfg);

#endif
