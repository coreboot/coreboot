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

int cb_sha_endian(enum vb2_hash_algorithm hash_alg, const uint8_t *data, uint32_t len,
		  uint8_t *digest, enum endian_algorithm endian)
{
	int i;
	int rv;
	uint32_t digest_size;
	uint8_t *result_ptr;
	uint8_t result[VB2_MAX_DIGEST_SIZE];

	switch (hash_alg) {
	case VB2_HASH_SHA1:
	        digest_size = VB2_SHA1_DIGEST_SIZE;
		break;
	case VB2_HASH_SHA256:
	        digest_size = VB2_SHA256_DIGEST_SIZE;
		break;
	case VB2_HASH_SHA512:
	        digest_size = VB2_SHA512_DIGEST_SIZE;
		break;
	default:
		return VB2_ERROR_SHA_INIT_ALGORITHM;
	}

	result_ptr = result;
	rv = vb2_digest_buffer(data, len, hash_alg, result_ptr, digest_size);
	if (rv || (endian == NO_ENDIAN_ALGORITHM))
		return rv;

	for (i = 0; i < digest_size; ++i) {
		if (endian == BIG_ENDIAN_ALGORITHM) {
			/* use big endian */
			digest[i] = *result_ptr++;
		} else {
			/* use little endian */
			digest[digest_size - i - 1] = *result_ptr++;
		}
	}
	return rv;
}
