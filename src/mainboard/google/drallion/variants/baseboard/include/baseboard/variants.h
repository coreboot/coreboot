/*
 * This file is part of the coreboot project.
 *
 * Copyright 2019 Google LLC
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

#ifndef BASEBOARD_VARIANTS_H
#define BASEBOARD_VARIANTS_H

#include <fsp/api.h>

void variant_mainboard_post_init_params(FSPM_UPD *mupd);

#endif /* BASEBOARD_VARIANTS_H */
