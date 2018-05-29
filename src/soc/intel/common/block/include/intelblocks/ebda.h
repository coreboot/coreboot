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

#include <soc/ebda.h>

#define EBDA_SIGNATURE 0xebdaebda

/*
 * Mainboard Override function
 *
 * Mainboard directory may implement below functionality for romstage.
 */

/* Fill up EBDA structure inside Mainboard directory */
void create_mainboard_ebda(struct ebda_config *cfg);

/*
 * SoC overrides
 *
 * All new SoC must implement below functionality for romstage.
 */
void fill_soc_memmap_ebda(struct ebda_config *cfg);

/*
 * API to perform below operation
 * 1. Initialize EBDA area
 * 2. Fill up EBDA structure inside SOC directory
 * 3. Fill up EBDA structure inside Mainboard directory
 * 4. Store EBDA structure into EBDA area
 */
void fill_ebda_area(void);

/* Fill the ebda object pointed to by cfg. Object will be zero filled
 * if signature check fails. */
void retrieve_ebda_object(struct ebda_config *cfg);

/*
 * EBDA structure
 *
 * SOC should implement EBDA structure as per need
 * as below.
 *
 * Note: First 4 bytes should be reserved for signature as
 * 0xEBDA
 *
 * struct ebda_config {
 *	uint32_t signature;
 *	<Required variables..>
 * };
 */

#endif
