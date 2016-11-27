/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Timothy Pearson <tpearson@raptorengineeringinc.com>, Raptor Engineering
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
#ifndef S3UTILS_H
#define S3UTILS_H

#include "../wrappers/mcti.h"
#include "mct_d.h"

#ifdef __RAMSTAGE__
int8_t save_mct_information_to_nvram(void);
void copy_mct_data_to_save_variable(struct amd_s3_persistent_data* persistent_data);
#endif

void calculate_and_store_spd_hashes(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat);
void compare_nvram_spd_hashes(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat);

#endif
