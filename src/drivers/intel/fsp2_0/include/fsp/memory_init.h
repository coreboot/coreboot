/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2018 Facebook Inc
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

#ifndef _FSP2_0_MEMORY_INIT_H_
#define _FSP2_0_MEMORY_INIT_H_

#include <types.h>

/*
 * Updates mrc cache hash if it differs.
 */
void mrc_cache_update_hash(const uint8_t *data, size_t size);

/*
 * Verifies mrc cache hash which is stored somewhere.
 * return 1 verification was successful and 0 for error.
 */
int mrc_cache_verify_hash(const uint8_t *data, size_t size);

#endif /* _FSP2_0_MEMORY_INIT_H_ */
