/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Intel Corporation.
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

#ifndef _INTEL_COMMON_VBT_H_
#define _INTEL_COMMON_VBT_H_

#include <commonlib/region.h>
#include <types.h>

/*
 * Returns the CBFS filename of the VBT blob.
 *
 * The default implementation returns "vbt.bin", but other implementations can
 * override this.
 */
const char *mainboard_vbt_filename(void);

/*
 * locate vbt.bin file. Returns a pointer to its content.
 * If vbt_size is non-NULL, also return the vbt's size.
 */
void *locate_vbt(size_t *vbt_size);
/*
 * Returns VBT pointer and mapping after checking prerequisites for Pre OS
 * Graphics initialization
 */
void *vbt_get(void);
#endif
