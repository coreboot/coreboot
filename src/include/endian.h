/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef _ENDIAN_H_
#define _ENDIAN_H_

#include <arch/byteorder.h>
#include <stdint.h>
#include <string.h>
#include <swab.h>

/* This include depends on previous ones, do not reorder. */
#include <commonlib/bsd/_endian.h>

#define be64_to_cpu(x) be64toh(x)
#define be32_to_cpu(x) be32toh(x)
#define be16_to_cpu(x) be16toh(x)
#define le64_to_cpu(x) le64toh(x)
#define le32_to_cpu(x) le32toh(x)
#define le16_to_cpu(x) le16toh(x)
#define cpu_to_be64(x) htobe64(x)
#define cpu_to_be32(x) htobe32(x)
#define cpu_to_be16(x) htobe16(x)
#define cpu_to_le64(x) htole64(x)
#define cpu_to_le32(x) htole32(x)
#define cpu_to_le16(x) htole16(x)

#endif
