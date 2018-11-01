/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2018 Intel Corporation.
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

#ifndef SOC_EBDA_H
#define SOC_EBDA_H

#include <stdint.h>

struct ebda_config {
	uint32_t signature; /* 0x00 - EBDA signature */
	uint32_t tolum_base; /* 0x04 - coreboot memory start */
	uint32_t reserved_mem_size; /* 0x08 - chipset reserved memory size */
};

#endif
