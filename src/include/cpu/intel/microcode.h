/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 The ChromiumOS Authors.  All rights reserved.
 * Copyright (C) 2000 Ronald G. Minnich
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
#ifndef __CPU__INTEL__MICROCODE__
#define __CPU__INTEL__MICROCODE__

#include <stdint.h>

void intel_update_microcode_from_cbfs(void);
/* Find a microcode that matches the revision and platform family returning
 * NULL if none found. */
const void *intel_microcode_find(void);
/* It is up to the caller to determine if parallel loading is possible as
 * well as ensuring the microcode matches the family and revision (i.e. with
 * intel_microcode_find()). */
void intel_microcode_load_unlocked(const void *microcode_patch);

/* SoC specific check to determine if microcode update is really
 * required, will skip microcode update if true. */
int soc_skip_ucode_update(u32 currrent_patch_id, u32 new_patch_id);

/* return the the version of the currently running microcode */
uint32_t get_current_microcode_rev(void);

/* extract microcode revision from the given patch */
uint32_t get_microcode_rev(const void *microcode);
/* extract microcode size from the given patch */
uint32_t get_microcode_size(const void *microcode);
/* extract checksum from the given patch */
uint32_t get_microcode_checksum(const void *microcode);
#endif
