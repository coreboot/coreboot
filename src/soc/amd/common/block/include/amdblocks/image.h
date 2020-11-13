/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef AMD_BLOCK_IMAGE_H
#define AMD_BLOCK_IMAGE_H

#include <stdint.h>

void *amd_find_image(const void *start_address, const void *end_address,
			uint32_t alignment, const char name[8]);

#endif /* AMD_BLOCK_IMAGE_H */
