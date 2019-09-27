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

#ifndef __DEVICE_PNP_TYPE_H__
#define __DEVICE_PNP_TYPE_H__

#include <stdint.h>

typedef u32 pnp_devfn_t;

#define PNP_DEV(PORT, FUNC) (((PORT) << 8) | (FUNC))

#if defined(__SIMPLE_DEVICE__)
#define ENV_PNP_SIMPLE_DEVICE 1
#else
#define ENV_PNP_SIMPLE_DEVICE 0
#endif

#endif /* __DEVICE_PNP_TYPE_H__ */
