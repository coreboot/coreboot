/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013-2014 Sage Electronic Engineering, LLC.
 * Copyright (C) 2015-2016 Intel Corporation
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

#ifndef CHIPSET_FSP_UTIL_H
#define CHIPSET_FSP_UTIL_H

#include <fsp.h>

#define FSP_INFO_HEADER_GUID \
  { \
  0x912740BE, 0x2284, 0x4734, {0xB9, 0x71, 0x84, 0xB0, 0x27, 0x35, 0x3F, 0x0C} \
  }

/*
 * The FSP Image ID is different for each platform's FSP and
 * can be used to verify that the right FSP binary is loaded.
 * For the Broadwell-DE FSP, the Image Id is "BDX-DE".
 */
#define FSP_IMAGE_ID_DWORD0 ((unsigned int)(FSP_IMAGE_ID))
#define FSP_IMAGE_ID_DWORD1 ((unsigned int)(FSP_IMAGE_ID >> 32))

#endif /* CHIPSET_FSP_UTIL_H */
