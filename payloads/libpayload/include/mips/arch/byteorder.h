/*
 * This file is part of the libpayload project.
 *
 * Copyright (C) 2014 Imagination Technologies
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

#ifndef __MIPS_ARCH_BYTEORDER_H__
#define __MIPS_ARCH_BYTEORDER_H__

#include <stdint.h>
#include <swab.h>

#ifndef __ORDER_LITTLE_ENDIAN__
#error "What endian are you!?"
#endif

#define cpu_to_le64(x) ((uint64_t)(x))
#define le64_to_cpu(x) ((uint64_t)(x))
#define cpu_to_le32(x) ((uint32_t)(x))
#define le32_to_cpu(x) ((uint32_t)(x))
#define cpu_to_le16(x) ((uint16_t)(x))
#define le16_to_cpu(x) ((uint16_t)(x))
#define cpu_to_be64(x) swab64(x)
#define be64_to_cpu(x) swab64(x)
#define cpu_to_be32(x) swab32((x))
#define be32_to_cpu(x) swab32((x))
#define cpu_to_be16(x) swab16((x))
#define be16_to_cpu(x) swab16((x))

#endif /* __MIPS_ARCH_BYTEORDER_H__ */
