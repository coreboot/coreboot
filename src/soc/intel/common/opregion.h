/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Intel Inc.
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

#ifndef _COMMON_OPREGION_H_
#define _COMMON_OPREGION_H_

#include <drivers/intel/gma/opregion.h>

/* Loads vbt and initializes opregion. Returns non-zero on success */
int init_igd_opregion(igd_opregion_t *opregion);

#endif /* _COMMON_OPREGION_H_ */
