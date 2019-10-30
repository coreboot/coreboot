/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2018-2019, Eltan B.V.
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

#ifndef __SECURITY_CB_SHA_H__
#define __SECURITY_CB_SHA_H__

#include <2rsa.h>
#include <vb21_common.h>
#include <vb2_api.h>

vb2_error_t cb_sha_little_endian(enum vb2_hash_algorithm hash_alg, const uint8_t *data,
				 uint32_t len, uint8_t *digest);

#endif
