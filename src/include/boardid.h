/*
 * This file is part of the coreboot project.
 *
 * Copyright 2014 Google Inc.
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

#ifndef __INCLUDE_BOARDID_H__
#define __INCLUDE_BOARDID_H__

#include <stdint.h>

#define UNDEFINED_STRAPPING_ID (~0)

uint32_t board_id(void);	/* differentiates revisions */
uint32_t ram_code(void);	/* identifies installed DRAM modules */
uint32_t sku_id(void);		/* differentiates other optional components */

#endif /* __INCLUDE_BOARDID_H__ */
