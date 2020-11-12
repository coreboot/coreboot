/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __AMD_BLOCK_IMAGE_H__
#define __AMD_BLOCK_IMAGE_H__

#include <stdint.h>

void *amd_find_image(const void *start_address, const void *end_address,
			uint32_t alignment, const char name[8]);

#endif /* __AMD_BLOCK_IMAGE_H__ */
