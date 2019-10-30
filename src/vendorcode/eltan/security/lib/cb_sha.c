/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2019 Eltan B.V.
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

#include <cb_sha.h>

vb2_error_t cb_sha_little_endian(enum vb2_hash_algorithm hash_alg, const uint8_t *data,
				 uint32_t len, uint8_t *digest)
{
	int i;
	int rv;
	uint32_t digest_size = vb2_digest_size(hash_alg);
	uint8_t result[VB2_MAX_DIGEST_SIZE];

	if (!digest_size)
		return VB2_ERROR_SHA_INIT_ALGORITHM;

	rv = vb2_digest_buffer(data, len, hash_alg, (uint8_t *)&result, digest_size);
	if (rv)
		return rv;

	for (i = 0; i < digest_size; ++i) {
		/* use little endian */
		digest[digest_size - i - 1] = result[i];
	}
	return rv;
}
