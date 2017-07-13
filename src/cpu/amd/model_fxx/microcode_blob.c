/*
 * This file is part of the coreboot project.
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

unsigned char microcode[] __attribute__((aligned(16))) = {
#if !IS_ENABLED(CONFIG_K8_REV_F_SUPPORT)
#include "../../../../3rdparty/blobs/cpu/amd/model_fxx/microcode.h"
#endif
};
