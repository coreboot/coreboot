/*
 * This file is part of the coreboot project.
 *
 * Copyright 2016 Google Inc.
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

#ifndef _COMMONLIB_FSP_H_
#define _COMMONLIB_FSP_H_

#include <stddef.h>
#include <stdint.h>

/*
 * Relocate FSP held within buffer defined by size to new_addr. Returns < 0
 * on error, offset to FSP_INFO_HEADER on success.
 */
ssize_t fsp_component_relocate(uintptr_t new_addr, void *fsp, size_t size);

/* API to relocate fsp 1.1 component. */
ssize_t fsp1_1_relocate(uintptr_t new_addr, void *fsp, size_t size);

#endif
